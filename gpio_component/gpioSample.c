//-------------------------------------------------------------------------------------------------
/**
 * @file gpioSample.c
 *
 * Sample app making use of the helper lib (gpio_iot) to drive CF3-GPIO on IoT0 card for mangOH Green/Red
 *	Scenario :
 *		GPIO_2 and GPIO_4 drive 2 LEDs to blink alternatively
 *		GPIO_1 is configured as an input (switch) to toggle another LED driven by GPIO_3
 *	This app can drive IoT0card's GPIOs on mangOH Green or Red (config tree) without changing the code nor IoT0 wiring.
 *
 *  NC - March 2018
 */
//-------------------------------------------------------------------------------------------------

#include "legato.h"
#include "interfaces.h"

#include "gpio_iot.h"   //use the gpio_iot helper lib



//Callback to handle level transition on GPIO_1
static void OnGpio1Change(bool state, void *ctx)
{
    LE_INFO("GPIO_1 State change %s", state?"TRUE":"FALSE");

    //GPIO_1 button/switch has been pressed, just toggle the LED driven by GPIO_3
    bool status = gpio_iot_Read(3);
	status = !status;
	gpio_iot_SetOutput(3, status);
}


//This implements the led blink scenario
void Blink()
{
	//Blink scenario (driven by timer) : alternating the GPIO_2 and GPIO_4 (they blink oppositely)

	//Retrieve the output level of GPIO_2
    bool state = gpio_iot_Read(2);

	gpio_iot_Read(4);   //just trace the output level of GPIO_4 to logread

	//Set GPIO_4 output to be the same level as GPIO_2
	gpio_iot_SetOutput(4, state);

	//Reverse the output of GPIO_2
    state = !state;
	gpio_iot_SetOutput(2, state);

    //just trace the output level of GPIO_1 & GPIO_3 to logread
	gpio_iot_Read(1);
	gpio_iot_Read(3);

	LE_INFO(" ");
}


COMPONENT_INIT
{
	//Initialize gpio_iot helper lib : Will be using IoT0 slot of the mangOH board
    //this will set the target mangOH board type: setting in config tree : "/gpio_iot/mangohType"
	gpio_iot_Init();

	//GPIO_1 as an input : connect a switch/PushButton to GPIO_1 (Pin24 of IoT card) and GND
	gpio_iot_SetInput(1, true);
	gpio_iot_EnablePullUp(1);   //enable the internal pull-up resistor
    
	//if the button is pushed, then call OnGpio1Change callback function
	gpio_iot_AddChangeEventHandler(1, GPIO_IOT_EDGE_RISING, OnGpio1Change, NULL, 100);

	//GPIO_2 is an output : Use a transistor to drive the LED.
	gpio_iot_SetPushPullOutput(2, true, true);

	//GPIO_3 is an output : Use a transistor to drive a LED/Motor.
	gpio_iot_SetPushPullOutput(3, true, true);

	//GPIO_4 is an output : Use a transistor to drive the LED.
	gpio_iot_SetPushPullOutput(4, true, true);

  
	//Set timer to animate LEDs
	le_timer_Ref_t ledTimerRef = le_timer_Create("ledTimer");            //create timer

	le_clk_Time_t      interval = { 2, 0 };                //timer to expire every 2 seconds
	le_timer_SetInterval(ledTimerRef, interval);
	le_timer_SetRepeat(ledTimerRef, 0);                   //set repeat to always

	//set callback function to handle timer expiration event
	le_timer_SetHandler(ledTimerRef, Blink);

	//start timer
	le_timer_Start(ledTimerRef);
}
