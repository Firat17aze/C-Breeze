/**
 * Ultrasonic Sensor (HC-SR04) Driver Module
 * Polling-based implementation for reliable measurements
 */

#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdint.h>

/**
 * Initialize ultrasonic sensor
 * Pin 9 (PB1) - Trigger (OUTPUT)
 * Pin 10 (PB2) - Echo (INPUT)
 */
void ultrasonic_init(void);

/**
 * Measure echo time in microseconds (round-trip time)
 * @return: Time in microseconds (0 = error/timeout)
 * To calculate distance: distance_cm = time_us / 58
 */
uint16_t ultrasonic_measure(void);

#endif /* ULTRASONIC_H */

