/*
 * sumobot.h
 *
 *  Created on: Apr 2, 2024
 *      Author: nikiemao
 */

#ifndef INC_SUMOBOT_H_
#define INC_SUMOBOT_H_

#include "stm32u575xx.h"
#include "stdint.h"


#endif /* INC_SUMOBOT_H_ */

void initBasic(void);
/*
 * basic clock setup
 * @param: none
 * @return: none
 */
void initGPIO(void);
/*
 * initialize GPIO port pins
 * @param: none
 * @return: none
 */
void initTim2(void);
/*
 * initialize timer TIM2 channel 1 and 2 in PWM mode
 * @param: none
 * @return: none
 */
void speed_control(float duty1, float duty2);
/*
 * function to control the speed of 2 motors
 * @param: PWM duty cycle 1 and 2
 * @return: none
 */
void forward (float speed1, float speed2);
/*
 * setting robot to move forward
 * @param: specific speed of each motor
 * @return: none
 */
void backward (float speed1, float speed2);
/*
 * setting robot to move backward
 * @param: specific speed of each motor
 * @return: none
 */
void turn_left (float speed1, float speed2);
/*
 * setting robot to make a left turn
 * @param: specific speed of each motor
 * @return: none
 */
void turn_right (float speed1, float speed2);
/*
 * setting robot to make a right turn
 * @param: specific speed of each motor
 * @return: none
 */
void spin_left (float speed1, float speed2);
/*
 * setting robot to spin left
 * @param: specific speed of each motor
 * @return: none
 */
void spin_right (float speed1, float speed2);
/*
 * setting robot to spin right
 * @param: specific speed of each motor
 * @return: none
 */
void stop (void);
/*
 * setting robot to stop
 * @param: none
 * @return: none
 */
void initAdc(void);
/*
 * initialize the analog to digital converter (ADC)
 * @param: none
 * @return: none
 */
void initInterrupt(void);
/*
 * Initialize ADC interrupt
 * @param: none
 * @return: none
*/
void initTim4(void);
/*
 * Initialize timer TIM4 as an interrupt timer to switch on and off TIM3
 * @param: none
 * @return: none
 */
void initTim3(void);
/*
 * Initialize timer TIM3 in toggle mode for the IR sensor
 * @param: none
 * @return: none
 */
