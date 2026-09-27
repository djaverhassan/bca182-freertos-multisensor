#include "sensors.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"
#include "dht22.h"
#include "ldr.h"
#include "rtos_objects.h"
#include "log.h"

#define SENSOR_PERIOD_MS  2000U

void Sensors_Init(void)
{
    DHT22_Init();
    LDR_Init();
}

/* SensorTask samples the DHT22 and LDR every 2 seconds. vTaskDelayUntil()
   keeps acquisition aligned to a fixed period instead of accumulating drift. */
void SensorTask(void *argument)
{
    (void)argument;
    SensorData data = {0};
    Dht22Reading r;
    uint16_t raw;
    TickType_t lastWakeTime = xTaskGetTickCount();

    for (;;)
    {
        if (DHT22_Read(&r))
        {
            data.temperature = (float)r.temp_x10 / 10.0f;
            data.humidity    = (float)r.hum_x10 / 10.0f;
            data.dhtValid    = true;
        }
        else
        {
            data.dhtValid = false;      /* retain the old values but mark the sample invalid */
            Log("[EnvSensor] DHT22 sample failed\r\n");
        }

        if (LDR_ReadRaw(&raw))
        {
            data.lightLevel = LDR_RawToPercent(raw);
        }

        /* Include the current PIR state published by MotionTask. */
        data.motionDetected = (xEventGroupGetBits(systemEvents) & EVENT_PIR_LEVEL) != 0;

        /* Length-1 overwrite queues keep only the latest sensor snapshot. Separate
           queues let the display and alarm consume data independently. */
        xQueueOverwrite(sensorToDisplayQueue, &data);
        xQueueOverwrite(sensorToAlarmQueue, &data);

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(SENSOR_PERIOD_MS));
    }
}