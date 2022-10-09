#include "alarm.h"

Alarm *new_alarm(void (*handler)(int), unsigned int timeout)
{
	Alarm *alarm = (Alarm *)malloc(sizeof(Alarm));
	if(!alarm)
		return NULL;

	alarm->counter = 0;
	alarm->isActive = FALSE;
	alarm->timeout = timeout;
	alarm->handler = handler;
}

void delete_alarm(Alarm *alarm)
{
	free(alarm);
}

void set_alarm(Alarm *alarm)
{
	alarm->isActive = TRUE;
	(void)signal(SIGALRM, alarm->handler);
}

void start_alarm(Alarm *a)
{
	alarm(a->timeout);
	#ifdef DEBUG
		printf("Started alarm with %lu seconds\n", a->timeout);
	#endif
	a->isActive = TRUE;
}

void stop_alarm()
{
	alarm(0);
	#ifdef DEBUG
		printf("Alarm stoped.\n");
	#endif
}