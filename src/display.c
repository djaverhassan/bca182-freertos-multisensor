#include <string.h>
#include <stdbool.h>
#include "display.h"
#include "ssd1306.h"
#include "sensors.h"
#include "logic.h"
#include "rtos_objects.h"
#include "log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#define DISPLAY_WAIT_MS  100U

/* Convert a fixed-point tenths value to display text, e.g. 254 -> "25.4". */
static void FormatTenths(char *buf, int32_t v)
{
    char tmp[12];
    int i = 0;
    int neg = (v < 0);
    if (neg) { v = -v; }

    tmp[i++] = (char)('0' + (v % 10)); v /= 10;
    tmp[i++] = '.';
    do { tmp[i++] = (char)('0' + (v % 10)); v /= 10; } while (v > 0);
    if (neg) { tmp[i++] = '-'; }

    int j = 0;
    while (i > 0) { buf[j++] = tmp[--i]; }
    buf[j] = '\0';
}

static void FormatUint(char *buf, uint32_t v)
{
    char tmp[12];
    int i = 0;
    do { tmp[i++] = (char)('0' + (v % 10)); v /= 10; } while (v > 0);

    int j = 0;
    while (i > 0) { buf[j++] = tmp[--i]; }
    buf[j] = '\0';
}

static int32_t ToTenths(float v)
{
    return (int32_t)(v * 10.0f + ((v >= 0.0f) ? 0.5f : -0.5f));
}

static void Render(const SensorData *d, bool haveData, DisplayMode mode, bool alarm)
{
    char value[16];

    if (!haveData)
    {
        strcpy(value, "...");
    }
    else
    {
        switch (mode)
        {
            case DISPLAY_TEMPERATURE:
                if (d->dhtValid) { FormatTenths(value, ToTenths(d->temperature)); strcat(value, " C"); }
                else             { strcpy(value, "--.- C"); }
                break;

            case DISPLAY_HUMIDITY:
                if (d->dhtValid) { FormatTenths(value, ToTenths(d->humidity)); strcat(value, " %"); }
                else             { strcpy(value, "--.- %"); }
                break;

            case DISPLAY_LIGHT:
                FormatUint(value, (uint32_t)d->lightLevel);
                strcat(value, " %");
                break;

            case DISPLAY_MOTION:
            default:
                strcpy(value, d->motionDetected ? "YES" : "NO");
                break;
        }
    }

    SSD1306_Clear();
    SSD1306_DrawString(0, 0,  "ROOM STATUS", 1);
    SSD1306_DrawString(0, 16, displayModeName(mode), 1);
    SSD1306_DrawString(0, 32, value, 2);
    if (alarm)
    {
        SSD1306_DrawString(0, 56, "! TEMP LIMIT", 1);
    }
    (void)SSD1306_Update();
}

/* DisplayTask exclusively owns the OLED. In ACTIVE mode it refreshes when
   data or the selected view changes. In INACTIVE mode the panel is powered
   down and the task waits for reactivation. */
void DisplayTask(void *argument)
{
    (void)argument;
    SensorData data = {0};
    bool haveData = false;
    DisplayMode mode = DISPLAY_TEMPERATURE;
    bool lastAlarm = false;

    if (SSD1306_Init())
    {
        Log("[OLED] Display ready\r\n");
    }
    else
    {
        Log("[OLED] Display setup failed\r\n");
    }
    Render(&data, haveData, mode, lastAlarm);

    for (;;)
    {
        EventBits_t bits = xEventGroupGetBits(systemEvents);

        if ((bits & EVENT_ACTIVE) == 0)
        {
            SSD1306_SetPower(false);
            Log("[OLED] Power off - INACTIVE\r\n");

            (void)xEventGroupWaitBits(systemEvents, EVENT_ACTIVE, pdFALSE, pdTRUE, portMAX_DELAY);

            SSD1306_SetPower(true);
            Log("[OLED] Power on - ACTIVE\r\n");

            /* Refresh any values or selected view that changed while the panel was off. */
            (void)xQueueReceive(sensorToDisplayQueue, &data, 0);
            DisplayMode m;
            if (xQueueReceive(displayModeQueue, &m, 0) == pdPASS) { mode = m; }
            lastAlarm = (xEventGroupGetBits(systemEvents) & EVENT_ALARM) != 0;
            Render(&data, haveData, mode, lastAlarm);
            continue;
        }

        bool changed = false;
        DisplayMode newMode;

        if (xQueueReceive(sensorToDisplayQueue, &data, pdMS_TO_TICKS(DISPLAY_WAIT_MS)) == pdPASS)
        {
            haveData = true;
            changed = true;
        }

        if (xQueueReceive(displayModeQueue, &newMode, 0) == pdPASS && newMode != mode)
        {
            mode = newMode;
            changed = true;
        }

        bool alarm = (xEventGroupGetBits(systemEvents) & EVENT_ALARM) != 0;
        if (alarm != lastAlarm)
        {
            lastAlarm = alarm;
            changed = true;
        }

        if (changed)
        {
            Render(&data, haveData, mode, lastAlarm);
        }
    }
}