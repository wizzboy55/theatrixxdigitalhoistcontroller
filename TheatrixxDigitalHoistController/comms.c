/*
 * comms.c
 *
 * Created: 2020-05-14 5:12:22 PM
 *  Author: charl
 */ 

#include "comms.h"
#include "HardwareDescriptor.h"
#include "queue.h"

#include "control.h"
#include "remote.h"

HoistControl_t nextMessage;
BaseType_t pendingMessage = pdFALSE;

#define COMMS_ADDRESS 0x55;
#define COMMS_TIMEOUT 150

Rs485Status_t rs485Status;

Checksum_t xCommsComputeChecksum(uint8_t* message, Rs485Size_t size) {
	if(message == NULL) {
		return 0;
	}	
	
	Checksum_t checksum = 0;
	for(Rs485Size_t i = 0; i < size; i++) {
		checksum += message[i];
	}
	
	return checksum;
}

void vCommsSendMessage(HoistControl_t* hoistControl) {
	nextMessage = *hoistControl;
	pendingMessage = pdTRUE;
}

void vCommsMasterTask(void *p) {
	
	ReplyMessage_t replyMessage;
	HoistMessage_t currentMessage;
	
	rs485Status.config = &rs485Config;
	rs485Status.rxBuffer = (uint8_t*)&replyMessage;
	rs485Status.rxBufferSize = sizeof(replyMessage);
	rs485Status.rxQueue = xQueueCreate(5, sizeof(replyMessage));
	rs485Status.txBuffer = (uint8_t*)&currentMessage;
	rs485Status.txBufferSize = sizeof(currentMessage);
	rs485Status.txTask = xTaskGetCurrentTaskHandle();
	
	for(;;) {
		vTaskDelay(50 / portTICK_PERIOD_MS);
		
		if(pendingMessage) {
			pendingMessage = pdFALSE;
			currentMessage.hoistControl = nextMessage;
			currentMessage.address = COMMS_ADDRESS;
			currentMessage.sequence++;
			currentMessage.checksum = 0;
			currentMessage.checksum = xCommsComputeChecksum((uint8_t*)&currentMessage, sizeof(currentMessage));

			xRs485SendMessage(&rs485Status, (uint8_t*)&currentMessage, sizeof(currentMessage), portMAX_DELAY);
		} else {
			continue;
		}
		
		ReplyMessage_t rxMessage;
		
		if(rs485Status.rxQueue != NULL) {
			while(xQueueReceive(rs485Status.rxQueue, &rxMessage, pdMS_TO_TICKS(COMMS_TIMEOUT)) == pdTRUE) {
				if(rxMessage.reply == eOK) {
					vRs485SetLinkLed(&rs485Config);
					vRemoteSetLinkEmergency(pdTRUE, pdFALSE);
				} else if(rxMessage.reply == eEStop) {
					vRs485SetLinkLed(&rs485Config);
					vRemoteSetLinkEmergency(pdTRUE, pdTRUE);
				} else {
					vRs485ClearLinkLed(&rs485Config);
					vRemoteSetLinkEmergency(pdFALSE, pdTRUE);
				}
				break;
			}
		}
	}
}

void vCommsSlaveTask(void *p) {
	
	ReplyMessage_t replyMessage;
	HoistMessage_t currentMessage;
	
	rs485Status.config = &rs485Config;
	rs485Status.txBuffer = (uint8_t*)&replyMessage;
	rs485Status.txBufferSize = sizeof(replyMessage);
	rs485Status.txTask = xTaskGetCurrentTaskHandle();
	rs485Status.rxBuffer = (uint8_t*)&currentMessage;
	rs485Status.rxBufferSize = sizeof(currentMessage);
	rs485Status.rxQueue = xQueueCreate(1, sizeof(currentMessage));
	
	for(;;) {
		HoistMessage_t rxMessage;
		
		if(rs485Status.rxQueue != NULL) {
			if(xQueueReceive(rs485Status.rxQueue, &rxMessage, pdMS_TO_TICKS(COMMS_TIMEOUT)) == pdTRUE) {
				Checksum_t checksum = xCommsComputeChecksum((uint8_t*)& currentMessage, sizeof(currentMessage) - sizeof(Checksum_t));
				if(checksum != currentMessage.checksum) {
					vControlStop();
					continue;
				}
				
				vControlNewMessage(&currentMessage.hoistControl);

				replyMessage.address = currentMessage.address;
				replyMessage.checksum = 0;
				replyMessage.sequence = currentMessage.sequence;
				if(xControlEStopPresent() == pdTRUE) {
					replyMessage.reply = eOK;
				} else {
					replyMessage.reply = eEStop;
				}
				replyMessage.checksum = xCommsComputeChecksum((uint8_t*)&replyMessage, sizeof(replyMessage));

				xRs485SendMessage(&rs485Status, (uint8_t*)&replyMessage, sizeof(replyMessage), portMAX_DELAY);
			}
		} else {
			vControlStop();
		}
	}
}

BaseType_t xCommsInit(BaseType_t isMaster) {
	BaseType_t res;
	
	res = xRs485Init(&rs485Config);
	
	if(isMaster) {
		res &= xTaskCreate(vCommsMasterTask, "comms", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);	
	} else {
		res &= xTaskCreate(vCommsSlaveTask, "comms", configMINIMAL_STACK_SIZE, NULL, configMAX_PRIORITIES - 1, NULL);	
	}
	
	return res;
}

void SERCOM3_Handler(void) {
	vRs485InterruptHandler(&rs485Status);
}