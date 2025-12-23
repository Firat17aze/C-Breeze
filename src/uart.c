/**
 * UART Communication Module Implementation
 * ATmega328P UART0 - Direct Register Access
 */

#include "uart.h"
#include <avr/io.h>
#include <util/delay.h>

/**
 * Calculate UBRR value for given baud rate
 * Formula: UBRR = (F_CPU / (16 * baud_rate)) - 1
 * For 16MHz clock and 9600 baud: UBRR = (16000000 / (16 * 9600)) - 1 = 103
 */
static uint16_t calculate_ubrr(uint32_t baud_rate) {
    return (F_CPU / (16UL * baud_rate)) - 1;
}

/**
 * Initialize UART
 */
void uart_init(uint32_t baud_rate) {
    uint16_t ubrr = calculate_ubrr(baud_rate);
    
    // Set baud rate
    UBRR0H = (uint8_t)(ubrr >> 8);
    UBRR0L = (uint8_t)(ubrr);
    
    // Enable receiver and transmitter
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
    
    // Wait for UART to stabilize
    _delay_ms(10);
}

/**
 * Send a single character (blocking)
 */
void uart_send(char data) {
    // Wait until transmit buffer is empty
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Put data into buffer, sends the data
    UDR0 = data;
}

/**
 * Send a null-terminated string
 */
void uart_send_string(const char *str) {
    while (*str) {
        uart_send(*str++);
    }
}

/**
 * Send a number as string (0-65535)
 */
void uart_send_number(uint16_t number) {
    char buffer[6];
    uint8_t i = 0;
    
    if (number == 0) {
        uart_send('0');
        return;
    }
    
    // Convert number to string (reverse order)
    while (number > 0 && i < 5) {
        buffer[i++] = '0' + (number % 10);
        number /= 10;
    }
    
    // Send in correct order
    while (i > 0) {
        uart_send(buffer[--i]);
    }
}

/**
 * Check if data is available
 */
uint8_t uart_available(void) {
    return (UCSR0A & (1 << RXC0)) != 0;
}

/**
 * Receive a character (blocking)
 */
char uart_receive(void) {
    // Wait until data is received
    while (!(UCSR0A & (1 << RXC0)));
    
    // Get and return received data
    return UDR0;
}

