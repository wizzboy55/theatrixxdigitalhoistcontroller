/*
 * HardwareDescriptor.c
 *
 * Created: 2020-03-09 9:12:07 AM
 *  Author: charl
 */ 

#include "HardwareDescriptor.h"

SercomSpiHwConfig_t remoteLedSpiHwConfig = {
	.module = SERCOM2,
	.pin_mosi = GPIO(GPIO_PORTA, 8),
	.pin_miso = GPIO(GPIO_PORTA, 10),
	.pin_clk = GPIO(GPIO_PORTA, 11),
	.pin_cs = GPIO(GPIO_PORTA, 9),
	.pinmux_sercom = PINMUX_PA08D_SERCOM2_PAD0,
	.baud = 100000,
};

SercomSpiHwConfig_t remoteSwitchSpiHwConfig = {
	.module = SERCOM0,
	.pin_mosi = GPIO(GPIO_PORTA, 4),
	.pin_miso = GPIO(GPIO_PORTA, 6),
	.pin_clk = GPIO(GPIO_PORTA, 7),
	.pin_cs = GPIO(GPIO_PORTA, 5),
	.pinmux_sercom = PINMUX_PA05D_SERCOM0_PAD1,
	.baud = 100000,
};

SercomSpiHwConfig_t controlSpiHwConfig = {
	.module = SERCOM2,
	.pin_mosi = GPIO(GPIO_PORTA, 8),
	.pin_miso = GPIO(GPIO_PORTA, 11),
	.pin_clk = GPIO(GPIO_PORTA, 9),
	.pin_cs = GPIO(GPIO_PORTA, 10),
	.pinmux_sercom = PINMUX_PA08D_SERCOM2_PAD0,
	.baud = 50000,
};

SpiShiftRegistersConfig_t remoteLedShiftRegisterConfig = {
	.latchPolarity = LatchHigh,
	.dataOrder = MSBFirst,
	.isInit = pdFALSE,
};

SpiShiftRegistersConfig_t remoteSwitchShiftRegisterConfig = {
	.latchPolarity = LatchLow,
	.dataOrder = MSBFirst,
	.isInit = pdFALSE,
};

SpiShiftRegistersConfig_t controlShiftRegisterConfig = {
	.latchPolarity = LatchHigh,
	.dataOrder = MSBFirst,
	.isInit = pdFALSE,
};

// Deprecated
GpioShiftRegistersConfig_t remoteLedShiftRegisterConfigGpio = {
	.hw.gpio_clk = GPIO(GPIO_PORTA, 11),
	.hw.gpio_cs = GPIO(GPIO_PORTA, 9),
	.hw.gpio_mosi = GPIO(GPIO_PORTA, 8),
	.hw.gpio_miso = GPIO(GPIO_PORTA, 10),
	.dataOrder = MSBFirst,
	.isInit = pdFALSE,
};

// Deprecated
GpioShiftRegistersConfig_t remoteSwitchShiftRegisterConfigGpio = {
	.hw.gpio_clk = GPIO(GPIO_PORTA, 7),
	.hw.gpio_cs = GPIO(GPIO_PORTA, 5),
	.hw.gpio_mosi = GPIO(GPIO_PORTA, 10),
	.hw.gpio_miso = GPIO(GPIO_PORTA, 6),
	.dataOrder = MSBFirst,
	.isInit = pdFALSE,
};

// Deprecated
GpioShiftRegistersConfig_t controlShiftRegisterConfigGpio = {
	.hw.gpio_clk = GPIO(GPIO_PORTA, 9),
	.hw.gpio_cs = GPIO(GPIO_PORTA, 10),
	.hw.gpio_mosi = GPIO(GPIO_PORTA, 8),
	.hw.gpio_miso = GPIO(GPIO_PORTA, 11),
	.dataOrder = MSBFirst,
	.isInit = pdFALSE,
};

Rs485Config_t rs485Config = {
	.module = SERCOM3,
	.rxpin = GPIO(GPIO_PORTA, 17),
	.txpin = GPIO(GPIO_PORTA, 16),
	.dirpin = GPIO(GPIO_PORTA, 18),
	.pinmux = PINMUX_PA17D_SERCOM3_PAD1,
	.ledpin = GPIO(GPIO_PORTA, 15),
	.ledinv = pdTRUE,
	.baudrate = 115200,
	.dataOrder = MSBFirst,
	.parity = EvenParity,
	.stopBits = TwoStopBit,
	.characterSize = Char9bit,
	.guardTime = 5,
	.useInterrupt = pdTRUE,
};