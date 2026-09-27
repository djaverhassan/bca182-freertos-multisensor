# Unit Test Coverage

## Overview

The project includes hardware-independent unit tests for the main decision logic used by the RoomSense Monitor.

The tests focus on three areas:

- Temperature alarm evaluation
- Display navigation
- ACTIVE / INACTIVE system-state transitions

A total of 13 meaningful test cases are included.

---

## Temperature Alarm Tests

The required temperature limits are:

- Low temperature limit: 18 °C
- High temperature limit: 30 °C

| Test | Input | Expected Result |
|---|---:|---|
| Below lower threshold | 17.9 °C | Low temperature alarm |
| Exactly lower threshold | 18.0 °C | Normal |
| Normal temperature | 25.0 °C | Normal |
| Exactly upper threshold | 30.0 °C | Normal |
| Above upper threshold | 30.1 °C | High temperature alarm |

These tests verify the boundary conditions as well as normal operating temperature behavior.

---

## Display Navigation Tests

The rotary encoder cycles through the following display pages:

Temperature → Humidity → Light → Motion

The reverse direction follows the opposite sequence.

| Test | Starting Page | Action | Expected Page |
|---|---|---|---|
| Forward navigation | Temperature | Next | Humidity |
| Forward wraparound | Motion | Next | Temperature |
| Reverse navigation | Humidity | Previous | Temperature |
| Reverse wraparound | Temperature | Previous | Motion |

These tests verify both normal transitions and wraparound behavior.

---

## System State Tests

The application supports two main operating states:

- ACTIVE
- INACTIVE

| Test | Current State | Condition | Expected State |
|---|---|---|---|
| No inactivity timeout | ACTIVE | Timeout not reached | ACTIVE |
| Inactivity timeout | ACTIVE | Timeout reached | INACTIVE |
| No motion while inactive | INACTIVE | No motion | INACTIVE |
| Motion while inactive | INACTIVE | Motion detected | ACTIVE |

These tests verify the core state-transition logic used by the motion and state-management tasks.

---

## Test Summary

Total test coverage:

- Temperature alarm logic: 5 tests
- Display navigation: 4 tests
- System state logic: 4 tests
- Total: 13 tests

The tests focus on deterministic application logic that can be verified independently from the simulated STM32 hardware.