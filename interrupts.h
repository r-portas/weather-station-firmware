/**
 * Contains all interrupt specific code
 */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

void init_interrupts();

uint16_t get_time_delta();

int windSpeed;

int time;
uint16_t sys_clock;
char raw_gps[200];
int gps_packet_counter;

char lat[30];
char lng[30];

#endif
