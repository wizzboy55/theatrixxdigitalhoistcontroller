/*
 * control.c
 *
 * Created: 2020-03-18 3:01:12 PM
 *  Author: charl
 */ 

#include "control.h"
#include "HardwareDescriptor.h"
#include "task.h"
#include "timers.h"

#include "string.h"

ControlRegisters_t controlRegisters;
ControlRegisters_t emptyRegisters;

TimerHandle_t controlTimer;
#define CONTROL_TIMEOUT 150
BaseType_t controlTimedOut = pdFALSE;
uint8_t uValidMessagesReceived = 0;
#define CONTROL_VALIDMESSAGESTHRESHOLD 5

void vControlStop(void) {
	controlTimedOut = pdTRUE;
	xGpioShiftRegistersPush(&controlShiftRegisterConfigGpio, emptyRegisters.reg, NULL, sizeof(ControlRegisters_t));
}

void vControlNewMessage(HoistControl_t *newMessage) {
	uint8_t led_go = 0;
	for(uint8_t i = 0; i < sizeof(HoistControl_t); i++) {
		controlRegisters.hoistControl.reg[i] = newMessage->reg[i];
		if(i % 2 == 0) {
			if(newMessage->reg[i] != 0x00 && newMessage->reg[i+1] != 0x00) {
				led_go |= 1<<(i % 2);
			}
		}
	}
	
	controlRegisters.led_go = led_go;
	controlRegisters.led_link = 0xFF;
	
	if(uValidMessagesReceived < CONTROL_VALIDMESSAGESTHRESHOLD) {
		uValidMessagesReceived++;
	} else {
		xTimerReset(controlTimer, portMAX_DELAY);
		controlTimedOut = pdFALSE;
	}
}

void vControlTask(void* p) {
	
	memset(&emptyRegisters, 0, sizeof(emptyRegisters));
	
	for(;;) {
		if(controlTimedOut) {
			xGpioShiftRegistersPush(&controlShiftRegisterConfigGpio, emptyRegisters.reg, NULL, sizeof(ControlRegisters_t));
		} else {
			xGpioShiftRegistersPush(&controlShiftRegisterConfigGpio, controlRegisters.reg, NULL, sizeof(ControlRegisters_t));
		}
		vTaskDelay(25 / portTICK_PERIOD_MS);
	}
}

void vControlTimerCallback(TimerHandle_t xTimer) {
	uValidMessagesReceived = 0;
	controlTimedOut = pdTRUE;
}

BaseType_t xControlInit(void) {
	BaseType_t res;
	
	res = xGpioShiftRegistersInit(&controlShiftRegisterConfigGpio);
	
	samgpio_setPinDirection(GPIO_DISABLE, GPIO_DIRECTION_OUT);
	samgpio_setPinFast(GPIO_DISABLE);
	
	controlTimer = xTimerCreate("ControlTimer", CONTROL_TIMEOUT / portTICK_PERIOD_MS, pdFALSE, NULL, vControlTimerCallback);
	
	res &= xTaskCreate(vControlTask, "control", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, NULL);
	
	return res;
}}

#if (IS_REMOTE == 0)
void SERCOM2_Handler(void) {
	vSpiRegistersSpiInterruptHandler(&controlShiftRegisterConfig);
}
#endif