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

#include "legato.h"
#include "interfaces.h"

#include "gpio_iot.h"

//specify the type of mangOH board in config tree : 0=mangOH-Red, 1=mangOH-Green
#define CONFIG_TREE_MANGOH_BOARD_INT				"/gpio_iot/mangohType"

//mangOH IOT card only handle up to 4 CF3-GPIO
#define MAX_GPIO_COUNT      4

//3 known type for the time being
#define MANGOH_TYPE_COUNT   GPIO_IOT_MANGOH_YELLOW+1

//macro to map to the proper le_gpio API depending of the CF3-GPIO-Pin#, 3 mappings : Red, Green, Yellow
#define LE_GPIO_FUNC(RedCf3Pin, GreenCf3Pin, YellowCf3Pin, Name)  \
        {RedCf3Pin, le_gpioPin ## RedCf3Pin ## _ ## Name}, {GreenCf3Pin, le_gpioPin ## GreenCf3Pin ## _ ## Name}, {YellowCf3Pin, le_gpioPin ## YellowCf3Pin ## _ ## Name}

//redefining generic polarity
typedef enum
{
    GPIO_IOT_ACTIVE_HIGH = 0,
    GPIO_IOT_ACTIVE_LOW = 1
} gpio_iot_Polarity_t;

//for the mapping
typedef struct 
{
    int     cf3GpioPinNumber;
    void*   leGpioPtr;
} gpio_le_function_t;

typedef struct {
    const char*             functionNamePtr;
    gpio_le_function_t      gpioFunctionPtr[MAX_GPIO_COUNT][MANGOH_TYPE_COUNT];
} gpio_Function_t;

//callbacks definition
typedef bool (* pfnNoArgRetBool)();
typedef gpio_iot_Polarity_t (* pfnNoArgRetPolarity)();
typedef gpio_iot_PullUpDown_t (* pfnNoArgRetPullUpDown)();
typedef le_result_t (* pfnIntBoolRetleresult)(int, bool);
typedef le_result_t (* pfnNoArgRetleresult)();
typedef le_result_t (* pfnIntRetleresult)(int);
typedef gpio_iot_ChangeEventHandlerRef_t (* pfnIntCbCtxtIntRetChangeEventhRef)(int, gpio_iot_ChangeCallbackFunc_t, void *, int32_t);
typedef gpio_iot_Edge_t (* pfnNoArgRetEdge)(); 

//board names
const char*   _gpio_mangoh_board[] = {"mangOH Red", "mangOH Green", "mangOH Yellow"};

//Actual mapping to le_gpio api
gpio_Function_t       _gpio_functions[] = {
    {
        .functionNamePtr = "Read",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, Read)},   //mapping le_gpio_Read function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, Read)},   //mapping le_gpio_Read function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, Read)},     //mapping le_gpio_Read function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, Read)}       //mapping le_gpio_Read function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "IsInput",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, IsInput)},   //mapping le_gpio_IsInput function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, IsInput)},   //mapping le_gpio_IsInput function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, IsInput)},     //mapping le_gpio_IsInput function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, IsInput)}       //mapping le_gpio_IsInput function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "GetPolarity",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, GetPolarity)},   //mapping le_gpio_GetPolarity function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, GetPolarity)},   //mapping le_gpio_GetPolarity function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, GetPolarity)},     //mapping le_gpio_GetPolarity function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, GetPolarity)}       //mapping le_gpio_GetPolarity function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "GetPullUpDown",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, GetPullUpDown)},   //mapping le_gpio_GetPullUpDown function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, GetPullUpDown)},   //mapping le_gpio_GetPullUpDown function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, GetPullUpDown)},     //mapping le_gpio_GetPullUpDown function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, GetPullUpDown)}       //mapping le_gpio_GetPullUpDown function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "SetPushPullOutput",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, SetPushPullOutput)},   //mapping le_gpio_SetPushPullOutput function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, SetPushPullOutput)},   //mapping le_gpio_SetPushPullOutput function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, SetPushPullOutput)},     //mapping le_gpio_SetPushPullOutput function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, SetPushPullOutput)}       //mapping le_gpio_SetPushPullOutput function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "Activate",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, Activate)},   //mapping le_gpio_Activate function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, Activate)},   //mapping le_gpio_Activate function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, Activate)},     //mapping le_gpio_Activate function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, Activate)}       //mapping le_gpio_Activate function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "Deactivate",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, Deactivate)},   //mapping le_gpio_Deactivate function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, Deactivate)},   //mapping le_gpio_Deactivate function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, Deactivate)},     //mapping le_gpio_Deactivate function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, Deactivate)}       //mapping le_gpio_Deactivate function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "SetInput",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, SetInput)},   //mapping le_gpio_SetInput function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, SetInput)},   //mapping le_gpio_SetInput function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, SetInput)},     //mapping le_gpio_SetInput function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, SetInput)}       //mapping le_gpio_SetInput function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "AddChangeEventHandler",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, AddChangeEventHandler)},   //mapping le_gpio_AddChangeEventHandler function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, AddChangeEventHandler)},   //mapping le_gpio_AddChangeEventHandler function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, AddChangeEventHandler)},     //mapping le_gpio_AddChangeEventHandler function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, AddChangeEventHandler)}       //mapping le_gpio_AddChangeEventHandler function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "EnablePullUp",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, EnablePullUp)},   //mapping le_gpio_EnablePullUp function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, EnablePullUp)},   //mapping le_gpio_EnablePullUp function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, EnablePullUp)},     //mapping le_gpio_EnablePullUp function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, EnablePullUp)}       //mapping le_gpio_EnablePullUp function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "EnablePullDown",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, EnablePullDown)},   //mapping le_gpio_EnablePullDown function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, EnablePullDown)},   //mapping le_gpio_EnablePullDown function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, EnablePullDown)},     //mapping le_gpio_EnablePullDown function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, EnablePullDown)}       //mapping le_gpio_EnablePullDown function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    },
    {
        .functionNamePtr = "GetEdgeSense",
        .gpioFunctionPtr =  {
                                {LE_GPIO_FUNC(42, 42, 42, GetEdgeSense)},   //mapping le_gpio_GetEdgeSense function for GPIO_1 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(13, 33, 13, GetEdgeSense)},   //mapping le_gpio_GetEdgeSense function for GPIO_2 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(7, 13, 7, GetEdgeSense)},     //mapping le_gpio_GetEdgeSense function for GPIO_3 (mRed, mGreen , mYellow)
                                {LE_GPIO_FUNC(8, 8, 8, GetEdgeSense)}       //mapping le_gpio_GetEdgeSense function for GPIO_4 (mRed, mGreen , mYellow)
                            }
    }
};


