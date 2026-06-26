/*
 * main.c
 *
 *  Created on: Mar 23, 2024
 *      Author: nikiemao
 *
 */

// Frequency of 3kHz
#include "stm32u5xx.h"
#include "main.h"
#include <sumobot.h>

// global variable
uint8_t val1 = 0;
uint8_t val2 = 0;
uint16_t sensor0_val = 0;
uint16_t sensor1_val = 0;
uint8_t currentChannel = 0; // 0 for Left, 1 for Right

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
	initAdc();
	initInterrupt();

    typedef enum {FWD,BWD, LEFTURN, RIGHTURN}state;
   		 state current_state = FWD;

    int sensor0;
    int sensor1;

    int intensity0;
    int intensity1;

    uint8_t iter = 0;

	LCD_print_char(3, 12,'%');
	LCD_print_char(5, 13,'%');

	stop();
	msoe_delay_ms(5000);
    ADC4->CR |= (1 << 2); // start the conversion
    while(1){
        if (currentChannel == 0) {
            // Process left sensor eading
        	// calculated value of the voltage that produces the intensity
            float vin = ((float)sensor0_val / 1023) * 3.3;
            // calculated light intensity in percentage of the left photocell
            intensity0 = (vin / 3.3) * 100;
            LCD_print_udec3(3, 7, intensity0);
            //msoe_delay_ms(10);
        }
        if (currentChannel == 1){
            // Process right sensor reading
        	// calculated value of the voltage that produces the intensity
            float vin = ((float)sensor1_val / 1023) * 3.3;
            intensity1 = (vin / 3.3) * 100;
            LCD_print_udec3(5, 8, intensity1);
            //msoe_delay_ms(10);

        }

        if(intensity0 < 65){
        	sensor0 = 0;
        	LCD_print_udec3(3, 15, sensor0);
        }
        else {
        	sensor0 = 1;
        	LCD_print_udec3(3, 15, sensor0);
        }

        if(intensity1 < 78){
        	sensor1 = 0;
        	LCD_print_udec3(5, 15, sensor1);
        }
        else {
        	sensor1 = 1;
        	LCD_print_udec3(5, 15, sensor1);
        }


		if(sensor0 == 1 && sensor1 == 1){
			if(!val1 && val2){
				LCD_print_str(0, 0, "IR ON ");
				forward(0.6,0.4);
			}
			else if(val1 && !val2){
				LCD_print_str(0, 0, "IR ON ");
				forward(0.4,0.6);
			}
			else if(!val1 && !val2){
				LCD_print_str(0, 0, "IR ON ");
				forward(0.5,0.5);
			}
			else {
				LCD_print_str(0, 0, "IR OFF");
				spin_right(0.5, 0.5);
			}
		}
		else {
			stop();
            msoe_delay_ms(50);
            backward(0.4, 0.4);
            msoe_delay_ms(700);
		}
    	/*if(current_state == FWD){
    		if(sensor0 == 0 && sensor1 == 1){
    			stop();
                msoe_delay_ms(200);
                backward(0.6, 0.6);
                msoe_delay_ms(1600);
        		current_state = RIGHTURN;
    		}

    		else if(sensor0 == 1 && sensor1 == 0){
    			stop();
                msoe_delay_ms(200);
                backward(0.6, 0.6);
                msoe_delay_ms(1600);
        		current_state = LEFTURN;
    		}

    		else if(sensor0 == 0 && sensor1 == 0){
    			stop();
                msoe_delay_ms(200);
                backward(0.6, 0.6);
                msoe_delay_ms(1600);
        		current_state = RIGHTURN;
    		}
    		else{
        		current_state = FWD;
    		}
    	}

    	if(current_state == BWD){
    		if((sensor0 == 0 && sensor1 == 1) || (sensor0 == 1 && sensor1 == 0) || (sensor0 == 0 && sensor1 == 0) ){
                forward(0.6, 0.6);
                msoe_delay_ms(1600);
    			current_state = RIGHTURN;
    		}
    		else{
                backward(0.6, 0.6);
                msoe_delay_ms(1600);
        		current_state = FWD;
    		}
    	}

    	if(current_state == RIGHTURN){
    		if(sensor0 == 0 && sensor1 == 1 ){
        		current_state = LEFTURN;
    		}
    		else if((sensor0 == 1 && sensor1 == 0) ||(sensor0 == 0 && sensor1 == 0)){
        		current_state = RIGHTURN;
    		}
    		else{
        		turn_right(0, 0.6);
        		//msoe_delay_ms(2000);
        		current_state = FWD;
    		}
    	}

    	if(current_state == LEFTURN){
    		if(sensor0 == 0 && sensor1 == 1 ){
        		current_state = RIGHTURN;
    		}
    		else if((sensor0 == 1 && sensor1 == 0) ||(sensor0 == 0 && sensor1 == 0)){
        		current_state = LEFTURN;
    		}
    		else{
    	    	turn_left(0.6, 0);
    	    	//msoe_delay_ms(2000);
        		current_state = FWD;
    		}
    	}

    	if(current_state == FWD)
    		forward(0.7, 0.7);
    	else if(current_state == BWD)
    		backward(0.7, 0.7);
    	else if(current_state == RIGHTURN)
    		turn_right(0.2, 0.6);
    	else
    		turn_left(0.6, 0.2);*/

		iter++;  // keep loop non-empty
    }

}// end main
