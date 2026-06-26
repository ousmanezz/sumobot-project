/*
 * sumobot.c
 *
 *  Created on: Apr 2, 2024
 *      Author: nikiemao
 *
 *      			***Functions Declarations***
 *      This file contains all the function used to implementing the behavior of the sumobot and
 *      initialization of the A/D converter used to read the line sensors values and the TIM3 and TIM4 to
 *      read to IR object sensor data.
 *      The A/D uses two channels (channel 5 and channel 6) setup in initADC() with an interrupt to allow the
 *      selection of the specific channel to read from. Each channel is connected to an analog line sensor.
 *      TIM4 is used to switch and off TIM using capture/compare and update interrupt
 *     	TIM3 is set to a 38kHz frequency in toggle mode for outputting square waves
 *
 *      The program utilizes a timer TIM2 with two channels set in PWM (Pulse Width Modulation) mode.
 *		Through PWM, the duty cycle of each channel will regulate the speed of each motor. Additionally,
 *		four GPIO pins (PA3, PA4, PB3 and PB4) are configured as output mode, with two pins
 *		assigned to each motor to control the direction of the bot. The PWM signal generation is achieved
 *		by setting the port pin in alternate function mode for.
 *		These pins are PA0 and PA1 that control TIM2 channel 1 and channel 2.
 *		the frequency utilized is 3kHz, used to determine the ARR value.
 *		6 functions set to describe each specific behavior of the motor and implemented in a pattern in main.c.
 */

// Frequency of 3kHz
#include "stm32u5xx.h"
#include "main.h"
#include <sumobot.h>
#define PSC2 50 // with clock at 160 MHz
#define T2ARR 1066
#define PSC3 55 // with clock at 160 MHz
#define T3ARR 39 // ARR value coresponding to 38kHz
#define PSC4 200
#define T4ARR 4384// ARR value corresponding to 5.48 ms total (182.48Hz)

/*uint16_t sensor0_val = 0;
uint16_t sensor1_val = 0;
uint8_t currentChannel = 0; */// 0 for Left, 1 for Right

// basic initializations
void initBasic(void){
		// Basic setup - clocks
	// enable clock for PWR module
	RCC->AHB3ENR |= (1 << 2);
	PWR->SVMCR |= (1 << 29);  // set IO2SV bit to allow GPIOG use
	RCC->AHB2ENR1 |= 0x000000FF; // enable GPIOa - h clocks
	RCC->APB1ENR1 |= (1 << 0); // enable TIM2 clock
	RCC->APB1ENR1 |= (1 << 1); // enable TIM3 clock
	RCC->APB1ENR1 |= (1 << 2); // enable TIM4 clock
}// end initBasic


// function to initialize the GPIO port pins
void initGPIO(void){
	// setup PA0 and PA1 for timer
	GPIOA->MODER &= ~(0b11);  // clear bits for PA0
	GPIOA->MODER &= ~(0b11 << 2);  // clear bits for PA1
	GPIOA->MODER |= (0b10);   // alt. function for bit0
	GPIOA->MODER |= (0b10 << 2);   // alt. function for bit1
	GPIOA->AFR[0] |= 1; // TIM2 ch1
	GPIOA->AFR[0] |= (1 << 4); // TIM2 ch2

	//set PA3 and PA4 as the input direction control of the left motor
	GPIOA->MODER &= ~(0b11 << 6);  // clear bits for PA3
	GPIOA->MODER |= (0b01 << 6);   // make PA3 output
	GPIOA->MODER &= ~(0b11 << 8);  // clear bits for PA4
	GPIOA->MODER |= (0b01 << 8);   // make PA4 output

	//set PB3 and PB4 as the input direction control of the right motor
	GPIOB->MODER &= ~(0b11 << 6);  // clear bits for PB3
	GPIOB->MODER |= (0b01 << 6);   // make PB3 output
	GPIOB->MODER &= ~(0b11 << 8);  // clear bits for PB4
	GPIOB->MODER |= (0b01 << 8);   // make PB4 output

	//set PC3 as output for the LED to observe IR sensor behavior
	GPIOC->MODER &= ~(0b11 << 6);  // clear bits for PC3
	GPIOC->MODER |= (0b01 << 6);   // make PC3 output

	// setup pins for timer TIM3 for object sensor
	GPIOA->MODER &= ~(0b11 << 12);  // clear bits for PA6
	GPIOA->MODER |= (0b10 << 12);  // alt. function for bit6
	GPIOA->AFR[0] |= (2 << 24); // TIM3 ch1

	GPIOA->MODER &= ~(0b11 << 14);  // clear bits for PA7
	GPIOA->MODER |= (0b10 << 14);  // alt. function for bit7
	GPIOA->AFR[0] |= (2 << 28); // TIM3 ch2

	GPIOA->MODER &= ~(0b11 << 4); // clear bits for PA2 and set to input mode
	GPIOA->MODER &= ~(0b11 << 16); // clear bits for PA8 and set to input mode

	return;
}// end initGPIO