//Specifies the type of mangOH board being used. Due to different GPIO wiring
gpio_iot_mangohType_t               _gpio_iot_mangohType;


//return the type of board
gpio_iot_mangohType_t gpio_iot_GetMangohType()
{
    return _gpio_iot_mangohType;
}

//Set the type of board
void gpio_iot_SetMangohType(gpio_iot_mangohType_t mangohType)
{
	_gpio_iot_mangohType = mangohType;
    
    //persist the setting in Config Tree
    le_cfg_QuickSetInt(CONFIG_TREE_MANGOH_BOARD_INT, _gpio_iot_mangohType);
}

//Return the result of the mapping to a le_gpioPin function
gpio_le_function_t* GetFunctionPtr
(
    const char *    functionNamePtr,
    uint32_t        gpioNumber
)
{
    if (gpioNumber <= 0 || gpioNumber > MAX_GPIO_COUNT )
    {
        LE_INFO("!!!! GetFunctionPtr - Invalid GPIO Number !!!!");
        return NULL;
    }

    int functionIdx;

    //LE_INFO("GetFunctionPtr - looking for GPIO_%d(%s)...", gpioNumber, functionNamePtr);

    for (functionIdx = 0; functionIdx < NUM_ARRAY_MEMBERS(_gpio_functions); functionIdx++)
    {
        gpio_Function_t * functionPtr = &(_gpio_functions[functionIdx]);

        //LE_INFO("Parsing %s...", functionPtr->functionNamePtr);

        if (strcmp(functionPtr->functionNamePtr, functionNamePtr) == 0)
        {
            gpio_le_function_t* pLeFunc = &functionPtr->gpioFunctionPtr[gpioNumber-1][_gpio_iot_mangohType];

            //LE_INFO("Found %s... %p", functionPtr->functionNamePtr, pLeFunc);

            return pLeFunc;
        }
    }

    fprintf(stderr, "%s(%d) function does not exist", functionNamePtr, gpioNumber);

    return NULL;
}

