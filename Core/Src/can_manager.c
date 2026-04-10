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

void CAN_Setup_Settings() {
	uint32_t can_freq = HAL_RCC_GetPCLK1Freq() / Settings_CAN_Handles[0]->Init.Prescaler;	    	// CAN frequency from peripheral APB1 clock and prescaler
	uint32_t can_baudrate = can_freq / TIME_QUANTA;

	Settings_CANTypeDef settings;
	settings.NomSpeed = can_baudrate;
	settings.FdSpeed = 0;
	settings.Enabled = 1;
	settings.ListenOnly = 0;
	settings.FdMode = 0;

	Settings_Set_CAN_Config(Settings_CAN_Handles[0], &settings);

	Settings_CAN[1] = Settings_CAN_Default;
}

void CAN_DisplayFrame(CAN_RxFrameTypeDef* can_frame, uint8_t which_bus) {
	COMM_Send_CAN_Frame_To_Buffer(GVRET_Get_Serial_Buffer(), can_frame, which_bus);
}

void CAN_SendFrame(CAN_HandleTypeDef* can_handle, CAN_TxFrameTypeDef* frame) {
	uint32_t tx_mailbox;
	HAL_CAN_AddTxMessage(can_handle, &frame->Header, frame->Data, &tx_mailbox);
}

void CAN_Loop() {
	CAN_RxFrameTypeDef can_frame;
    uint32_t serial_length = COMM_Get_Available_Bytes(GVRET_Get_Serial_Buffer());

    // Read CAN1
    if (Settings_CAN_Handles[0] != NULL) {
    	Settings_CANTypeDef* settings = &Settings_CAN[0];
    	if (settings && settings->Enabled != 0) {
    		while (HAL_CAN_GetRxFifoFillLevel(Settings_CAN_Handles[0], CAN_RX_FIFO0) > 0 && serial_length < COMM_BUFF_SIZE - BUFF_SIZE_THRESHOLD) {
    			if (settings->FdMode == 0) {
    				// Receive new frame
    				HAL_CAN_GetRxMessage(Settings_CAN_Handles[0], CAN_RX_FIFO0, &can_frame.Header, can_frame.Data);

					// Send to buffer
    				CAN_DisplayFrame(&can_frame, 0);
    			}

    			// Update serial buffer length
    			serial_length = COMM_Get_Available_Bytes(GVRET_Get_Serial_Buffer());
    		}
    	}
    }

    // Read CAN2
    if (Settings_CAN_Handles[1] != NULL) {
        	Settings_CANTypeDef* settings = &Settings_CAN[1];
        	if (settings && settings->Enabled != 0) {
        		while (HAL_CAN_GetRxFifoFillLevel(Settings_CAN_Handles[1], CAN_RX_FIFO1) > 0 && serial_length < COMM_BUFF_SIZE - BUFF_SIZE_THRESHOLD) {
        			if (settings->FdMode == 0) {
        				// Receive new frame
        				HAL_CAN_GetRxMessage(Settings_CAN_Handles[1], CAN_RX_FIFO1, &can_frame.Header, can_frame.Data);

    					// Send to buffer
        				CAN_DisplayFrame(&can_frame, 1);
        			}
        		}

        		// Update serial buffer length
        		serial_length = COMM_Get_Available_Bytes(GVRET_Get_Serial_Buffer());
        	}
        }
}
