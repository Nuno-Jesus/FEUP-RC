#ifndef ALARM_H
#define ALARM_H

#include "macros.h"

typedef struct alarm_h
{
	unsigned int counter;
	unsigned int timeout;
	void (*handler)(int);
}Alarm;

Alarm *new_alarm(void (*)(int), unsigned int);

void delete_alarm(Alarm *);

void set_alarm();

void start_alarm(Alarm *);

void stop_alarm();

void default_handler(int);

#endif