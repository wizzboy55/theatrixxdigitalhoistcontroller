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
ControlRegisters_t lastControlRegisters;

TimerHandle_t controlTimer;
#define CONTROL_TIMEOUT 150
BaseType_t controlTimedOut = pdTRUE;
uint8_t uValidMessagesReceived = 0;
#define CONTROL_VALIDMESSAGESTHRESHOLD 5
BaseType_t xControlEmergencyState = pdFALSE;
BaseType_t xControlLinkState = pdFALSE;

TimerHandle_t controlBlinkTimer;
uint8_t controlBlink = 0;

inline void vControlEnableContactors(void) {
	samgpio_setPinFast(GPIO_DISABLE);
}

inline void vControlDisableContactors(void) {
	samgpio_clearPinFast(GPIO_DISABLE);
}

inline BaseType_t xControlEStopPresent(void) {
	return samgpio_getPinLevel(GPIO_SIGNAL_ESTOP) == 1;
}

inline BaseType_t xControlShiftRegistersDiagnosticOK(void) {
	return samgpio_getPinLevel(GPIO_DIAG) == 1;
}

BaseType_t xControlRegistersAreEmpty(ControlRegisters_t* controlRegisters) {
	for(uint8_t i = 0; i < sizeof(HoistControl_t); i++) {
		if(controlRegisters->hoistControl.reg[i] != 0x00) {
			return pdFALSE;
		}
	}

	return pdTRUE;
}

BaseType_t xControlRegistersAreDifferent(ControlRegisters_t* reg1, ControlRegisters_t* reg2) {
	for(uint8_t i = 0; i < sizeof(HoistControl_t); i++) {
		if(reg1->hoistControl.reg[i] != reg2->hoistControl.reg[i]) {
			return pdTRUE;
		}
	}

	return pdFALSE;
}

void vControlStop(void) {
	controlTimedOut = pdTRUE;
	vControlDisableContactors();
	xGpioShiftRegistersPush(&controlShiftRegisterConfigGpio, emptyRegisters.reg, NULL, sizeof(ControlRegisters_t));
}

void vControlNewMessage(HoistControl_t *newMessage) {
	uint8_t led_go = 0;
	for(uint8_t i = 0; i < sizeof(HoistControl_t); i++) {
		controlRegisters.hoistControl.reg[i] = newMessage->reg[i];
		if((i % 2) == 0) {
			if(newMessage->reg[i] != 0x00 || newMessage->reg[i+1] != 0x00) {
				led_go |= 1<<(5-(i / 2));
			}
		}
	}
	
	controlRegisters.led_go = led_go;
	
	if(uValidMessagesReceived < CONTROL_VALIDMESSAGESTHRESHOLD) {
		uValidMessagesReceived++;
	} else {
		xTimerReset(controlTimer, portMAX_DELAY);
		controlTimedOut = pdFALSE;
		xControlLinkState = pdTRUE;
	}
}

void vControlBlinkTimerCallback(TimerHandle_t xTimer) {
	controlBlink++;
}

void vControlTask(void* p) {
	
	memset(&emptyRegisters, 0, sizeof(emptyRegisters));

	vControlDisableContactors();
	for(uint8_t i = 0; i < 6; i++) {
		controlRegisters.led_link = (1 << i);
		controlRegisters.led_go = (1 << i);
		vSpiRegistersWriteRegistersAsync(&controlShiftRegisterConfig, controlRegisters.reg, sizeof(ControlRegisters_t), NULL);
		vTaskDelay(pdMS_TO_TICKS(100));
	}

	for(;;) {

		xControlEmergencyState = xControlEStopPresent() == pdFALSE;

		if(xControlLinkState && xControlEmergencyState == pdFALSE) {
			controlRegisters.led_link = 0xFF;
			emptyRegisters.led_link = 0xFF;
		} else if(xControlLinkState && (controlBlink & BLINKMASK)) {
			controlRegisters.led_link = 0xFF;
			emptyRegisters.led_link = 0xFF;
		} else {
			controlRegisters.led_link = 0x00;
			emptyRegisters.led_link = 0x00;
		}

		if(controlTimedOut || xControlEStopPresent() == pdFALSE) {
			vControlDisableContactors();
			vSpiRegistersWriteRegistersAsync(&controlShiftRegisterConfig, emptyRegisters.reg, sizeof(ControlRegisters_t), NULL);
		} else {
			if(xControlRegistersAreEmpty(&controlRegisters)) {
				vControlDisableContactors();
				vSpiRegistersWriteRegistersAsync(&controlShiftRegisterConfig, emptyRegisters.reg, sizeof(ControlRegisters_t), NULL);
			} else {
				//vControlDisableContactors();
				if(xControlRegistersAreDifferent(&controlRegisters, &lastControlRegisters)) {
					vSpiRegistersWriteRegistersAsync(&controlShiftRegisterConfig, controlRegisters.reg, sizeof(ControlRegisters_t), NULL);
				}
				vControlEnableContactors();
			}
		}
		lastControlRegisters = controlRegisters;
		vTaskDelay(25 / portTICK_PERIOD_MS);
	}
}

void vControlTimerCallback(TimerHandle_t xTimer) {
	uValidMessagesReceived = 0;
	controlTimedOut = pdTRUE;
	xControlLinkState = pdFALSE;
}

BaseType_t xControlInit(void) {
	BaseType_t res;
	
	controlShiftRegisterConfig.hw = controlSpiHwConfig;
	
	res = xSpiRegistersInitHardware(&controlShiftRegisterConfig);
	
	vControlDisableContactors();
	samgpio_setPinDirection(GPIO_DISABLE, GPIO_DIRECTION_OUT);
	
	samgpio_setPinDirection(GPIO_DIAG, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(GPIO_DIAG, GPIO_PULL_UP);
	
	samgpio_setPinDirection(GPIO_SIGNAL_ESTOP, GPIO_DIRECTION_IN);
	samgpio_setPinPullMode(GPIO_SIGNAL_ESTOP, GPIO_PULL_DOWN);

	PORT->Group[GPIO_PORTA].PINCFG[8].bit.DRVSTR = 1;
	PORT->Group[GPIO_PORTA].PINCFG[9].bit.DRVSTR = 1;
	PORT->Group[GPIO_PORTA].PINCFG[10].bit.DRVSTR = 1;
	PORT->Group[GPIO_PORTA].PINCFG[11].bit.DRVSTR = 1;
	
	controlTimer = xTimerCreate("ControlTimer", CONTROL_TIMEOUT / portTICK_PERIOD_MS, pdFALSE, NULL, vControlTimerCallback);
	
	controlBlinkTimer = xTimerCreate("BlinkTim", BLINK_TIMER_HALFPERIOD / portTICK_PERIOD_MS, pdTRUE, NULL, vControlBlinkTimerCallback);
	res &= xTimerStart(controlBlinkTimer, portMAX_DELAY);

	res &= xTaskCreate(vControlTask, "control", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 2, NULL);
	
	return res;
}

#if (IS_REMOTE == 0)
void SERCOM2_Handler(void) {
	vSpiRegistersSpiInterruptHandler(&controlShiftRegisterConfig);
}
#endif