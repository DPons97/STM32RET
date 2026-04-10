/*
 * comm_buffer.c
 *
 *  Created on: Mar 24, 2026
 *      Author: d.pons
 */
#include "comm_buffer.h"

uint32_t COMM_Get_Available_Bytes(COMM_BufferTypeDef* buffer) {
	return buffer->COMM_Buffer_Len;
}

void COMM_Buffer_Clear(COMM_BufferTypeDef* buffer) {
	for (uint16_t i = 0; i < COMM_BUFF_SIZE; i++) {
		buffer->COMM_Buffer[i] = 0;
	}
	buffer->COMM_Buffer_Len = 0;
}


void COMM_Send_CAN_Frame_To_Buffer(COMM_BufferTypeDef* buffer, CAN_RxFrameTypeDef* frame, uint8_t which_bus) {
	uint8_t temp;

	if (buffer == NULL) {
		// todo Error
		return;
	}

	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = 0xF1;
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = 0; //0 = canbus frame sending

	uint32_t now = HAL_GetTick();
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(now & 0xFF);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(now >> 8);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(now >> 16);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(now >> 24);

	uint32_t id;
	if (frame->Header.IDE == CAN_ID_EXT) {
		id = frame->Header.ExtId;
	} else {
		id = frame->Header.StdId;
	}

	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(id & 0xFF);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(id >> 8);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(id >> 16);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = (uint8_t)(id >> 24);
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = frame->Header.DLC + (uint8_t)(which_bus << 4);
	for (int c = 0; c < frame->Header.DLC; c++) {
		buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = frame->Data[c];
	}
	//temp = checksumCalc(buff, 11 + frame.length);
	temp = 0;
	buffer->COMM_Buffer[buffer->COMM_Buffer_Len++] = temp;
		//Serial.write(buff, 12 + frame.length);
}
