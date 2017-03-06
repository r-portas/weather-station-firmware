/**
 * Handles all serial communication
 * @author Roy Portas
 */

#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <float.h>
#include <string.h>

#include "adc.h"
#include "serial.h"
#include "digital.h"
#include "eeprom.h"

// Store the count of the bytes sent
int counter = 0;

// JSON payload
char payload[250];
// saved data
char saved[150];
// The length of the JSON payload
int length;

/**
 * Forms a JSON payload from the sensor values
 */
void create_json_payload(int timestamp, int air, int temp, int light, char lat[10], char lng[10], int save) {
    length = sprintf(payload,
            "{\"Timestamp\": %d, \"Air\": %d, \"Temperature\": %d, \"Light\": %d, \"Latitude\": \"%s\", \"Longitude\": \"%s\"}\n\r",
            timestamp, air, temp, light, lat, lng);
    if (save) {
        save_to_eeprom(payload);
    }
}

/**
 * Prints a string to the serial port
 */
void debug_print(char* str) {
    int length = strlen(str);

    for (int i = 0; i < length; i++) {
        transmit_byte(str[i]);
    }

}

/**
 * Transmits the JSON payload over serial
 */
void transmit_payload(int timestamp, int air, int temp, int light, char lat[10], char lng[10], int save) {

    // Create the json payload
    create_json_payload(timestamp, air, temp, light, lat, lng, save);
    while (counter < length) {
        // Transmit the bytes one at a time
        transmit_byte(payload[counter]);
        counter++;
    }

    counter = 0;
}

int get_length() {
    return length;
}
/**
 * Initializes serial communication
 */
void init_serial(void) {
    UBRR0H  = (unsigned char) (MYUBRR >> 8);
    UBRR0L  = (unsigned char) MYUBRR;

    UCSR0B |= (1<<RXEN0) | (1<<TXEN0);
    UCSR0C |= (1<<USBS0) | (1<<UCSZ00);

    // USART1 for GPS
    UBRR1H  = (unsigned char) (MYUBRR >> 8);
    UBRR1L  = (unsigned char) MYUBRR;


    // 1 stop bit and
    // 8 bit data
    // No parity
    UCSR1B |= ((1<<RXEN1) | (1<<TXEN1) | (1<<RXCIE1));
    UCSR1C |= ((1<<USBS0) | (1 << UCSZ10) | (1 << UCSZ00));
}

char read_from_gps(void) {
    while (!(UCSR1A & (1 << RXC1))) {
        transmit_byte(UDR1);
        return UDR1;
    }
}

void echo() {
    while (1) {
        // Need to filter package to see if its the one we want
        while (!(UCSR1A & (1 << RXC1))) {}
        char recv_byte = UDR1;

        while ((UCSR0A & (1 << UDRE0)) == 0) {}
        UDR0 = recv_byte;

        if (recv_byte == '\n') {
            //break;
        }
    }
}

/**
 * Transmits a byte over USART
 */
void transmit_byte(unsigned char byte) {
    while (!(UCSR0A & (1 << UDRE0)));

    // Queue byte into buffer
    UDR0 = byte;
}

void receive_byte() {
    char c;
    if (UCSR0A & (1 << RXC0)) {
        c = UDR0; // 's'
        if (c == 's' || c == 'S') {
            /*
            transmit_byte('S');
            transmit_byte('t');
            */

            int saved_row = get_current_eeprom_row();

            while (saved_row > 0) {
                transmit_row(saved_row - 1);

                saved_row -= 1;
            }

            // Reset the eeprom row, which deletes the stored values
            set_current_eeprom_row(0);

            /*
            transmit_byte('E');
            transmit_byte('n');
            */

        }

    }
}
