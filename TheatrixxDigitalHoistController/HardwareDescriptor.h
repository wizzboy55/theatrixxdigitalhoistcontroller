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
#include "spi_shiftregisters.h"
#include "gpio_shiftregisters.h"
#include "rs485.h"

#define IS_REMOTE 1

#define GPIO_SWITCH_LOCK	GPIO(GPIO_PORTA, 2)
#define GPIO_SWITCH_RESET	GPIO(GPIO_PORTA, 3)
#define GPIO_SWITCH_GO		GPIO(GPIO_PORTA, 4)
#define GPIO_LED_ESTOP		GPIO(GPIO_PORTA, 14)
#define GPIO_LED_LINK		GPIO(GPIO_PORTA, 15)

#define GPIO_SIGNAL_ESTOP	GPIO(GPIO_PORTA, 0)
#define GPIO_DIAG			GPIO(GPIO_PORTA, 1)
#define GPIO_DISABLE		GPIO(GPIO_PORTA, 14)

SercomSpiHwConfig_t remoteLedSpiHwConfig;
SercomSpiHwConfig_t remoteSwitchSpiHwConfig;

SercomSpiHwConfig_t controlSpiHwConfig;

SpiShiftRegistersConfig_t remoteLedShiftRegisterConfig;
SpiShiftRegistersConfig_t remoteSwitchShiftRegisterConfig;

SpiShiftRegistersConfig_t controlShiftRegisterConfig;

GpioShiftRegistersConfig_t remoteLedShiftRegisterConfigGpio;
GpioShiftRegistersConfig_t remoteSwitchShiftRegisterConfigGpio;

GpioShiftRegistersConfig_t controlShiftRegisterConfigGpio;

#define SWITCHES_UPMASK 0xAA
#define SWITCHES_DOWNMASK 0x55

typedef union __attribute__((packed)) {
	struct __attribute__((packed)) {
		uint8_t A_down:1;
		uint8_t A_up:1;
		uint8_t B_down:1;
		uint8_t B_up:1;
		uint8_t C_down:1;
		uint8_t C_up:1;
		uint8_t D_down:1;
		uint8_t D_up:1;
	};
	uint8_t reg;
} HoistControlRegister_t;

typedef union __attribute__((packed)) {
	struct __attribute__((packed)) {
		HoistControlRegister_t Hoist45_48;
		HoistControlRegister_t Hoist41_44;
		HoistControlRegister_t Hoist37_40;
		HoistControlRegister_t Hoist33_36;
		HoistControlRegister_t Hoist29_32;
		HoistControlRegister_t Hoist25_28;
		HoistControlRegister_t Hoist21_24;
		HoistControlRegister_t Hoist17_20;
		HoistControlRegister_t Hoist13_16;
		HoistControlRegister_t Hoist9_12;
		HoistControlRegister_t Hoist5_8;
		HoistControlRegister_t Hoist1_4;
	};
	uint8_t reg[12];
} HoistControl_t;

typedef union __attribute__((packed)) {
	struct __attribute__((packed)) {
		HoistControl_t hoistControl;
		uint8_t led_go;
		uint8_t led_link;
	};
	uint8_t reg[sizeof(HoistControl_t)+2];
} ControlRegisters_t;

Rs485Config_t rs485Config;

#endif /* HARDWAREDESCRIPTOR_H_ */