//-------------------------------------------------------------------------------------------------
/**
 * @file gpio_iot.c
 *
 * Helper library to facilitate the use of CF3-GPIO on IoT0 card designed for mangOH Green/Red..
 *  This helper lib maps the proper module-CF3-GPIO-pins to IoTcard-GPIO-pins based on the selected mangOH board to be used.
 *  You don't need to figure out which le_gpioPinxxx function to be used to address a physical GPIO in on the IoT0 card.
 *  Your app can run on mangOH Green or Red without changing the code nor IoTcard wiring.
 *
 *  NC - March 2018
 */
//-------------------------------------------------------------------------------------------------

#ifndef _GPIO_IOT_H_
#define _GPIO_IOT_H_

typedef enum gpio_iot_mangohType
{
	GPIO_IOT_MANGOH_RED,
	GPIO_IOT_MANGOH_GREEN,
	GPIO_IOT_MANGOH_YELLOW
} gpio_iot_mangohType_t;

typedef enum name
{
	GPIO_IOT_EDGE_NONE,
	GPIO_IOT_EDGE_RISING,
	GPIO_IOT_EDGE_FALLING,
	GPIO_IOT_EDGE_BOTH
} gpio_iot_Edge_t;

typedef enum
{
    GPIO_IOT_PULL_OFF = 0,
    GPIO_IOT_PULL_DOWN = 1,
    GPIO_IOT_PULL_UP = 2
} gpio_iot_PullUpDown_t;

typedef void(* 	gpio_iot_ChangeCallbackFunc_t) (bool state, void *contextPtr);

typedef struct gpio_iot_ChangeEventHandler* gpio_iot_ChangeEventHandlerRef_t;


////////////////////////////////////////////////////////////////
//Initializer : call this first before accessing other function
void 								gpio_iot_Init();

////////////////////////////////////////////////////////////////
//mangOH board Type
gpio_iot_mangohType_t 				gpio_iot_GetMangohType();
void 								gpio_iot_SetMangohType(gpio_iot_mangohType_t mangohType);


//Configure the specified GPIO (1-4) as Output
void                    			gpio_iot_SetPushPullOutput(uint32_t gpioNumber, bool bActiveHigh, bool bInitValue);
//Set the output level
void                    			gpio_iot_SetOutput(uint32_t gpioNumber, bool bActivate);


//Configure the specified GPIO (1-4) as Input
void                    			gpio_iot_SetInput(uint32_t gpioNumber, bool bActiveHigh);
le_result_t             			gpio_iot_EnablePullUp(uint32_t gpioNumber);
le_result_t             			gpio_iot_EnablePullDown(uint32_t gpioNumber);
//Set GPIO input change handler
gpio_iot_ChangeEventHandlerRef_t  	gpio_iot_AddChangeEventHandler
                                        (
                                        	uint32_t gpioNumber,
                                            gpio_iot_Edge_t trigger,
                                            gpio_iot_ChangeCallbackFunc_t handlerPtr,
                                            void *contextPtr,
                                            int32_t sampleMs
                                        );

//Read the output of the specified GPIO (1-4)
bool                    			gpio_iot_Read(uint32_t gpioNumber);				//true=activated, false=deactivated

//Properties of the specified GPIO (1-4)
bool                    			gpio_iot_IsInput(uint32_t gpioNumber);			//true=Input, false=OUTPUT
gpio_iot_Edge_t        				gpio_iot_GetEdgeSense(uint32_t gpioNumber);     //0=NONE, 1=RISING, 2=FALLING, 3=BOTH
bool                    			gpio_iot_GetPolarity(uint32_t gpioNumber);		//true= ACTIVE_HIGH, false=ACTIVE_LOW
gpio_iot_PullUpDown_t               gpio_iot_GetPullUpDown(uint32_t gpioNumber);	//0=GPIO_IOT_PULL_OFF, 1=GPIO_IOT_PULL_DOWN, 2=GPIO_IOT_PULL_UP


#endif 	//_GPIO_IOT_H_
