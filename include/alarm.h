#ifndef ALARM_H
#define ALARM_H

/* AlarmTask owns the buzzer and checks each valid temperature reading
   against the configured 18 C to 30 C operating range. */
void AlarmTask(void *argument);

#endif