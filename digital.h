/**
 * Handles digital input and output
 */

#ifndef DIGITAL_H
#define DIGITAL_H

typedef enum indicator_state_t {
    ON,
    OFF,
    TOGGLE
} indicator_state;

typedef enum power_state_t {
    POWER_ON,
    POWER_OFF
} power_state;

uint8_t digit; /* 2 = left, 1 = middle, 0 = right */

/**
 * Sets the indicator led
 */
void set_indicator_pin(indicator_state state);

/**
 * Sets up the input and output digital pins
 */
void init_digital_pins();

/* Record airflow per second */
int get_airflow (void);

void set_up_display (void);

void seven_segment_run (int display_value);

void display_digit(uint8_t number);

void setup_display();

void increment_digit();

void show_seven_seg();

int read_power_button();

void set_power_state(power_state state);

int read_save_button();

void set_digit(int val, int digitNo);

#endif
