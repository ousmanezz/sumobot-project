/*
 * test_obj_detect.c
 *
 *  Created on: Apr 11, 2024
 *      Author: nikiemao
 *
 *		This program is testing the IR object detector sensor.
 *		The bot is spinning in place searching for an object, once an object is detected
 *		it stops spinning and display "IR ON" on the LCD. Otherwise, it displays "IR OFF".
 */

#include "stm32u5xx.h"
#include "main.h"
#include <sumobot.h>

// global variable
uint8_t val1 = 0;
uint8_t val2 = 0;

int main(void){
    msoe_clk_setup(160);  // 160 MHz uncomment if higher speed desired
    msoe_delay_init();
    LCD_IO_Init();

    RCC->AHB3ENR |= RCC_AHB3ENR_PWREN;  // enable clock for PWR module

   //initialization of port pins and timer
    initBasic();
    initGPIO();
    initTim2();
	initTim3();
	initTim4();

	stop();
	msoe_delay_ms(5000);
    while(1){
		if(val1){
			LCD_print_str(0, 0, "IR OFF");
			spin_right(0.7, 0.7);
		}
		else {
			LCD_print_str(0, 0, "IR ON ");
			stop();
			while(1){

			}
		}

    }

}// end main

/// ISR for Tim5 interrupt
void TIM4_IRQHandler(void){
    if (TIM4->SR & (1 << 0)){ // Update interrupt occurred
        TIM4->SR &= ~(1 << 0); // Clear update interrupt flag
        // Set CCR1 value for the next pulse
        TIM3->CCER |= 1; // Turn on CCR
        TIM4->CCR1 = 384;
    }

   if (TIM4->SR & (1 << 1)){ // Capture/compare CC1 interrupt occurred
        TIM4->SR &= ~(1 << 1); // Clear CC1 interrupt flag
        TIM3->CCER &= ~1; // Turn off CCR
        val1 = GPIOA->IDR & (1 << 2); // read the sensor 1
    }
}



