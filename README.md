# Sumobot Project

An autonomous sumo robot built on the STM32U575 microcontroller. The robot searches an arena for an opponent, drives forward to push it out of the ring, and uses line sensors to stay inside the ring boundary.

## Overview

This is bare-metal firmware (direct register programming, no HAL) for an STM32U5 series MCU. The robot combines three subsystems to play autonomous sumo: dual-motor drive with PWM speed control and GPIO direction control, two downward-facing line sensors to detect the ring edge, and an infrared object detector to find the opponent.

The high-level behavior: the bot spins in place searching for an object. Once the IR detector spots one, it drives forward to attack and push it out. When a line sensor detects the ring boundary, it backs up, turns, and resumes searching, keeping itself safely inside the ring.

## Hardware

- MCU: STM32U575 (STM32U5 family)
- 2x DC motors with a motor driver
- 2x reflectance line sensors (analog)
- 1x IR object detector
- 1x LCD display (used for sensor calibration and readout)

## Peripheral Usage

- TIM2 (CH1 and CH2): PWM generation for motor speed control at 3 kHz, output on PA0 and PA1.
- GPIO PA3, PA4, PB3, PB4: motor direction control, two pins per motor.
- ADC4 (channels 5 and 6): reads the two analog line sensors, with an interrupt to switch between channels.
- TIM3: 38 kHz toggle-mode square wave for the IR sensor.
- TIM4: gates TIM3 on and off via capture/compare and update interrupts.

## Repository Structure

- `Inc/sumobot.h` — function prototypes and peripheral declarations
- `Src/main.c` — main control loop and behavior pattern
- `Src/sumobot.c` — core driver: motors, ADC, timers, and behavior functions
- `Src/stm32u5xx_it.c` — interrupt service routines
- `Src/test_motors.c` — standalone test for motor drive
- `Src/test_Lsensors.c` — standalone test for line sensors with LCD readout
- `Src/test_obj_detect.c` — standalone test for IR object detection
- `Src/test_behavior.c` — standalone test for the full search-and-push behavior
- `Src/test_lcd.c` — standalone test for the LCD display

## Behavior Logic

The line sensors return reflected-light intensity as a percentage, which is used to set thresholds that distinguish the dark ring surface from the white boundary line. The control loop then runs the following pattern: spin in place to search for an opponent; on IR detection, drive forward to push it; on line detection at the ring edge, back up and turn away; then resume spinning and searching.

## Building and Flashing

This is a register-level project for the STM32U575 and can be built and flashed with STM32CubeIDE, or with an arm-none-eabi-gcc toolchain plus a debugger such as ST-Link. Import the project (or add the files under Src and Inc to your build), make sure the include path contains the Inc folder, build, and flash to the board over ST-Link.

## Testing

Each test file contains its own entry point to exercise a single subsystem in isolation. Build with one test file at a time, excluding main.c, to verify motors, sensors, IR detection, behavior, or the LCD independently before running the full firmware.

## Author

Created by nikiemao, 2024.
