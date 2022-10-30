#include "alarm.h"

Alarm *a;

Alarm *new_alarm(void (*handler)(int), unsigned int timeout)
{
	Alarm *alarm = (Alarm *)malloc(sizeof(Alarm));
	if(!alarm)
		return NULL;

	alarm->counter = 0;
	alarm->isActive = FALSE;
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
	a->counter = 0;
	#ifdef DEBUG
		printf("\n\t>>> ALARM STARTED <<<\n\n", a->timeout);
	#endif
}

void stop_alarm()
{
	alarm(0);
	a->isActive = FALSE;

	#ifdef DEBUG
		printf("\n\t>>> ALARM STOPPED <<<\n\n");
	#endif
}

void default_handler(int signal)
{
	#ifdef DEBUG
		printf("\n\t>>>TIMEOUT #%d <<<\n\n", a->counter);
	#endif

	a->counter++;
	a->isActive = FALSE;
	alarm(a->timeout);
}