#ifndef ALARM_H
#define ALARM_H

#include "macros.h"

typedef struct alarm_h
{
	int isActive;
	unsigned int counter;
	unsigned int timeout;
	void (*handler)(int);
}Alarm;

Alarm *new_alarm(void (*)(int), unsigned int);

void delete_alarm(Alarm *);

void set_alarm(Alarm *);

void start_alarm(Alarm *);

#endif