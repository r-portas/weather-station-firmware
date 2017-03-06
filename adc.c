/*
 * Handles all ADC operations
 *
 * @author Roy Portas
 */
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>

#define ADC_VOLTAGE 5
#define ADC_RESOLUTION 1023


void init_adc() {
    ADMUX |= (1 << REFS0);

    // Set prescaller to 128
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
}

/**
 * Converts the raw value to degrees
 */
int convert_to_degrees(uint16_t raw) {
    // Divide by 1000 to get millivolts
    //long millivoltage = (10 * raw * ADC_VOLTAGE) / ADC_RESOLUTION;

    uint16_t temp = (60 * raw) / 204;
    int actTemp = temp - 10;

    // Will return the temperature multiplied by 10
    return actTemp;
}

uint16_t read_adc(uint8_t adc_channel) {

    // Select ADC channel with safety mask
    ADMUX &= 0xF0;
    ADMUX |= adc_channel;

    // Single conversion mode
    ADCSRA |= (1 << ADSC);

    // Wait until ADC conversion is complete
    while (ADCSRA & (1 << ADSC));

    return ADC;
}
