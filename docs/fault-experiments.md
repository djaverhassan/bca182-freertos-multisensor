# FreeRTOS Fault Experiments

## Overview

This document defines the deliberate FreeRTOS fault experiments required for the RoomSense Monitor.

Each experiment temporarily introduces an incorrect design condition so that its effect on scheduling, responsiveness, or shared-resource behavior can be observed.

After each experiment, the correct implementation must be restored.

---

## Experiment 1 — Remove Task Blocking

### Objective

Observe what happens when a continuously executing FreeRTOS task no longer blocks or delays between executions.

### Procedure

1. Select a task that normally uses `vTaskDelay()`, `vTaskDelayUntil()`, or another blocking operation.
2. Temporarily remove or disable the blocking operation.
3. Run the Wokwi simulation.
4. Observe:
   - task execution frequency
   - responsiveness of other tasks
   - serial output behavior
   - possible starvation of lower-priority tasks
5. Restore the original blocking operation.

### Expected Effect

A task that continuously executes without blocking can consume excessive CPU time.

If the task has a sufficiently high priority, lower-priority tasks may receive less processor time or may appear unresponsive.

### Actual Observation

To be recorded after the experiment is performed.

### Result

Pending verification.

---

## Experiment 2 — Increase Task Priority

### Objective

Observe the effect of assigning an unnecessarily high priority to a task that performs frequent work.

### Procedure

1. Choose a task that normally has a moderate or low priority.
2. Temporarily assign it a higher priority.
3. Run the Wokwi simulation.
4. Observe whether:
   - other tasks respond more slowly
   - display updates are affected
   - sensor or input handling behavior changes
   - CPU scheduling behavior becomes less balanced
5. Restore the original task priority.

### Expected Effect

FreeRTOS always gives execution preference to the highest-priority Ready task.

An unnecessarily high priority may reduce the responsiveness of lower-priority tasks if the high-priority task becomes Ready frequently.

### Actual Observation

To be recorded after the experiment is performed.

### Result

Pending verification.

---

## Experiment 3 — Remove Serial Mutex

### Objective

Observe why shared serial output requires synchronization when multiple tasks can print messages.

### Procedure

1. Temporarily remove the `serialMutex` protection from shared serial logging.
2. Run the Wokwi simulation.
3. Allow several tasks to generate diagnostic output.
4. Observe whether messages become:
   - interleaved
   - incomplete
   - difficult to read
5. Restore the mutex protection.

### Expected Effect

Without mutual exclusion, multiple tasks may access the same serial output resource at nearly the same time.

This can cause diagnostic messages from different tasks to become mixed together.

### Actual Observation

To be recorded after the experiment is performed.

### Result

Pending verification.

---

## Restoration Requirement

After each experiment, the normal RoomSense Monitor implementation must be restored.

The final project must retain:

- proper task blocking
- justified task priorities
- mutex protection for shared serial output