#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

/* StateTask manages ACTIVE/INACTIVE behavior and publishes the current mode
   through EVENT_ACTIVE. */
void StateTask(void *argument);

#endif