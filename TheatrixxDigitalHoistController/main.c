#include <atmel_start.h>

#include "HardwareDescriptor.h"
#include "samgpio.h"
#include "debug_interface.h"
#include "utilities.h"

#include "remote.h"
#include "control.h"
#include "comms.h"

#include "FreeRTOS.h"
#include "task.h"

#define IS_REMOTE pdTRUE

int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();
	
	if(IS_REMOTE) {
		xRemoteInit();
	} else {
		xControlInit();
	}
	
	xCommsInit(IS_REMOTE);
	
	vTaskStartScheduler();
	
	DEBUG_printf( ("Error! Scheduler returned!\n") );

	utils_WatchDogReset();
}

void vApplicationIdleHook(void) {
	for(;;) {

	}
}