//function to set the speed of the motors
void speed_control(float duty1, float duty2){
	TIM2->CCR1 = (T2ARR + 1) * duty1;
	TIM2->CCR2 = (T2ARR + 1) * duty2;
}// end speed_control

//function to initialize and setup the timer (TIM2) as a PWM
void initTim2(void){
	TIM2->PSC = PSC2;
	TIM2->ARR = T2ARR;

	TIM2->CR1 |= (1 << 7);  //set ARPE bit - enable auto-reload preload
	TIM2->CCMR1 |= ((0b0110 << 4) | (1 << 3));  // PWM1 mode, enable preload reg. - CC1
	TIM2->CCMR1 |= ((0b0110 << 12) | (1 << 11));  // PWM1 mode, enable preload reg. - CC2
	TIM2->CCER |= (1 << 0);  // enable output on pin for CC1
	TIM2->CCER |= (1 << 4);  // enable output on pin for CC2
	TIM2->EGR |= (1 << 0); //update timer registers
	TIM2->CR1 |= (1 << 0);  // enable timer2
}//end initTim2

// initialize the timer TIM4 as interrupt to switch on and off TIM3
void initTim4(void){
	TIM4->PSC = PSC4;
	TIM4->ARR = T4ARR;
	TIM4->CR1 |= (1 << 7);  //set ARPE bit - enable auto-reload preload
	TIM4->EGR |= (1 << 0); //update timer registers
	TIM4->CR1 |= (1 << 2);  // interrupt only on over/underflow, also UDIS=0 (default)
	TIM4->DIER |= (1 << 0) | (1 << 1);   // Enable update and CC1 interrupts
	//TIM4->DIER |= (1 << 2);   // Enable CC2 interrupt
    TIM4->SR &= ~((1 << 0) | (1 << 1));    // Clear pending bits - make sure no stray interrupts at start
	TIM4->CR1 |= (1 << 0);  // enable timer4
	NVIC_EnableIRQ(TIM4_IRQn);
}

// initialize the timer TIM3 to output square wave and read the object
void initTim3(void){
	TIM3->PSC = PSC3;
	TIM3->ARR = T3ARR;
	TIM3->CCR1 = T3ARR;
	//TIM3->CCR2 = T3ARR;
	TIM3->CCMR1 |= ((3 << 4) | (1 << 3));  // toggle mode, enable preload reg. - CC1
	//TIM3->CCMR1 |= ((3 << 12) | (1 << 11));  // toggle mode, enable preload reg. - CC2
	TIM3->CCER |= (1 << 0);  // enable output on pin for CC1
	//TIM3->CCER |= (1 << 4);  // enable output on pin for CC2
	TIM3->CR1 |= (1 << 7);  //set ARPE bit - enable auto-reload preload
	TIM3->EGR |= (1 << 0); //update timer registers
	TIM3->CR1 |= (1 << 0);  // enable timer2
}

// function to set the robot to move backward with a specific speed
void forward (float speed1, float speed2){
	GPIOA->ODR |= (1 << 4); // turn on PA4
	GPIOA->ODR &= ~(1 << 3); // turn off PA3
	GPIOB->ODR |= (1 << 4); // turn on PB4
	GPIOB->ODR &= ~(1 << 3); // turn off PB3
	speed_control(speed1, speed2); // setup speed of two motors
}// end forward

