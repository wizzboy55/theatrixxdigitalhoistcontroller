#include <atmel_start.h>

#include "HardwareDescriptor.h"
#include "samgpio.h"
#include "debug_interface.h"
#include "utilities.h"

#include "FreeRTOS.h"
#include "task.h"

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	vTaskStartScheduler();
	
	DEBUG_printf( ("Error! Scheduler returned!\n") );

	utils_WatchDogReset();
}
