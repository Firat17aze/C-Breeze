/**
 * IoT Smart Fan Control System - Arduino Uno Main Program
 * Bare Metal C Implementation (No Arduino Framework)
 * ATmega328P Register-Level Programming
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "uart.h"
#include "ultrasonic.h"

// Pin Definitions (ATmega328P)
#define FAN_IN1     PD4  // Pin 4 - L293D Input 1
#define FAN_IN2     PD3  // Pin 3 - L293D Input 2
#define FAN_ENABLE  PD5  // Pin 5 - L293D Enable (PWM capable)

// Operating Modes
#define MODE_AUTO   0
#define MODE_MANUAL 1

// System State
volatile uint8_t system_mode = MODE_AUTO;
volatile uint8_t fan_state = 0;  // 0 = OFF, 1 = ON
volatile uint8_t camera_activated = 0;  // Camera detection signal
volatile uint16_t current_distance = 0;  // Distance in cm (calculated)
volatile uint16_t current_time_us = 0;   // Time in microseconds (from sensor)
volatile uint8_t manual_fan_command = 0;  // 0 = OFF, 1 = ON
volatile uint8_t invalid_reading_count = 0;  // Counter for consecutive invalid readings

// Distance threshold (50cm default)
#define DISTANCE_THRESHOLD_CM 70
#define HYSTERESIS_CM 5

// Time thresholds for ultrasonic sensor (ROUND TRIP TIME in microseconds)
// Sound speed = 343 m/s = 0.0343 cm/µs = 29.1 µs/cm
// Round trip: time_us = distance_cm * 2 / 0.0343 = distance_cm * 58.3
// For 70cm: 70 * 58.3 = 4081µs round trip
// For 75cm (with hysteresis): 75 * 58.3 = 4372µs round trip
#define TIME_THRESHOLD_MAX_US 4081    // 70cm - fan ON when time LESS than this
#define TIME_THRESHOLD_OFF_US 4372    // 75cm - fan OFF when time MORE than this

// Fan timeout (10 minutes = 600000 ms, but we'll use a counter)
#define FAN_TIMEOUT_COUNT 60000  // Adjust based on loop frequency

// Grace period when person moves out of range (3 seconds)
// With ~10ms loop, 3 seconds = 300 iterations
#define OUT_OF_RANGE_GRACE_COUNT 300

// Measure distance every 4 seconds (400 iterations at ~10ms each)
#define DISTANCE_MEASURE_INTERVAL 400

volatile uint32_t fan_on_time = 0;
volatile uint16_t out_of_range_counter = 0;  // Counter for grace period
volatile uint16_t measure_counter = 0;  // Counter for distance measurement interval

/**
 * Initialize GPIO pins for fan control
 */
