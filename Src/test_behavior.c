/*
 * test_behavior.c
 *
 *  Created on: Apr 19, 2024
 *      Author: nikiemao
 *
 *     	This program is testing the IR object detector sensor and the actions taken upon detection.
 *		The bot is spinning in place searching for an object, once an object is detected
 *		it moves forward to attack and push the object outside of the ring. Then moves backward
 *		when it detects a line and resumes spinning again searching for objects.
 *		The bot remains on the ring after pushing the object out as it detects the line at the edge of the ring
 *
 */


// Frequency of 3kHz
#include "stm32u5xx.h"
#include "main.h"
#include <sumobot.h>

// global variable
uint8_t val1 = 0;
uint16_t sensor0_val = 0;
uint16_t sensor1_val = 0;
uint8_t currentChannel = 0; // 0 for Left, 1 for Right

int main(void){
    msoe_clk_setup(160);  // 160 MHz uncomment if higher speed desired
    msoe_delay_init();
    LCD_IO_Init();

    RCC->AHB3ENR |= RCC_AHB3ENR_PWREN;  // enable clock for PWR module

   //initialization of port pins, ADC, Interrupts and timer
    initBasic();
    initGPIO();
    initTim2();
	initTim3();
	initTim4();
	initAdc();
	initInterrupt();

    int sensor0;
    int sensor1;

    int intensity0;
    int intensity1;

    LCD_print_str(0, 0, "Welcome");
	LCD_print_str(3, 12,"L sensor");
	LCD_print_str(5, 13,"R sensor");

	LCD_print_char(3, 12,'%');
	LCD_print_char(5, 13,'%');


	stop();
	msoe_delay_ms(5000);
    ADC4->CR |= (1 << 2); // start the conversion
    while(1){
    	 LCD_print_str(0, 0, "          ");
    	 LCD_print_str(1, 2, "Searching...");
        if (currentChannel == 0) {
            // Process left sensor reading
        	// calculated value of the voltage that produces the intensity
            float vin = ((float)sensor0_val / 1023) * 3.3;
            // calculated light intensity in percentage of the left sensor
            intensity0 = (vin / 3.3) * 100;
            LCD_print_udec3(3, 7, intensity0);
            //msoe_delay_ms(10);
        }
        if (currentChannel == 1){
            // Process right sensor reading
        	// calculated value of the voltage that produces the intensity
            float vin = ((float)sensor1_val / 1023) * 3.3;
            // calculated light intensity in percentage of the right sensor
            intensity1 = (vin / 3.3) * 100;
            LCD_print_udec3(5, 8, intensity1);
            //msoe_delay_ms(10);

        }

        //threshold setup for line detection
        if(intensity0 < 65){
        	sensor0 = 0;
        	LCD_print_udec3(3, 15, sensor0);
        }
        else {
        	sensor0 = 1;
        	LCD_print_udec3(3, 15, sensor0);
        }

        if(intensity1 < 88){
        	sensor1 = 0;
        	LCD_print_udec3(5, 15, sensor1);
        }
        else {
        	sensor1 = 1;
        	LCD_print_udec3(5, 15, sensor1);
        }

        //bot spinning and searching an object if the line sensors are not detecting a line
		if(sensor1 == 1){
			if(!val1){
				LCD_print_str(1, 2, "               ");
				LCD_print_str(1, 2, "GOT YOU!");
				forward(0.95,0.95);
			}
			else {
				LCD_print_str(1, 2, "Searching...");
				spin_right(0.95, 0.95);
			}
		}
		else if(sensor1 == 0 || sensor0 == 0){
			LCD_print_str(1, 2, "               ");
			LCD_print_str(1, 2, "Whoops...");
			stop();
            msoe_delay_ms(200);
            backward(0.6, 0.6);
            msoe_delay_ms(1000);
		}
    }

}// end main
