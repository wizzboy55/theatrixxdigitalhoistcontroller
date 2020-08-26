/*
 * remote.h
 *
 * Created: 2020-03-18 3:00:49 PM
 *  Author: charl
 */ 


#ifndef REMOTE_H_
#define REMOTE_H_

#include "HardwareDescriptor.h"

BaseType_t xRemoteInit(void);
void vRemoteSetLinkEmergency(BaseType_t remLink, BaseType_t remEmergency);

#endif /* REMOTE_H_ */