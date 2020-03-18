/*
 * HardwareDescriptor.h
 *
 * Created: 2019-09-19 4:57:49 PM
 *  Author: charl
 */ 


#ifndef HARDWAREDESCRIPTOR_H_
#define HARDWAREDESCRIPTOR_H_

#include "FreeRTOS.h"
#include "samgpio.h"
#include "dmx.h"
#include "ccl.h"
#include "samc20e18a.h"

#define GPIO_SWITCH_LOCK	GPIO(GPIO_PORTA, 2)
#define GPIO_SWITCH_RESET	GPIO(GPIO_PORTA, 3)
#define GPIO_SWITCH_GO		GPIO(GPIO_PORTA, 4)
#define GPIO_LED_ESTOP		GPIO(GPIO_PORTA, 14)
#define GPIO_LED_LINK		GPIO(GPIO_PORTA, 15)
#define GPIO_SIGNAL_ESTOP	GPIO(GPIO_PORTA, 0)

#endif /* HARDWAREDESCRIPTOR_H_ */