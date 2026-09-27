#include "input.h"
#include "logic.h"
#include "rtos_objects.h"
#include "log.h"
#include "stm32f1xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "event_groups.h"

#define ENC_PORT         GPIOA
#define ENC_CLK_PIN      GPIO_PIN_4
#define ENC_DT_PIN       GPIO_PIN_5
#define INPUT_PERIOD_MS  50U

/* Encoder movement is accumulated by the ISR and consumed by InputTask.
   Positive = clockwise, negative = counterclockwise. */
static volatile int32_t encoderSteps = 0;

void Input_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};

    /* Use the CLK falling edge as the encoder step event. */
    g.Pin  = ENC_CLK_PIN;
    g.Mode = GPIO_MODE_IT_FALLING;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENC_PORT, &g);

    /* DT is sampled at the edge to determine rotation direction. */
    g.Pin  = ENC_DT_PIN;
    g.Mode = GPIO_MODE_INPUT;
    g.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(ENC_PORT, &g);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

void EXTI4_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(ENC_CLK_PIN);
}

/* Keep the ISR short and RTOS-free. DT level identifies the direction. */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == ENC_CLK_PIN)
    {
        if (HAL_GPIO_ReadPin(ENC_PORT, ENC_DT_PIN) == GPIO_PIN_SET)
        {
            encoderSteps++;
        }
        else
        {
            encoderSteps--;
        }
    }
}

/* InputTask checks encoder movement periodically and publishes the selected
   display view. Input is ignored whenever the system is INACTIVE. */
void InputTask(void *argument)
{
    (void)argument;
    DisplayMode mode = DISPLAY_TEMPERATURE;

    xQueueOverwrite(displayModeQueue, &mode);

    for (;;)
    {
        int32_t steps;

        /* Capture and clear the accumulated steps atomically. */
        taskENTER_CRITICAL();
        steps = encoderSteps;
        encoderSteps = 0;
        taskEXIT_CRITICAL();

        bool active = (xEventGroupGetBits(systemEvents) & EVENT_ACTIVE) != 0;

        if (steps != 0 && active)
        {
            while (steps > 0) { mode = nextDisplayMode(mode);     steps--; }
            while (steps < 0) { mode = previousDisplayMode(mode); steps++; }

            xQueueOverwrite(displayModeQueue, &mode);

            Log_Begin();
            Log("[Encoder] View: ");
            Log(displayModeName(mode));
            Log("\r\n");
            Log_End();
        }

        vTaskDelay(pdMS_TO_TICKS(INPUT_PERIOD_MS));
    }
}