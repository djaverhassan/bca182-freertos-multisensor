System Architecture
Overview
The RoomSense Monitor uses the STM32F103C8T6 Blue Pill as the main controller.

The STM32 receives environmental and user-input data from the DHT22, LDR, PIR sensor, and rotary encoder. FreeRTOS tasks process these inputs and coordinate the OLED display and buzzer outputs.

Hardware Architecture
```mermaid
flowchart LR

    DHT[DHT22\nTemperature + Humidity]
    LDR[LDR\nAmbient Light]
    PIR[PIR Sensor\nMotion Detection]
    ENC[Rotary Encoder\nUser Input]

    MCU[STM32F103C8T6\nBlue Pill]

    OLED[SSD1306 OLED\nInformation Display]
    BUZ[Buzzer\nTemperature Alarm]
    SERIAL[Serial Monitor\nDiagnostics]

    DHT --> MCU
    LDR --> MCU
    PIR --> MCU
    ENC --> MCU

    MCU --> OLED
    MCU --> BUZ
    MCU --> SERIAL 