//Call the proper le_gpioPinxx_Read function based on the provided IoT0-GPIO pin# (1 - 4)
bool gpio_iot_Read(uint32_t  gpioNumber)
{
    const char* name = "Read";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr("Read", gpioNumber);

    bool state = false;
    
    if (gpioFunctionPtr)
    {
        pfnNoArgRetBool    pFn = (pfnNoArgRetBool) gpioFunctionPtr->leGpioPtr;
        state = pFn();

        LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %d", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, state);
    }

    return state;
}

//Call the proper le_gpioPinxx_IsInput function based on the provided IoT0-GPIO pin# (1 - 4)
bool gpio_iot_IsInput(uint32_t  gpioNumber)
{
    const char* name = "IsInput";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    bool state = false;
    
    if (gpioFunctionPtr)
    {
        pfnNoArgRetBool    pFn = (pfnNoArgRetBool) gpioFunctionPtr->leGpioPtr;
        state = pFn();

        LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, state ? "Yes" : "No");
    }

    return state;
}

//Call the proper le_gpioPinxx_GetPolarity function based on the provided IoT0-GPIO pin# (1 - 4)
bool gpio_iot_GetPolarity(uint32_t  gpioNumber)
{
    const char* name = "GetPolarity";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    bool            bPolarity = false;
    
    if (gpioFunctionPtr)
    {
        pfnNoArgRetPolarity    pFn = (pfnNoArgRetPolarity) gpioFunctionPtr->leGpioPtr;
        gpio_iot_Polarity_t     polarity = pFn();

        if (polarity == GPIO_IOT_ACTIVE_HIGH)
        {
            bPolarity = true;
        }

        LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, bPolarity ? "ACTIVE_HIGH" : "ACTIVE_LOW");
    }

    return bPolarity;
}

//Call the proper le_gpioPinxx_GetPullUpDown function based on the provided IoT0-GPIO pin# (1 - 4)
gpio_iot_PullUpDown_t gpio_iot_GetPullUpDown(uint32_t gpioNumber)
{
    const char* name = "GetPullUpDown";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnNoArgRetPullUpDown    pFn = (pfnNoArgRetPullUpDown) gpioFunctionPtr->leGpioPtr;
        gpio_iot_PullUpDown_t pud = pFn();

        if (pud == GPIO_IOT_PULL_DOWN)
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "pull down");
        }
        else if (pud == GPIO_IOT_PULL_UP)
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "pull up");
        }
        else
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "pull none");
        }
        return pud;

        
    }

    return -1;
}


//To Set a GPIO "As Output"
//Call the proper le_gpioPinxx_SetPushPullOutput function based on the provided IoT0-GPIO pin# (1 - 4)
void gpio_iot_SetPushPullOutput(uint32_t gpioNumber, bool bActiveHigh, bool bInitValue)
{
    gpio_iot_Polarity_t polarity = bActiveHigh ? GPIO_IOT_ACTIVE_HIGH : GPIO_IOT_ACTIVE_LOW;

    const char* name = "SetPushPullOutput";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnIntBoolRetleresult    pFn = (pfnIntBoolRetleresult) gpioFunctionPtr->leGpioPtr;
        pFn(polarity, bInitValue);

        gpio_iot_Read(gpioNumber);

        gpio_iot_IsInput(gpioNumber);

    }
    
}


//Activate/Deactivate an output
//Call the proper le_gpioPinxx_Activate / le_gpioPinxx_Deactivate function based on the provided IoT0-GPIO pin# (1 - 4)
void gpio_iot_SetOutput(uint32_t gpioNumber, bool bActivate)
{
    char name[32] = {0};

    if (bActivate)
    {
        strcpy(name, "Activate");        
    }
    else
    {
        strcpy(name, "Deactivate");
    }

    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnNoArgRetleresult    pFn = (pfnNoArgRetleresult) gpioFunctionPtr->leGpioPtr;
        pFn();
    }
}



