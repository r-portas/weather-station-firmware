/**
 * Handles digital input and output
 */
#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>

#include "digital.h"
#include "eeprom.h"

uint8_t value;

// The three values to display
uint8_t digit1 = 1;
uint8_t digit2 = 13;
uint8_t digit3 = 12;

void increment_digit() {
    digit = (digit + 1) % 3;
}

void set_digit(int val, int digitNo) {
    switch (digitNo) {
        case 1:
            digit1 = val;
            break;

        case 2:
            digit2 = val;
            break;

        case 3:
            digit3 = val;
            break;
    }
}

/**
 * Sets the input and output digital pins
 */
void init_digital_pins() {
    MCUCR = (1<<JTD);
    MCUCR = (1<<JTD);

    DDRD |= (1 << PD5);
    // D4 an input
    DDRD &= ~(1 << PD4);

    DDRB |= (1 << PB2 | 1 << PB3);

    setup_display();
}

/**
 * Reads the power button state
 * returns either 0 or 1
 */
int read_power_button() {
    // 16 = 2^4
    int val = (PIND & (1 << PD4)) >> 4;
    return val;
}

void set_power_state(power_state state) {
    switch (state) {
        case POWER_ON:
            PORTB &= ~(1 << PORTB3);
            break;

        case POWER_OFF:
            PORTB |= (1 << PORTB3);
            break;
    }
}

/**
 * Reads the power button state
 * returns either 0 or 1
 */
int read_save_button() {
    int val = (PIND & (1 << PD5)) >> 5;
    return val;
}

/**
 * Sets the indicator led
 */
void set_indicator_pin(indicator_state state) {
    switch(state) {
        case ON:
            PORTB |= (1 << PORTB2);
            break;

        case OFF:
            PORTB &= ~(1 << PORTB2);
            break;

        case TOGGLE:
            PORTB ^= (1 << PORTB2);
            break;
    }
}

int get_airflow(void) {
    int freq;
    // number of turns per second
    freq = TCNT0;
    if ((TIFR1 & (1 << OCF1A)) == 0) {
        // Its invalid
    } else {
        TCNT0 = 0;
        TIFR1 &= (1 << OCF1A);
    }

    return freq;
}

// Seven segment display
// a - PC1, b - PC2, c - PC3, d - PC4, e - PC5, f - PD6, g - PD7

void show_seven_seg() {

    switch (digit) {
        case 0:
            PORTB |= (1<<5)|(1<<6);
            PORTB &= ~(1<<7);
            display_digit(digit1);
            break;
        case 1:
            PORTB |= (1<<5)|(1<<7);
            PORTB &= ~(1<<6);
            display_digit(digit2);
            break;
        case 2:
            PORTB |= (1<<7)|(1<<6);
            PORTB &= ~(1<<5);
            display_digit(digit3);
            break;
    }
}

int a = 0;
int b = 5;
int c = 6;
int d = 4;
int e = 3;
int f = 2;
int g = 1;
int dp = 7;