void init_fan_control(void) {
    // Set PD3, PD4, PD5 as outputs (Fan control pins)
    DDRD |= (1 << FAN_IN1) | (1 << FAN_IN2) | (1 << FAN_ENABLE);
    
    // Initialize all pins to LOW (fan off)
    PORTD &= ~((1 << FAN_IN1) | (1 << FAN_IN2) | (1 << FAN_ENABLE));
    
    // Configure Timer0 for PWM on PD5 (OC0B)
    // Fast PWM mode, non-inverting, prescaler 64
    TCCR0A = (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64
    OCR0B = 255;  // 100% duty cycle initially
}

/**
 * Turn fan ON (forward rotation)
 */
void fan_on(void) {
    PORTD |= (1 << FAN_IN1);   // Set IN1 HIGH
    PORTD &= ~(1 << FAN_IN2);  // Set IN2 LOW
    OCR0B = 255;  // Full speed (PWM controls ENABLE automatically)
    fan_state = 1;
    fan_on_time = 0;
}

/**
 * Turn fan OFF
 */
void fan_off(void) {
    OCR0B = 0;  // Stop PWM (disables fan)
    // Also set both inputs LOW for safety
    PORTD &= ~((1 << FAN_IN1) | (1 << FAN_IN2));
    fan_state = 0;
}

/**
 * Process serial commands from laptop
 */
void process_serial_command(char cmd) {
    switch (cmd) {
        case 'A':
        case 'H':
            // Camera detected human in close range
            camera_activated = 1;
            break;
            
        case 'D':
        case 'L':
            // No human detected or out of range
            camera_activated = 0;
            break;
            
        case 'M':
            // Switch to manual mode
            system_mode = MODE_MANUAL;
            camera_activated = 0;  // Disable camera detection
            uart_send_string("MODE:MANUAL\n");
            break;
            
        case 'O':
            // Switch to auto mode
            system_mode = MODE_AUTO;
            uart_send_string("MODE:AUTO\n");
            break;
            
        case 'F':
            // Fan command prefix - wait for next character
            manual_fan_command = 'F';
            break;
            
        case '1':
            // Could be camera signal or fan ON command
            if (manual_fan_command == 'F') {
                // This is F1 - Manual fan ON
                if (system_mode == MODE_MANUAL) {
                    fan_on();
                    uart_send_string("FAN:ON\n");
                }
                manual_fan_command = 0;
            } else {
                // This is camera signal - human detected
                camera_activated = 1;
            }
            break;
            
        case '0':
            // Could be camera signal or fan OFF command
            if (manual_fan_command == 'F') {
                // This is F0 - Manual fan OFF
                if (system_mode == MODE_MANUAL) {
                    fan_off();
                    uart_send_string("FAN:OFF\n");
                }
                manual_fan_command = 0;
            } else {
                // This is camera signal - no human detected
                camera_activated = 0;
            }
            break;
            
        default:
            // Unknown command - reset prefix
            manual_fan_command = 0;
            break;
    }
}

/**
 * Send status update to laptop
 */
void send_status_update(void) {
    // Always send time for real-time updates (in microseconds)
    uart_send_string("TIME:");
    uart_send_number(current_time_us);
    uart_send_string("\n");
    
    // Always send fan status in AUTO mode for dashboard consistency
    if (system_mode == MODE_AUTO) {
        if (fan_state) {
            uart_send_string("FAN:ON\n");
        } else {
            uart_send_string("FAN:OFF\n");
        }
    }
}

/**
 * Main control loop
 */
int main(void) {
    // Initialize all subsystems
    uart_init(9600);
    ultrasonic_init();
    init_fan_control();
    
    // Enable global interrupts
    sei();
    
    // Startup message
    uart_send_string("SYSTEM:READY\n");
    uart_send_string("MODE:AUTO\n");
    
    // Main loop
    while (1) {
        // Read serial commands
        if (uart_available()) {
            char cmd = uart_receive();
            process_serial_command(cmd);
        }
        
        // Measure distance only every 4 seconds (saves CPU, more stable)
        measure_counter++;
        if (measure_counter >= DISTANCE_MEASURE_INTERVAL) {
            measure_counter = 0;
            current_time_us = ultrasonic_measure();
            
            // Convert to cm
            if (current_time_us > 0 && current_time_us < 25000) {
                current_distance = current_time_us / 58;
            }
        }
        
        if (system_mode == MODE_AUTO) {
            // Auto mode: Fan works when BOTH conditions are true:
            // 1. Camera detects human (camera_activated = 1)
            // 2. Distance is within 50cm (time < 2915µs)
            //
            // SPECIAL CASE: If time_us is 0 or very small (<200µs ~3cm), 
            // the sensor can't measure (too close). If camera sees human, assume in range.
            //
            // GRACE PERIOD: If person moves out of range, wait 3 seconds before turning off
            
            uint8_t in_range;
            if (current_time_us == 0 || current_time_us < 200) {
                // Sensor reading invalid or very close (<3cm)
                // If camera sees human, assume they're in range (too close to measure)
                in_range = camera_activated;
            } else {
                // Valid sensor reading - check if within 50cm threshold
                in_range = (current_time_us <= TIME_THRESHOLD_MAX_US);
            }
            uint8_t conditions_met = camera_activated && in_range;
            
            if (conditions_met) {
                // Both conditions met - turn on fan and reset grace period counter
                out_of_range_counter = 0;
                if (fan_state == 0) {
                    fan_on();
                    fan_on_time = 0;
                }
            } else if (fan_state == 1) {
                // Fan is ON but conditions not met - check grace period
                if (!camera_activated) {
                    // No human detected by camera - turn off immediately
                    fan_off();
                    out_of_range_counter = 0;
                    fan_on_time = 0;
                } else if (!in_range) {
                    // Human detected but out of range - use grace period
                    out_of_range_counter++;
                    if (out_of_range_counter >= OUT_OF_RANGE_GRACE_COUNT) {
                        // Grace period expired - turn off fan
                        fan_off();
                        out_of_range_counter = 0;
                        fan_on_time = 0;
                    }
                    // Otherwise keep fan running during grace period
                }
            }
            
            // Check for maximum runtime timeout (10 minutes)
            if (fan_state == 1) {
                fan_on_time++;
                if (fan_on_time > FAN_TIMEOUT_COUNT) {
                    fan_off();
                    uart_send_string("AUTO:TIMEOUT\n");
                    fan_on_time = 0;
                    out_of_range_counter = 0;
                }
            }
        } else {
            // Manual mode: Only respond to manual commands
            // Manual commands are handled in process_serial_command
            // Distance is still measured for display
        }
        
        // Send status updates every ~2 seconds
        static uint16_t status_counter = 0;
        if (++status_counter >= 200) {
            send_status_update();
            status_counter = 0;
        }
        
        // 10ms delay per loop iteration
        _delay_ms(10);
    }
    
    return 0;
}

