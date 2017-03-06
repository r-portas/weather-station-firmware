/**
 * Handle interrupts
 *
 * @author Roy Portas
 */

#include <avr/interrupt.h>
#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "digital.h"
#include "serial.h"
#include "interrupts.h"

int second_counter;
// Clock set to 1/10 of a second
int COUNTER_MAX = 200;

// Stores the last ICR value
volatile uint16_t oldICR = 0;
volatile uint16_t icrDelta = 0;
volatile uint16_t icr_counter = 0;

uint16_t sys_clock = 0;

void write_lat_lng(char* str, int isLat) {

    for (int i = 0; i < 10; i++) {

        if (isLat == 1) {
            lat[i] = str[i];
        } else {
            lng[i] = str[i];
        }

        if (str[i] == '\0') {
            break;
        }
    }

}

/**
 * Note: Input Capture is on ICP1 (PORTD6)
 */
void init_airflow(void) {

    // Set up the pin
    DDRD &= ~(1 << PD6);

    TCNT1 = 0;

    /*
    TCCR1B |= (0 << WGM12) | (1 << ICES1) | (4 << CS10);

    TIMSK1 |= (1 << ICIE1) | (1 << TOIE1);
    */

    // Get the rising edge
    TCCR1B |= (1 << ICES1);

    // 64 prescaler
    TCCR1B |= (4 << CS10);

    // Enable ICP interrupt
    TIMSK1 |= ((1 << ICIE1) | (1 << TOIE1));

    // Clear the input capture flag
    TIFR1  &= ~(1 << ICF1);

    gps_packet_counter = 0;

    lat[0] = '\0';
    lng[0] = '\0';
}

void init_interrupts() {

    init_airflow();

    // timer0
    TCCR0A = (1 << WGM01);
    OCR0A = (uint8_t) 125000;

    // Enable timer0 interrupt
    TIMSK0 = (1 << OCIE0A);

    TCNT0 = 0x00;
    // Set the prescaler to 64
    TCCR0B = (1 << CS00) | (1 << CS01);

    time = 0;
    second_counter = 0;

    sei();
}

void clock_notifier() {
    // Add functions to be called every second here
    sys_clock++;
}

ISR(TIMER0_COMPA_vect) {
    increment_digit();
    show_seven_seg();
    second_counter++;
    if (second_counter == COUNTER_MAX) {
        second_counter = 0;
        clock_notifier();
    }

}

ISR(USART1_RX_vect) {
    char recv_byte = UDR1;

    if (recv_byte == '\n') {
        if (raw_gps[0] == '$' &&
            raw_gps[1] == 'G' &&
            raw_gps[2] == 'P' &&
            raw_gps[3] == 'G' &&
            raw_gps[4] == 'G' &&
            raw_gps[5] == 'A') {


                /*
                for (int c = 0; c < gps_packet_counter; c++) {
                    transmit_byte(raw_gps[c]);
                }
                */


                int comma_count = 0;
                int item_counter = 0;
                int i = 0;

                for (int i = 0; i < gps_packet_counter; i++) {

                    char c = raw_gps[i];

                    if (c == ',') {

                        if (comma_count == 2) {
                            lat[item_counter] = '\0';
                        }

                        if (comma_count == 4) {
                            lng[item_counter] = '\0';
                        }

                        comma_count++;
                        item_counter = 0;
                    } else {

                        if (comma_count == 2) {
                            // Latitude
                            lat[item_counter] = c;
                            item_counter++;
                        }

                        if (comma_count == 4) {
                            // Longitude
                            lng[item_counter] = c;
                            item_counter++;
                        }
                    }
                }
        }


        gps_packet_counter = 0;
    } else {
        raw_gps[gps_packet_counter] = recv_byte;
        gps_packet_counter += 1;
    }
}

uint16_t get_time_delta() {
    return icrDelta;
}

/**
 * Clear the value when it overflows
 */
ISR(TIMER1_OVF_vect) {
    windSpeed = 0;
}

ISR(TIMER1_CAPT_vect) {
    TCNT1 = 0;
    icrDelta = ICR1;
}