void display_digit(uint8_t number){

    switch (number) {

        case 0:
            PORTC |= (1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e)|(1<<f);
            PORTC &= ~(1<<g);
            break;
        case 1:
            PORTC &= ~((1<<a)|(1<<d)|(1<<e));
            PORTC |= (1<<b)|(1<<c);
            PORTC &= ~((1<<f)|(1<<g));
            break;
        case 2:
            PORTC &= ~(1<<c);
            PORTC |= (1<<a)|(1<<b)|(1<<d)|(1<<e);
            PORTC |= (1<<g);
            PORTC &= ~(1<<f);
            break;
        case 3:
            PORTC |= (1<<a)|(1<<b)|(1<<c)|(1<<d);
            PORTC &= ~(1<<e);
            PORTC |= (1<<g);
            PORTC &= ~(1<<f);
            break;
        case 4:
            PORTC &= ~((1<<a)|(1<<d)|(1<<e));
            PORTC |= (1<<b)|(1<<c);
            PORTC |= (1<<g)|(1<<f);
            break;
        case 5:
            PORTC &= ~((1<<b)|(1<<e));
            PORTC |= (1<<a)|(1<<c)|(1<<d);
            PORTC |= (1<<f)|(1<<g);
            break;
        case 6:
            PORTC &= ~(1<<b);
            PORTC |= (1<<a)|(1<<c)|(1<<d)|(1<<e);
            PORTC |= (1<<f)|(1<<g);
            break;
        case 7:
            PORTC &= ~((1<<d)|(1<<e));
            PORTC |= (1<<a)|(1<<b)|(1<<c);
            PORTC &= ~((1<<f)|(1<<g));
            break;
        case 8:
            PORTC |= (1<<a)|(1<<d)|(1<<b)|(1<<c)|(1<<e);
            PORTC |= (1<<f)|(1<<g);
            break;
        case 9:
            PORTC |= (1<<a)|(1<<f)|(1<<b)|(1<<g);
            PORTC |= (1<<c);
            PORTC &= ~(1<<d|1>>e);
            break;
        case 10:
            // Turn decimal point on
            PORTC |= (1 << dp);
            break;
        case 11:
            // Turn decimal point off
            PORTC &= ~(1 << dp);
            break;
        case 12:
            // -
            PORTC |= (1<<g);
            PORTC &= ~((1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e)|(1<<f)|(1<<dp));
            break;
        case 13:
            // All on
            PORTC |= (1<<a)|(1<<d)|(1<<b)|(1<<c)|(1<<e);
            PORTC |= (1<<f)|(1<<g)|(1<<dp);
            break;
        case 14:
            // All leds off
            PORTC &= ~((1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e)|(1<<f)|(1<<g)|(1<<dp));
            break;
        case 20:
            PORTC |= (1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<e)|(1<<f)|(1<<dp);
            PORTC &= ~(1<<g);
            break;
        case 21:
            PORTC &= ~((1<<a)|(1<<d)|(1<<e));
            PORTC |= (1<<b)|(1<<c)|(1<<dp);
            PORTC &= ~((1<<f)|(1<<g));
            break;
        case 22:
            PORTC &= ~(1<<c);
            PORTC |= (1<<a)|(1<<b)|(1<<d)|(1<<e)|(1<<dp);
            PORTC |= (1<<g);
            PORTC &= ~(1<<f);
            break;
        case 23:
            PORTC |= (1<<a)|(1<<b)|(1<<c)|(1<<d)|(1<<dp);
            PORTC &= ~(1<<e);
            PORTC |= (1<<g);
            PORTC &= ~(1<<f);
            break;
        case 24:
            PORTC &= ~((1<<a)|(1<<d)|(1<<e));
            PORTC |= (1<<b)|(1<<c)|(1<<dp);
            PORTC |= (1<<g)|(1<<f);
            break;
        case 25:
            PORTC &= ~((1<<b)|(1<<e));
            PORTC |= (1<<a)|(1<<c)|(1<<d)|(1<<dp);
            PORTC |= (1<<f)|(1<<g);
            break;
        case 26:
            PORTC &= ~(1<<b);
            PORTC |= (1<<a)|(1<<c)|(1<<d)|(1<<e);
            PORTC |= (1<<f)|(1<<g)|(1<<dp);
            break;
        case 27:
            PORTC &= ~((1<<d)|(1<<e));
            PORTC |= (1<<a)|(1<<b)|(1<<c)|(1<<dp);
            PORTC &= ~((1<<f)|(1<<g));
            break;
        case 28:
            PORTC |= (1<<a)|(1<<d)|(1<<b)|(1<<c)|(1<<e);
            PORTC |= (1<<f)|(1<<g)|(1<<dp);
            break;
        case 29:
            PORTC |= (1<<a)|(1<<f)|(1<<b)|(1<<g);
            PORTC |= (1<<c)|(1<<dp);
            PORTC &= ~(1<<d|1>>e);
            break;
    }
}


/**
 * Sets up the seven segment display
 */
void setup_display () {
    /* Set (all pins) to be outputs */
    DDRC |= (1<<0) | (1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7);

    /* Set digit pins */
    DDRB |= (1<<5)|(1<<6)|(1<<7);

}

/**
 * Updates the seven segment display with the firmware's current state
 */
void seven_segment_run (int display_value){
    /* Output the current digit */
    if(digit == 0) {
        /* Extract the ones place from the timer counter 0 value */
        value = display_value %10;

    } else if (digit == 1) {
        /* Extract the tens place from the timer counter 0 */
        value = (display_value/10) % 10;

    } else if (digit == 2) {
        value = (display_value/100);
    }

    display_digit(value);
    /* Change the digit flag for next time. if 0 becomes 1, if 1 becomes 0. */


}
