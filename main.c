/**
 * Program entrypoint
 *
 * Contains the main statemachine of the application
 *
 * @author Roy Portas
 */

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "adc.h"
#include "serial.h"
#include "digital.h"
#include "interrupts.h"

#define BLINK_DELAY_MS 50

// Stores the states in the state machine
enum state_t {
    WIND_SENSOR,
    LIGHT_SENSOR,
    TEMP_SENSOR,
    GPS,
    BUTTON,
    SERIAL,
    SLEEP,
    SEVEN_SEG
} state = WIND_SENSOR;

enum seven_seg_state_t {
    WIND,
    WIND_BLANK,
    TEMP,
    TEMP_BLANK
} seven_seg_state = WIND;

// Raw temperature value
int temp_adc = 0;
int light_adc = 0;

// Airflow value
int airflow;
uint16_t last_seg_time = 0;

int timestamp = 0;

int power_save_button_state = 0;
int power_save_enabled = 0;

int gps_record_button_state = 0;

/*
 * Processed values
 */
int32_t temperature = -31;
int light = 0;

/**
 * Calculates the temperature
 */
int32_t calculate_temperature(int raw_temp) {
    int32_t t;

    int32_t first = (-5 * raw_temp * raw_temp) / 10000;
    int32_t second = (32 * raw_temp) / 100;
    int32_t third = 92995 / 10000;

    t = first + second - third;
    return t;
}

/**
 * Calculates the windspeed
 */
int calculate_windspeed(int raw_windspeed) {

    /*
    if (raw_windspeed < 0) {
        raw_windspeed = raw_windspeed * -1;
    }
    */

    raw_windspeed = raw_windspeed / 1000;

    return raw_windspeed;
}

/**
 * Calculates the light reading
 */
int calculate_light(int raw_light) {
    return raw_light;
}

/**
 * Checks if the power save button is pressed when the device is turned on
 */
int check_power_save() {
    int button_state = read_power_button();

    if (button_state == 1) {
        set_digit(14, 3);
        set_digit(14, 2);
        set_digit(14, 1);

        set_power_state(POWER_OFF);
        set_indicator_pin(OFF);
        power_save_enabled = 1;
    }

    return button_state;
}

/**
 * The state machine handles the program flow
 *
 * Developer note: Please make sure to sequentially go through each
 * state from top to bottom
 */
void state_machine() {
    switch (state) {
        case WIND_SENSOR:
            // Read the wind sensor

            airflow = get_time_delta();

            // Do processing here
            windSpeed = calculate_windspeed(airflow);

            state = LIGHT_SENSOR;
            break;

        case LIGHT_SENSOR:
            // Read the light sensor

            light_adc = read_adc(0);
            light = calculate_light(light_adc);

            state = TEMP_SENSOR;
            break;

        case TEMP_SENSOR:
            // Read the temperature sensor

            temp_adc = read_adc(1);
            temperature = calculate_temperature(temp_adc);
            //temperature = temp_adc;

            state = GPS;
            break;

        case GPS:
            // This is handled by an interrupt

            state = BUTTON;
            break;

        case BUTTON:
            // Read in the values for the buttons

            /*
               power_save_button_state = read_power_button();

               if (power_save_button_state == 1) {
               if (power_save_enabled == 1) {
               set_power_state(POWER_ON);
               power_save_enabled = 0;

               } else {
               set_digit(14, 3);
               set_digit(14, 2);
               set_digit(14, 1);

               set_power_state(POWER_OFF);
               set_indicator_pin(OFF);
               power_save_enabled = 1;
               }

               _delay_ms(400);

               }
               */

            state = SEVEN_SEG;

            break;

        case SEVEN_SEG:

            switch (seven_seg_state) {
                case WIND:

                    // Stay in this state for 3 seconds
                    if (sys_clock > last_seg_time + 30) {

                        last_seg_time = sys_clock;
                        seven_seg_state = WIND_BLANK;
                    }


                    int t = (windSpeed/100) % 10;
                    if (t == 0) {
                        t = 14;
                    }
                    set_digit(t, 3);

                    t = (windSpeed/10) % 10;
                    if (t == 0) {
                        t = 10;
                    } else {
                        // Offset by 20 to display decimal point
                        t += 20;
                    }
                    set_digit(t, 2);

                    set_digit(windSpeed % 10, 1);

                    break;

                case WIND_BLANK:
                    // Stay in this state for half a second
                    if (sys_clock > last_seg_time + 5) {

                        last_seg_time = sys_clock;
                        seven_seg_state = TEMP;
                    }

                    set_digit(14, 3);
                    set_digit(14, 2);
                    set_digit(14, 1);

                    break;

                case TEMP:
                    // Stay in this state for 1 seconds
                    if (sys_clock > last_seg_time + 10) {

                        last_seg_time = sys_clock;
                        seven_seg_state = TEMP_BLANK;
                    }

                    int temp;
                    if (temperature < 0) {
                        set_digit(12, 3);
                        temp = temperature * -1;
                    } else {
                        set_digit(14, 3);
                        temp = temperature;
                    }
                    set_digit((temp/10) % 10, 2);
                    set_digit(temp % 10, 1);

                    break;

                case TEMP_BLANK:
                    // Stay in this state for half a second
                    if (sys_clock > last_seg_time + 5) {

                        last_seg_time = sys_clock;
                        seven_seg_state = WIND;
                    }


                    set_digit(14, 3);
                    set_digit(14, 2);
                    set_digit(14, 1);

                    break;

            }


            state = SERIAL;
            break;

        case SERIAL:
            // Transmit to the PC over serial

            set_indicator_pin(OFF);
            int save = read_save_button();

            receive_byte();

            transmit_payload(
                    timestamp,
                    windSpeed,
                    temperature,
                    light,
                    lat,
                    lng,
                    save
                    );

            _delay_ms(100);
            set_indicator_pin(ON);


            state = WIND_SENSOR;
            break;
    }
}

/**
 * The main loop of the application
 */
int main (void) {

    init_digital_pins();
    init_adc();
    init_serial();
    init_interrupts();

    set_indicator_pin(ON);
    set_power_state(POWER_ON);

    int digit_test = 0;

    // Check if power save is enabled
    _delay_ms(500);

    if (check_power_save()) {
        // If power save is enabled, send board to sleep and idle
        while(1) {
            _delay_ms(5000);
        }

    } else {
        // Main loop
        while(1) {

            // Ensure the indicator light is on
            state_machine();

            _delay_ms(100);
        }
    }

}

