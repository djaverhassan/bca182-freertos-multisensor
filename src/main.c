#include "stm32f1xx_hal.h"
#include "log.h"
#include "sensors.h"
#include "input.h"
#include "motion.h"
#include "app.h"

void Error_Handler(void)
{
    __disable_irq();
    for (;;) { }
}

/* PC13 status LED: provides a simple visual fault indication. */
static void StatusLed_Init(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef g = {0};
    g.Pin   = GPIO_PIN_13;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOC, &g);

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* off */
}

/* Initialize board peripherals, then transfer control to the RTOS application. */
int main(void)
{
    SCB->VTOR = FLASH_BASE;

    HAL_Init();
    StatusLed_Init();
    Log_Init();
    Sensors_Init();
    Input_Init();
    Motion_Init();

    Log("BCA182 RoomSense Monitor\r\n");

    app_main();   /* creates RTOS resources, launches tasks, then starts scheduling */

    for (;;) { }
}