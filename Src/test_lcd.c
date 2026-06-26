/*
 * test_lcd.c
 *
 *  Created on: Apr 20, 2024
 *      Author: nikiemao
 *
 * 		This is a basic program for testing proper functionality of
 * 		the LCD display by trying to display some words on the display
 */

#include "stm32u5xx.h"
#include "main.h"
#include <sumobot.h>

int main(void){
    msoe_clk_setup(160);  // 160 MHz uncomment if higher speed desired
    msoe_delay_init();
    LCD_IO_Init();

    while(1){
    	LCD_print_str(0, 0, "LCD");
    	LCD_print_str(1, 2, "Test");
    	LCD_print_str(2, 4, "Perfect");
    }

}
