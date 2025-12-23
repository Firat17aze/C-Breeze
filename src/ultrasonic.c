/**
 * Ultrasonic Sensor (HC-SR04) - SIMPLE VERSION
 * One measurement at a time, no complex filtering
 * Call every ~4 seconds from main loop
 */

#include "ultrasonic.h"
#include <avr/io.h>
#include <util/delay.h>

// Pin definitions - ORIGINAL PINS (Pin 9 trigger, Pin 10 echo)
#define TRIGGER_PIN  PB1  // Pin 9
#define ECHO_PIN     PB2  // Pin 10

// Last valid reading
static uint16_t last_good = 0;

/**
 * Initialize sensor
 */
void ultrasonic_init(void) {
    // Trigger = OUTPUT, Echo = INPUT
    DDRB |= (1 << TRIGGER_PIN);
    DDRB &= ~(1 << ECHO_PIN);
    PORTB &= ~((1 << TRIGGER_PIN) | (1 << ECHO_PIN));
    
    // Timer1 for timing (prescaler 8 = 0.5µs/tick)
    TCCR1A = 0;
    TCCR1B = (1 << CS11);
    TCNT1 = 0;
    
    _delay_ms(50);
}

/**
 * Take ONE measurement
 * Returns round-trip time in microseconds
 */
uint16_t ultrasonic_measure(void) {
    uint16_t i;
    uint16_t start, end;
    
    // Wait if echo stuck HIGH
    for (i = 0; i < 1000; i++) {
        if (!(PINB & (1 << ECHO_PIN))) break;
        _delay_us(10);
    }
    if (i >= 1000) return last_good;
    
    // Send 10µs trigger pulse
    PORTB &= ~(1 << TRIGGER_PIN);
    _delay_us(5);
    TCNT1 = 0;
    PORTB |= (1 << TRIGGER_PIN);
    _delay_us(10);
    PORTB &= ~(1 << TRIGGER_PIN);
    
    // Wait for echo to go HIGH
    for (i = 0; i < 10000; i++) {
        if (PINB & (1 << ECHO_PIN)) break;
        _delay_us(1);
    }
    if (i >= 10000) return last_good;
    start = TCNT1;
    
    // Wait for echo to go LOW
    for (i = 0; i < 25000; i++) {
        if (!(PINB & (1 << ECHO_PIN))) break;
        _delay_us(1);
    }
    if (i >= 25000) return last_good;
    end = TCNT1;
    
    // Calculate microseconds
    uint16_t ticks = (end >= start) ? (end - start) : (65535 - start + end);
    uint16_t us = ticks / 2;
    
    // Valid range check (2cm to 400cm)
    if (us >= 116 && us <= 23200) {
        last_good = us;
        return us;
    }
    
    return last_good;
}

