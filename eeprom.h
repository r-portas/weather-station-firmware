/**
 * Handles all serial communication
 */

#ifndef EEPROM_H
#define EEPROM_H

#define ROW_BYTE 150

void load_from_eeprom(char from_eeprom[150]);

void save_to_eeprom(char saved[150]);

void load_from_eeprom(char from_eeprom[150]);

void get_eeprom_row(char from_eeprom[150], int row);

void transmit_row(int row);

int get_current_eeprom_row();

int set_current_eeprom_row(int val);

#endif
