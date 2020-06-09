/*
 * remote.c
 *
 * Created: 2020-03-18 3:00:56 PM
 *  Author: charl
 */ 

#include "remote.h"
#include "HardwareDescriptor.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "sameic.h"
#include "string.h"

#include "comms.h"

HoistControl_t hoists_newSwitches;
HoistControl_t hoists_lastSwitches;
HoistControl_t hoists_currentSwitches;
HoistControl_t hoists_emptySwitches;

BaseType_t remoteLink = 0;
BaseType_t remoteEmergency = 1;

TimerHandle_t blinkTimer;
uint8_t blink = 0;
#define BLINKMASK 0x01
#define BLINK_TIMER_HALFPERIOD 400

void vRemoteApplyButtons(uint8_t newSwitches, uint8_t lastSwitches, uint8_t oldSwitches, uint8_t* output) {
	
	uint8_t up_pressed = newSwitches & SWITCHES_UPMASK & (lastSwitches ^ 0xFF);
	*output ^=  up_pressed;	
	up_pressed = (up_pressed >> 1) ^ 0xFF;
	*output &= up_pressed;
	
	uint8_t down_pressed = newSwitches & SWITCHES_DOWNMASK & (lastSwitches ^ 0xFF);
	*output ^= down_pressed;
	down_pressed = (down_pressed << 1) ^ 0xFF;
	*output &= down_pressed;
}

void vRemoteSetLinkEmergency(BaseType_t remLink, BaseType_t remEmergency) {
	remoteLink = remLink;
	remoteEmergency = remEmergency;
}

void vRemoteTask(void* p) {
	BaseType_t reset;
	BaseType_t lock;
	BaseType_t go;
	
	memset(&hoists_currentSwitches, 0, sizeof(hoists_currentSwitches));
	memset(&hoists_lastSwitches, 0, sizeof(hoists_lastSwitches));
	memset(&hoists_emptySwitches, 0, sizeof(hoists_emptySwitches));
	
	for(;;) {
		vTaskDelay(10 / portTICK_PERIOD_MS);
			
		samgpio_setPinLevel(GPIO_LED_ESTOP, remoteEmergency != 0);
		//		samgpio_setPinLevel(GPIO_LED_LINK, remoteLink == 0);
			
		reset = samgpio_getPinLevel(GPIO_SWITCH_RESET) == 0;
		lock = samgpio_getPinLevel(GPIO_SWITCH_LOCK) == 0;
		go = samgpio_getPinLevel(GPIO_SWITCH_GO) == 0;
			
		if(reset == pdTRUE) {
			memset(&hoists_currentSwitches, 0, sizeof(hoists_currentSwitches));
		} else if(lock == pdFALSE) {
			vSpiRegistersReadWriteRegistersAsync(&remoteSwitchShiftRegisterConfig, NULL, (uint8_t*)&hoists_newSwitches, sizeof(hoists_newSwitches), xTaskGetCurrentTaskHandle());
			xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
			for(uint8_t i = 0; i < sizeof(HoistControl_t); i++) {
				hoists_newSwitches.reg[i] ^= 0xFF;	// Flip bits
			}
			for(uint8_t i = 0; i < sizeof(HoistControl_t); i++) {
				vRemoteApplyButtons(hoists_newSwitches.reg[i], hoists_lastSwitches.reg[i], hoists_currentSwitches.reg[i], &hoists_currentSwitches.reg[i]);
				hoists_lastSwitches.reg[i] = hoists_newSwitches.reg[i];
			}
		}
		
		if(lock && (blink & BLINKMASK)) {
			vSpiRegistersReadWriteRegistersAsync(&remoteLedShiftRegisterConfig, (uint8_t*)&hoists_emptySwitches, NULL, sizeof(hoists_emptySwitches), NULL);
		} else {
			vSpiRegistersReadWriteRegistersAsync(&remoteLedShiftRegisterConfig, (uint8_t*)&hoists_currentSwitches, NULL, sizeof(hoists_currentSwitches), NULL);
		}
		
		if(go == pdTRUE) {
			vCommsSendMessage(&hoists_currentSwitches);
		} else {
			vCommsSendMessage(&hoists_emptySwitches);
		}
	}
}

void vBlinkTimerCallback(TimerHandle_t xTimer) {
	blink++;
}

BaseType_t xRemoteInit(void) {
	
	remoteLedShiftRegisterConfig.hw = remoteLedSpiHwConfig;
	remoteSwitchShiftRegisterConfig.hw = remoteSwitchSpiHwConfig;
	
	BaseType_t res;
	res = xSpiRegistersInitHardware(&remoteLedShiftRegisterConfig);
	res = xSpiRegistersInitHardware(&remoteSwitchShiftRegisterConfig);
	
	samgpio_setPinFunction(GPIO_SWITCH_GO, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinDirection(GPIO_SWITCH_GO, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(GPIO_SWITCH_GO, GPIO_PULL_UP);
	samgpio_setPinFunction(GPIO_SWITCH_LOCK, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinDirection(GPIO_SWITCH_LOCK, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(GPIO_SWITCH_LOCK, GPIO_PULL_UP);
	samgpio_setPinFunction(GPIO_SWITCH_RESET, GPIO_PIN_FUNCTION_OFF);
	samgpio_setPinDirection(GPIO_SWITCH_RESET, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(GPIO_SWITCH_RESET, GPIO_PULL_UP);
	
	samgpio_setPinDirection(GPIO_LED_ESTOP, GPIO_DIRECTION_OUT);
	samgpio_setPinDirection(GPIO_LED_LINK, GPIO_DIRECTION_OUT);
	
	samgpio_setPinLevel(GPIO_LED_ESTOP, remoteEmergency != 0);
	samgpio_setPinLevel(GPIO_LED_LINK, remoteLink == 0);
	
	blinkTimer = xTimerCreate("BlinkTim", BLINK_TIMER_HALFPERIOD / portTICK_PERIOD_MS, pdTRUE, NULL, vBlinkTimerCallback);
	res &= xTimerStart(blinkTimer, portMAX_DELAY);
	res &= xTaskCreate(vRemoteTask, "remote", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, NULL);
	
	return res;
}

void SERCOM0_Handler(void) {
	vSpiRegistersSpiInterruptHandler(&remoteSwitchShiftRegisterConfig);
}

void SERCOM2_Handler(void) {
	vSpiRegistersSpiInterruptHandler(&remoteLedShiftRegisterConfig);
}