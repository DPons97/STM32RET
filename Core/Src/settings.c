/*
 * settings.c
 *
 *  Created on: Mar 28, 2026
 *      Author: d.pons
 */

#include "settings.h"

#include "stm32f4xx_hal.h"

CAN_HandleTypeDef* Settings_CAN_Handles[NUM_CAN_BUSES] = {NULL, NULL};

Settings_CANTypeDef Settings_CAN[NUM_CAN_BUSES] = {Settings_CAN_Default, Settings_CAN_Default};

void Settings_Init_CAN1(CAN_HandleTypeDef* hcan) {
	Settings_CAN_Handles[0] = hcan;

	/*uint32_t can_freq = HAL_RCC_GetPCLK1Freq() / Settings_CAN_Handles[0]->Init.Prescaler;	    	// CAN frequency from peripheral APB1 clock and prescaler
	uint32_t can_baudrate = can_freq / TIME_QUANTA;

	Settings_CANTypeDef settings;
	settings.NomSpeed = can_baudrate;
	settings.FdSpeed = 0;
	settings.Enabled = 1;
	settings.ListenOnly = 0;
	settings.FdMode = 0;

	Settings_CAN[0] = settings;
	Settings_Set_CAN_Config(Settings_CAN_Handles[0], &Settings_CAN[0]);*/
}

void Settings_Init_CAN2(CAN_HandleTypeDef* hcan) {
	Settings_CAN_Handles[1] = hcan;

	/*uint32_t can_freq = HAL_RCC_GetPCLK1Freq() / Settings_CAN_Handles[1]->Init.Prescaler;	    	// CAN frequency from peripheral APB1 clock and prescaler
	uint32_t can_baudrate = can_freq / TIME_QUANTA;

	Settings_CANTypeDef settings;
	settings.NomSpeed = can_baudrate;
	settings.FdSpeed = 0;
	settings.Enabled = 1;
	settings.ListenOnly = 0;
	settings.FdMode = 0;

	Settings_CAN[1] = settings;
	Settings_Set_CAN_Config(Settings_CAN_Handles[1], &Settings_CAN[1]);*/
}

void Settings_Update_CAN_Config(CAN_HandleTypeDef* hcan, uint8_t can_idx) {
	if (hcan == NULL) {
		// todo Error
		return;
	}

	// Speed
	hcan->Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / Settings_CAN[can_idx].NomSpeed) / TIME_QUANTA;

	// Silent (listen-only) mode
	if (Settings_CAN[can_idx].ListenOnly) {
		hcan->Init.Mode =  CAN_MODE_SILENT;
	} else {
		hcan->Init.Mode =  CAN_MODE_NORMAL;
	}

	hcan->Init.TimeSeg1 = CAN_BS1_13TQ;
	hcan->Init.TimeSeg2 = CAN_BS2_2TQ;

	if (HAL_CAN_Init(hcan) != HAL_OK)
	{
		// todo Error
	}
}

void Settings_Set_CAN_Config(CAN_HandleTypeDef* hcan, Settings_CANTypeDef* settings) {
	if (hcan == NULL) {
		// todo Error non fatal
		return;
	}

	uint8_t can_idx = 0;
	if (hcan->Instance == CAN1) {
		can_idx = 0;
	} else if (hcan->Instance == CAN2) {
		can_idx = 1;
	} else {
		// todo Error
		return;
	}

	HAL_CAN_Stop(hcan);

	if (settings->Enabled == 0) {
		Settings_CAN[can_idx].NomSpeed = 0;
		Settings_CAN[can_idx].FdSpeed = 0;
		Settings_CAN[can_idx].Enabled = 0;
		Settings_CAN[can_idx].ListenOnly = 0;
		Settings_CAN[can_idx].FdMode = 0;
	} else {
		Settings_CAN[can_idx].NomSpeed = settings->NomSpeed;
		Settings_CAN[can_idx].FdSpeed = settings->FdSpeed;
		Settings_CAN[can_idx].Enabled = settings->Enabled;
		Settings_CAN[can_idx].ListenOnly = settings->ListenOnly;
		Settings_CAN[can_idx].FdMode = settings->FdMode;

		Settings_Update_CAN_Config(hcan, can_idx);
		Settings_Set_CAN_Filtering(hcan);

		/* Start the CAN peripheral */
		if (HAL_CAN_Start(hcan) != HAL_OK)
		{
			/* Start Error */
			// Error_Handler();
			return;
		}

		if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO1_MSG_PENDING) != HAL_OK) {
			// Error
			return;
		}
	}
}

void Settings_Set_CAN_Filtering(CAN_HandleTypeDef* hcan) {
	if (hcan == NULL) {
		// todo Error non fatal
		return;
	}

	CAN_FilterTypeDef filter;

	filter.FilterActivation = ENABLE;

	// Filter for FIFO0
	if (hcan->Instance == CAN1) {
		filter.FilterBank = 0;		// CAN0 uses filter bank 0-13
	} else if (hcan->Instance == CAN2) {
		filter.FilterBank = 14;		// CAN1 uses filter bank 14-27
	}

	filter.SlaveStartFilterBank = 14;

	filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	filter.FilterIdHigh = 0x0000;
	filter.FilterIdLow = 0x0000;
	filter.FilterMaskIdHigh = 0x0000;
	filter.FilterMaskIdLow = 0x0000;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;

	if (HAL_CAN_ConfigFilter(hcan, &filter) != HAL_OK) {
		// todo Error
		return;
	}

	// Filter for FIFO1
	if (hcan->Instance == CAN1) {
		filter.FilterBank = 1;		// CAN0 uses filter bank 0-13
	} else if (hcan->Instance == CAN2) {
		filter.FilterBank = 15;		// CAN1 uses filter bank 14-27
	}

	filter.FilterFIFOAssignment = CAN_FILTER_FIFO1;
	filter.FilterIdHigh = 0x0000;
	filter.FilterIdLow = 0x0000;
	filter.FilterMaskIdHigh = 0x0000;
	filter.FilterMaskIdLow = 0x0000;
	filter.FilterMode = CAN_FILTERMODE_IDMASK;
	filter.FilterScale = CAN_FILTERSCALE_32BIT;

	if (HAL_CAN_ConfigFilter(hcan, &filter) != HAL_OK) {
		// todo Error
		return;
	}

}
