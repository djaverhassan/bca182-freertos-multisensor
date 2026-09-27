#include <stdbool.h>
#include "alarm.h"
#include "buzzer.h"
#include "logic.h"
#include "sensors.h"
#include "rtos_objects.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#define ALARM_WAIT_MS  250U

void AlarmTask(void *argument)
{
    (void)argument;
    SensorData d;
    AlarmState state = ALARM_NORMAL;

    if (!Buzzer_Init())
    {
        Log("[TempAlarm] Buzzer setup failed\r\n");
    }

    for (;;)
    {
        /* Wait for updated sensor data, with a short timeout so INACTIVE mode
           can silence the buzzer without noticeable delay. */
        if (xQueueReceive(sensorToAlarmQueue, &d, pdMS_TO_TICKS(ALARM_WAIT_MS)) == pdPASS)
        {
            /* Invalid DHT22 samples do not create a new alarm condition. */
            if (d.dhtValid)
            {
                AlarmState newState = evaluateTemperature(d.temperature);   /* decision logic only */

                if (newState != state)
                {
                    state = newState;

                    Log_Begin();
                    Log("[TempAlarm] Status: ");
                    Log(alarmStateName(state));
                    Log("\r\n");
                    Log_End();

                    if (state != ALARM_NORMAL) { (void)xEventGroupSetBits(systemEvents, EVENT_ALARM); }
                    else                       { (void)xEventGroupClearBits(systemEvents, EVENT_ALARM); }
                }
            }
        }

        /* Audible alarm output is enabled only while the monitor is ACTIVE. */
        bool active = (xEventGroupGetBits(systemEvents) & EVENT_ACTIVE) != 0;
        Buzzer_Set(active && state != ALARM_NORMAL);                        /* buzzer output */
    }
}