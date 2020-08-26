/*
 * control.h
 *
 * Created: 2020-03-18 3:01:02 PM
 *  Author: charl
 */ 


#ifndef CONTROL_H_
#define CONTROL_H_

#include "FreeRTOS.h"
#include "HardwareDescriptor.h"

BaseType_t xControlEStopPresent(void);
void vControlNewMessage(HoistControl_t *newMessage);
void vControlStop(void);
BaseType_t xControlInit(void);

#endif /* CONTROL_H_ */