//function to set the robot to move backward with a specific speed
void backward (float speed1, float speed2){
	GPIOA->ODR |= (1 << 3); // turn on PA3
	GPIOA->ODR &= ~(1 << 4); // turn off PA4
	GPIOB->ODR |= (1 << 3); // turn on PB3
	GPIOB->ODR &= ~(1 << 4); // turn off PB4
	speed_control(speed1, speed2); // setup speed of two motors
}// end backward

//function to set the robot to turn left with a specific speed
void turn_left (float speed1, float speed2){
	GPIOA->ODR |= (1 << 4); // turn on PA4
	GPIOA->ODR &= ~(1 << 3); // turn off PA3
	GPIOB->ODR |= (1 << 4); // turn on PB4
	GPIOB->ODR &= ~(1 << 3); // turn off PB3
	speed_control(speed1, speed2); // setup speed of two motors
}// end turn_left

//function to set the robot to turn right with a specific speed
void turn_right (float speed1, float speed2){
	GPIOA->ODR |= (1 << 4); // turn on PA4
	GPIOA->ODR &= ~(1 << 3); // turn off PA3
	GPIOB->ODR |= (1 << 4); // turn on PB4
	GPIOB->ODR &= ~(1 << 3); // turn off PB3
	speed_control(speed1, speed2); // setup speed of two motors
}// end turn_right

//function to set the robot to spin left with a specific speed
void spin_left (float speed1, float speed2){
	GPIOA->ODR |= (1 << 4); // turn on PA4
	GPIOA->ODR &= ~(1 << 3); // turn off PA3
	GPIOB->ODR |= (1 << 3); // turn on PB4
	GPIOB->ODR &= ~(1 << 4); // turn off PB3
	speed_control(speed1, speed2); // setup speed of two motors
}// end spin_left

//function to set the robot to spin right with a specific speed
void spin_right (float speed1, float speed2){
	GPIOA->ODR |= (1 << 3); // turn on PA4
	GPIOA->ODR &= ~(1 << 4); // turn off PA3
	GPIOB->ODR |= (1 << 4); // turn on PB4
	GPIOB->ODR &= ~(1 << 3); // turn off PB3
	speed_control(speed1, speed2); // setup speed of two motors
}// end spin_right

//function to stop the robot
void stop (void){
	forward(0,0);
}// end stop

//function to initialize A/D converter on channel 5 and 6
void initAdc(void){
	RCC->AHB3ENR |= (1 << 5); // enable ADC4 clock
	// set ASV bit to remove Vdda power  isolation *
	PWR->SVMCR |= (1 << 30);
	// setup MSIK as clock source for ADC *
	RCC->CCIPR3 |= (0b101 << 12); // 101

	// turn on ADC voltage regulator
	ADC4->ISR |= (1 << 12); // clear the bit *
	ADC4->CR |= (1 << 28); //enable the regulator *
	while(!(ADC4->ISR &(1 << 12))); // empty while waiting for LDORDY bit *

	//calibration
	ADC4->CR |= (1 << 31); // initiate calibration *
	while(ADC4->CR & (1 << 31)); // empty while waiting for completion of calibration *

	ADC4->SMPR1 |= (0b101 << 0); // set the sampling time for 39.5 clock cycle
	ADC4->CHSELR |= (1 << 6); // enable channel 6 assuming default mode of CFGR1:CHSELRMOD
	ADC4->CHSELR |= (1 << 5); // enable channel 5 of ADC

	ADC4->CFGR1 |= (0b01 << 2); // 10-bit resolution
	ADC4->CFGR1 |= (1 << 12); // store new value on overrun *
	ADC4->CFGR1 |= (1 << 13); // select continuous mode (repeat)
	ADC4->IER |= (1 << 2); // enable EOC interrupt *
	ADC4->CR |= (1 << 0); // enable ADC4


}// end initAdc

// function to initialize A/D converter interrupt
void initInterrupt(void) {
    NVIC_EnableIRQ(ADC4_IRQn); // Enable ADC interrupt in NVIC
}// end initInterrupt


