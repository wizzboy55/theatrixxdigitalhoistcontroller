/*
 * comms.h
 *
 * Created: 2020-05-14 5:12:13 PM
 *  Author: charl
 */ 


#ifndef COMMS_H_
#define COMMS_H_

#include "FreeRTOS.h"
#include "task.h"
#include "rs485.h"

#include "HardwareDescriptor.h"

typedef uint8_t Checksum_t;
enum CommsReplies_e {eNoReply = 0, eError = 0xAA, eOK = 0xCC, eEStop = 0xEE};

typedef struct __attribute__ ((packed)) {
	Rs485Address_t address;
	uint8_t sequence;
	HoistControl_t hoistControl;
	Checksum_t checksum;
} HoistMessage_t;

typedef struct __attribute__ ((packed)) {
	Rs485Address_t address;
	uint8_t sequence;
	enum CommsReplies_e reply;
	Checksum_t checksum;
} ReplyMessage_t;

void vCommsSendMessage(HoistControl_t* hoistControl);
BaseType_t xCommsInit(BaseType_t isMaster);

#endif /* COMMS_H_ */