/**
 * Handles all ADC operations
 */

void init_adc();

uint16_t read_adc(uint8_t adc_channel);

int convert_to_degrees(uint16_t raw);
