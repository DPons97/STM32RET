/*
 * can_manager.c
 *
 *  Created on: Mar 30, 2026
 *      Author: d.pons
 */
#include "can_manager.h"

#include "can_frame.h"
#include "comm_buffer.h"
#include "settings.h"
#include "gvret_comm.h"

#include "stm32f4xx_hal.h"

typedef struct {
	CAN_RxFrameTypeDef RxFrame;

	uint8_t Bus;
} RxFrameBufferElemTypeDef;

RxFrameBufferElemTypeDef Rx_FrameBuffer[RX_BUFF_SIZE];
uint32_t Rx_FrameBufferStart = 0;
uint32_t Rx_FrameBufferEnd = 0;

uint32_t              TxMailbox;  /* The number of the mail box that transmitted the Tx message */

void CAN_DisplayFrame(CAN_RxFrameTypeDef* can_frame, uint8_t which_bus) {
	if (COMM_Get_Available_Bytes(GVRET_Get_Serial_Buffer()) < COMM_BUFF_SIZE - BUFF_SIZE_THRESHOLD) {
		// Enough spaze available -> Write directly into GVRET buffer
		COMM_Send_CAN_Frame_To_Buffer(GVRET_Get_Serial_Buffer(), can_frame, which_bus);
	} else {
		// Copy can frame into RX buffer. It will be processed later on
		Rx_FrameBuffer[Rx_FrameBufferEnd].RxFrame = *can_frame;
		Rx_FrameBuffer[Rx_FrameBufferEnd].Bus = which_bus;
		Rx_FrameBufferEnd = (Rx_FrameBufferEnd + 1) % RX_BUFF_SIZE;
	}
}

void CAN_SendFrame(CAN_HandleTypeDef* can_handle, CAN_TxFrameTypeDef* frame) {
	/* It's mandatory to look for a free Tx mail box */
	while(HAL_CAN_GetTxMailboxesFreeLevel(can_handle) == 0); /* Wait till a Tx mailbox is free. Using while loop instead of HAL_Delay() */

	if (HAL_CAN_AddTxMessage(can_handle, &frame->Header, frame->Data, &TxMailbox) != HAL_OK)
	{
		/* Transmission request Error */
		// Error_Handler();
		return;
	}
}

void CAN_ProcessNextRxBufferBatch() {
	while (COMM_Get_Available_Bytes(GVRET_Get_Serial_Buffer()) < COMM_BUFF_SIZE - BUFF_SIZE_THRESHOLD) {
		if (Rx_FrameBufferStart == Rx_FrameBufferEnd) {
			return;
		}

		CAN_DisplayFrame(&Rx_FrameBuffer[Rx_FrameBufferStart].RxFrame, Rx_FrameBuffer[Rx_FrameBufferStart].Bus);
		Rx_FrameBufferStart = (Rx_FrameBufferStart + 1) % RX_BUFF_SIZE;
	}
}