//Set a GPIO as "an Input"
//Call the proper le_gpioPinxx_SetInput function based on the provided IoT0-GPIO pin# (1 - 4)
void gpio_iot_SetInput(uint32_t gpioNumber, bool bPolarityHigh)
{
    const char* name = "SetInput";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnIntRetleresult    pFn = (pfnIntRetleresult) gpioFunctionPtr->leGpioPtr;

        gpio_iot_Polarity_t polarity = bPolarityHigh ? GPIO_IOT_ACTIVE_HIGH : GPIO_IOT_ACTIVE_LOW;

        pFn(polarity);

        gpio_iot_Read(gpioNumber);

        gpio_iot_IsInput(gpioNumber);

        gpio_iot_GetPolarity(gpioNumber);

        gpio_iot_GetPullUpDown(gpioNumber);
    }
}

//Call the proper le_gpioPinxx_AddChangeEventHandler function based on the provided IoT0-GPIO pin# (1 - 4)
gpio_iot_ChangeEventHandlerRef_t  gpio_iot_AddChangeEventHandler
(
    uint32_t    gpioNumber,
    gpio_iot_Edge_t trigger,
    gpio_iot_ChangeCallbackFunc_t handlerPtr,
    void *contextPtr,
    int32_t sampleMs
)
{
    const char* name = "AddChangeEventHandler";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnIntCbCtxtIntRetChangeEventhRef    pFn = (pfnIntCbCtxtIntRetChangeEventhRef) gpioFunctionPtr->leGpioPtr;

        return pFn(trigger, handlerPtr, contextPtr, sampleMs);
    }

    return NULL;
}

//Call the proper le_gpioPinxx_EnablePullUp function based on the provided IoT0-GPIO pin# (1 - 4)
le_result_t     gpio_iot_EnablePullUp(uint32_t gpioNumber)
{
    const char* name = "EnablePullUp";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnNoArgRetleresult    pFn = (pfnNoArgRetleresult) gpioFunctionPtr->leGpioPtr;

        return pFn();

    }

    return LE_FAULT;
}

//Call the proper le_gpioPinxx_EnablePullDown function based on the provided IoT0-GPIO pin# (1 - 4)
le_result_t     gpio_iot_EnablePullDown(uint32_t gpioNumber)
{
    const char* name = "EnablePullDown";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnNoArgRetleresult    pFn = (pfnNoArgRetleresult) gpioFunctionPtr->leGpioPtr;

        return pFn();

    }

    return LE_FAULT;
}
    
//Call the proper le_gpioPinxx_GetEdgeSense function based on the provided IoT0-GPIO pin# (1 - 4)
gpio_iot_Edge_t  gpio_iot_gpio1_GetEdgeSense(uint32_t gpioNumber)
{
    const char* name = "GetEdgeSense";
    gpio_le_function_t* gpioFunctionPtr = GetFunctionPtr(name, gpioNumber);

    if (gpioFunctionPtr)
    {
        pfnNoArgRetEdge    pFn = (pfnNoArgRetEdge) gpioFunctionPtr->leGpioPtr;

        gpio_iot_Edge_t    edgeSense = pFn();

        if (GPIO_IOT_EDGE_FALLING == edgeSense)
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "Falling edge");
        }
        else if (GPIO_IOT_EDGE_RISING == edgeSense)
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "Rising edge");
        }
        else if (GPIO_IOT_EDGE_BOTH == edgeSense)
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "Both edges");
        }
        else if (GPIO_IOT_EDGE_NONE == edgeSense)
        {
            LE_INFO("%s - GPIO_%d - CF3-Pin%d - %s : %s", _gpio_mangoh_board[_gpio_iot_mangohType], gpioNumber, gpioFunctionPtr->cf3GpioPinNumber, name, "NO edge");
        }    

        return edgeSense;
    }

    return GPIO_IOT_EDGE_NONE;
}

//Call this function first to initialize the type of mangOH board to be used
void gpio_iot_Init()
{
    //Specify the type of mangOH board.
    //The same application and the same IOT board can be reused on mangOH Red/Green without changing the code nor wiring.
    int cfgValue = le_cfg_QuickGetInt(CONFIG_TREE_MANGOH_BOARD_INT, -1);
    if (cfgValue < 0)
    {
        LE_INFO("No setting in Config Tree, default to mangOH Green");
        gpio_iot_SetMangohType(GPIO_IOT_MANGOH_GREEN);
    }
    else
    {
        LE_INFO("mangOH board type in Config Tree is %d", cfgValue);
        //set the board type in the helper lib
        gpio_iot_SetMangohType(cfgValue);
    }
}

