#include "alarm.h"

Alarm *a;

void default_handler(int signal)
{
	#ifdef DEBUG
		printf("Timeout #%d occured.\n", a->counter);
	#endif

	a->counter++;
	alarm(a->timeout);
}

Alarm *new_alarm(void (*handler)(int), unsigned int timeout)
{
	Alarm *alarm = (Alarm *)malloc(sizeof(Alarm));
	if(!alarm)
		return NULL;

	alarm->counter = 0;
	alarm->timeout = timeout;
	alarm->handler = !handler ? default_handler : handler;

	return alarm;
}

void delete_alarm(Alarm *alarm)
{
	free(alarm);
}

void set_alarm()
{
	(void)signal(SIGALRM, a->handler);
}

void start_alarm(Alarm *a)
{
	alarm(a->timeout);
	#ifdef DEBUG
		printf("Started alarm with %lu seconds\n", a->timeout);
	#endif
}

void stop_alarm()
{
	alarm(0);
	#ifdef DEBUG
		printf("Alarm stopped.\n");
	#endif
}