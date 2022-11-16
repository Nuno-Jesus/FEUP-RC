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

/**
 * @brief Creates a new Alarm struct, assigning the parameters to the new struct
 * @param handler A pointer to the function to work as handler
 * @param timeout Number of seconds to count between each trigger
 * @returns The new Alarm struct
 */
Alarm *new_alarm(void (*)(int), unsigned int);

/**
 * @brief Frees the memory of an Alarm struct
 * @param a The Alarm struct to delete
 */
void delete_alarm(Alarm *);

/**
 * @brief Uses signal function to initialize the alarms
 */
void set_alarm();

/**
 * @brief Starts a new alarm based on the timeout defined on an Alarm struct.
 * It also sets the number of triggers to 0.
 * @param a The Alarm struct where the timeout variable is stored at
 */
void start_alarm(Alarm *);

/**
 * @brief Cancels all the alarms 
 */
void stop_alarm();

/**
 * @brief This is the default handler the alarm function will call
 * @param signal (unused)
 */
void default_handler(int);

#endif