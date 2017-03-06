/**
 * (EEPROM) Data saving and loading
 * @author Sze-Nung Leung
 */

#include <stdio.h>
#include <string.h>
#include <avr/eeprom.h>

#include "serial.h"
#include "eeprom.h"

// The row of data that are most recently updated
int updated_row = 0; // max 6
// The row to write data to
int write_to_row = 0;

/**
 * Save the string in payload to eeprom
 */
void save_to_eeprom(char payload[150]) {
    int i;
    for (i = 0; i < strlen(payload); i++) {
        eeprom_update_byte((uint8_t*)((write_to_row* ROW_BYTE)+i), payload[i]);
    }
    while (i < 150) {
        eeprom_update_byte((uint8_t*)((write_to_row* ROW_BYTE)+i), '\0');
        i++;
    }
    write_to_row = (write_to_row + 1) % 6;
    if (write_to_row != 0) {
        updated_row = write_to_row - 1;
    } else {
        updated_row = 5;
    }
}

/**
 * Load the string read from eeprom into from_eeprom
 */
void load_from_eeprom(char from_eeprom[150]) {
    for (int i = 0; i < 150; i++) {
        from_eeprom[i] = eeprom_read_byte((uint8_t*)((updated_row * ROW_BYTE)+i));
    }
}

void get_eeprom_row(char from_eeprom[150], int row) {
    for (int i = 0; i < 150; i++) {
        from_eeprom[i] = eeprom_read_byte((uint8_t*)((row * ROW_BYTE)+i));
    }
}

void transmit_row(int row) {
    for (int i = 0; i < 150; i++) {
        transmit_byte(eeprom_read_byte((uint8_t*)((row * ROW_BYTE)+i)));
    }

}

int get_current_eeprom_row() {
    return updated_row;
}

int set_current_eeprom_row(int val) {
    updated_row = val;
}
