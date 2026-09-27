#include "app.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_objects.h"
#include "sensors.h"
#include "display.h"
#include "input.h"
#include "alarm.h"
#include "motion.h"
#include "system_state.h"
#include "log.h"

#define TASK_STACK_WORDS       256

/* Task priority values reflect response urgency; larger values run first. */
#define MOTION_TASK_PRIORITY   3   /* prompt response to PIR activity */
#define INPUT_TASK_PRIORITY    3   /* keeps encoder navigation responsive */
#define STATE_TASK_PRIORITY    3   /* handles activity-state changes quickly */
#define SENSOR_TASK_PRIORITY   2   /* periodic environmental sampling */
#define ALARM_TASK_PRIORITY    2   /* evaluates incoming temperature readings */
#define DISPLAY_TASK_PRIORITY  1   /* display refresh is least time-critical */

void app_main(void)
{
    Log("Initializing RoomSense...\r\n");

    if (!RTOS_Objects_Create())
    {
        Log("ERROR: RTOS resource setup failed\r\n");
        for (;;) { }
    }

    BaseType_t ok = pdPASS;
    ok &= xTaskCreate(MotionTask,  "PIR Watch",  TASK_STACK_WORDS, NULL, MOTION_TASK_PRIORITY,  NULL);
    ok &= xTaskCreate(StateTask,   "System State",   TASK_STACK_WORDS, NULL, STATE_TASK_PRIORITY,   NULL);
    ok &= xTaskCreate(InputTask,   "Encoder Input",   TASK_STACK_WORDS, NULL, INPUT_TASK_PRIORITY,   NULL);
    ok &= xTaskCreate(SensorTask,  "Env Sensor",  TASK_STACK_WORDS, NULL, SENSOR_TASK_PRIORITY,  NULL);
    ok &= xTaskCreate(AlarmTask,   "Temp Alarm",   TASK_STACK_WORDS, NULL, ALARM_TASK_PRIORITY,   NULL);
    ok &= xTaskCreate(DisplayTask, "OLED Display", TASK_STACK_WORDS, NULL, DISPLAY_TASK_PRIORITY, NULL);

    if (ok != pdPASS)
    {
        Log("ERROR: application task setup failed\r\n");
        for (;;) { }
    }

    Log("RTOS resources and tasks ready\r\n");
    Log("Launching scheduler...\r\n");
    vTaskStartScheduler();

    Log("ERROR: scheduler did not start\r\n");
    for (;;) { }
}