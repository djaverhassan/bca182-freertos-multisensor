# BCA182 Room Monitor

## Project Overview

RoomSense Monitor is a simulated real-time room monitoring system built around the STM32F103C8T6 Blue Pill microcontroller.

The project uses FreeRTOS to separate sensing, display control, user input, motion monitoring, alarm handling, and system-state management into independent tasks.

The system is developed using PlatformIO with the STM32Cube framework and simulated using Wokwi.

## Features

- Temperature and humidity monitoring using DHT22
- Ambient light monitoring using an LDR
- Motion detection using a PIR sensor
- SSD1306 OLED information display
- Rotary encoder navigation
- Temperature alarm using a buzzer
- ACTIVE and INACTIVE operating states
- Automatic inactivity detection
- Automatic reactivation after motion
- FreeRTOS queues for inter-task communication
- Event groups for system events
- Mutex protection for shared serial output

## Learning Objectives

This project demonstrates:

- STM32 development using PlatformIO
- STM32 HAL peripheral programming
- FreeRTOS task creation and scheduling
- Task priorities and blocking
- Queue-based inter-task communication
- Mutex synchronization
- Event-group signaling
- Periodic execution using `vTaskDelayUntil()`
- Embedded state-machine design
- Unit testing and static analysis
- Wokwi-based embedded-system simulation

## System Architecture

The STM32 Blue Pill acts as the central controller.

The main input devices are:

- DHT22
- LDR
- PIR motion sensor
- Rotary encoder

The output devices are:

- SSD1306 OLED
- Buzzer
- Serial Monitor

Sensor information is processed by FreeRTOS tasks and communicated between subsystems using queues and event groups.

## FreeRTOS Architecture

The application is divided into multiple FreeRTOS tasks:

- `SensorTask` — reads environmental sensors
- `DisplayTask` — manages the OLED display
- `InputTask` — processes rotary encoder input
- `MotionTask` — monitors PIR activity
- `AlarmTask` — evaluates temperature alarm conditions
- `StateTask` — manages ACTIVE and INACTIVE states

The application also uses queues, mutexes, and event groups to coordinate the tasks.

## Hardware / Simulated Components

| Component | Purpose |
|---|---|
| STM32 Blue Pill | Main microcontroller |
| DHT22 | Temperature and humidity |
| LDR | Ambient light |
| PIR Sensor | Motion detection |
| Rotary Encoder | User navigation |
| SSD1306 OLED | Information display |
| Buzzer | Temperature alarm |

## Pin Configuration

| Component | STM32 Pin |
|---|---|
| LDR Analog Output | PA0 |
| DHT22 Data | PA1 |
| Buzzer | PA2 |
| PIR Output | PA3 |
| Encoder CLK | PA4 |
| Encoder DT | PA5 |
| OLED SCL | PB6 |
| OLED SDA | PB7 |
| USART1 TX | PA9 |
| USART1 RX | PA10 |

## Task Design

Different task priorities are used according to how quickly each subsystem needs to respond.

Motion detection and user input require relatively fast response, while display updates can tolerate more delay.

Tasks block or delay when they have no immediate work instead of continuously consuming CPU time.

## Inter-Task Communication

FreeRTOS queues are used to transfer sensor information between tasks.

The system also uses:

- Event groups for system-state and alarm events
- A mutex to protect shared Serial output
- Blocking and periodic delays to coordinate task execution

## State Machine

The system supports two major operating states:

### ACTIVE

- OLED enabled
- Sensors processed normally
- Encoder navigation enabled
- Temperature alarm enabled

### INACTIVE

- OLED disabled or blank
- Unnecessary display activity reduced
- Motion monitoring remains enabled

Motion detected while INACTIVE returns the system to ACTIVE.

## Repository Structure

```text
.
├── docs/
├── include/
├── lib/
├── src/
├── test/
├── diagram.json
├── platformio.ini
├── wokwi.toml
└── README.md