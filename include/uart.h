/**
 * UART Communication Module (Bare Metal)
 * ATmega328P UART0 Implementation
 */

#ifndef UART_H
#define UART_H

#include <stdint.h>

/**
 * Initialize UART with specified baud rate
 * @param baud_rate: Desired baud rate (e.g., 9600)
 */
void uart_init(uint32_t baud_rate);

/**
 * Send a single character via UART
 * @param data: Character to send
 */
void uart_send(char data);

/**
 * Send a null-terminated string via UART
 * @param str: String to send
 */
void uart_send_string(const char *str);

/**
 * Send a number as string via UART
 * @param number: Number to send
 */
void uart_send_number(uint16_t number);

/**
 * Check if data is available to read
 * @return: 1 if data available, 0 otherwise
 */
uint8_t uart_available(void);

/**
 * Receive a character from UART (blocking)
 * @return: Received character
 */
char uart_receive(void);

#endif /* UART_H */

