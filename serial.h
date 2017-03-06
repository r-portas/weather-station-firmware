/**
 * Handles all serial communication
 */

#ifndef SERIAL_H
#define SERIAL_H

// Derived from datasheet
#define MYUBRR 51

/**
 * Transmits the payload over serial in JSON format
 */
void transmit_payload(int timestamp, int air, int temp, int light, char lat[10], char lng[10], int save);

int get_length();
/**
 * Initializes serial communication
 */
void init_serial(void);

/**
 * Transmits a byte over USART
 */
void transmit_byte(unsigned char byte);

/**
 * Receive a byte over USART
 */
void receive_byte(void);
 
/**
 * Debug print to the serial port
 */
void debug_print(char* str);

void echo();

char read_from_gps(void);
#endif
