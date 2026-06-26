/*
 * test_motors.c
 *
 *  Created on: Apr 2, 2024
 *      Author: nikiemao
 *
 *		This program is used to test the motors behaviors.
 *		Uses the 6 functions from sumobot.c to implements all the possible behavior of the bot.
 *		The bot stops for 5 seconds, then goes forward, then backward, then turn right and left
 *		and finally spin right and spin left to test those behaviors.
 */



#include "stm32u5xx.h"
#include "main.h"

int main(void){
    msoe_clk_setup(160);  // 160 MHz uncomment if higher speed desired
    msoe_delay_init();

   //initialization of port pins and timer
    initBasic();
    initGPIO();
    initTim2();

    // movements execution test following a pattern
	forward(0,0); // this is the stop state as the PWMs duty cycles are 0
	msoe_delay_ms(5000);
	forward(0.30, 0.30);
	msoe_delay_ms(3000);
	forward(0.80, 0.80);
	msoe_delay_ms(3000);
	forward(0,0);
	msoe_delay_ms(500);
	backward(0.5, 0.5);
	msoe_delay_ms(6000);
	forward(0.40, 0.40);
	msoe_delay_ms(1000);
	turn_left(0.60, 0.2);
	msoe_delay_ms(2000);
	forward(0.40, 0.40);
	msoe_delay_ms(1000);
	turn_right(0.20, 0.6);
	msoe_delay_ms(2000);
	forward(0.50, 0.50);
	msoe_delay_ms(1000);
	spin_right(0.7, 0.7);
	msoe_delay_ms(5000);
	forward(0,0);
	msoe_delay_ms(500);
	spin_left(0.7, 0.7);
	msoe_delay_ms(5000);
	forward(0.02,0.02);
	msoe_delay_ms(5000);
	forward(0,0);// this is the stop state as the PWMs duty cycle is 0

	return 0;

}
