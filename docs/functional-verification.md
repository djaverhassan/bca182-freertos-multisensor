# Functional Verification

## Overview

The RoomSense Monitor was verified in Wokwi by changing simulated sensor values and observing the OLED, buzzer, rotary encoder behavior, motion detection, and ACTIVE / INACTIVE state transitions.

The following functional tests correspond to the required laboratory verification cases.

---

## Verification Table

| Test ID | Input / Stimulus | Expected Result | Actual Observation | Result |
|---|---|---|---|---|
| FT-01 | Set DHT22 temperature to 28 째C | Displayed temperature updates | OLED displayed the updated temperature and the value remained within the normal range | PASS |
| FT-02 | Change DHT22 humidity value | Displayed humidity updates | Humidity page reflected the new simulated humidity value | PASS |
| FT-03 | Change LDR / photoresistor input | Light value changes | Light reading changed in response to the simulated LDR level | PASS |
| FT-04 | Rotate encoder clockwise | Next display page is selected | Display advanced through Temperature, Humidity, Light, and Motion pages | PASS |
| FT-05 | Rotate encoder counterclockwise | Previous display page is selected | Display moved through the pages in reverse order with wraparound | PASS |
| FT-06 | Set temperature above 30 째C | Alarm activates | Buzzer activated and the system reported a high-temperature condition | PASS |
| FT-07 | Return temperature to normal range | Alarm stops | Buzzer stopped after the temperature returned to the normal range | PASS |
| FT-08 | Trigger PIR motion | System is ACTIVE | Motion event was detected and the system remained or became ACTIVE | PASS |
| FT-09 | Allow inactivity timeout | System becomes INACTIVE | After the inactivity period, the system transitioned to INACTIVE and display activity was reduced | PASS |
| FT-10 | Trigger PIR while INACTIVE | System returns to ACTIVE | Motion reactivated the system and normal display behavior resumed | PASS |

---

## Notes

The system was tested using the simulated STM32 Blue Pill environment in Wokwi.

The verification focused on observable application behavior:

- sensor value updates
- display navigation
- alarm activation and recovery
- motion detection
- inactivity handling
- automatic reactivation

The temperature alarm uses the required thresholds:

- 18 째C lower limit
- 30 째C upper limit