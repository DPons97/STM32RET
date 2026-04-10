/*
 * comm_buffer.h
 *
 *  Created on: Mar 24, 2026
 *      Author: d.pons
 */

#ifndef INC_COMM_BUFFER_H_
#define INC_COMM_BUFFER_H_

#include "can_frame.h"

#define COMM_BUFF_SIZE 2048

typedef struct {
	uint8_t COMM_Buffer[COMM_BUFF_SIZE];

	uint32_t COMM_Buffer_Len;
} COMM_BufferTypeDef;

uint32_t COMM_Get_Available_Bytes(COMM_BufferTypeDef* buffer);
void COMM_Buffer_Clear(COMM_BufferTypeDef* buffer);
void COMM_Send_CAN_Frame_To_Buffer(COMM_BufferTypeDef* buffer, CAN_RxFrameTypeDef* frame, uint8_t which_bus);


#endif /* INC_COMM_BUFFER_H_ */

