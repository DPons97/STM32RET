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

void Settings_Init_CAN1_Reference(CAN_HandleTypeDef* hcan) {
	Settings_CAN_Handles[0] = hcan;
}

void Settings_Init_CAN2_Reference(CAN_HandleTypeDef* hcan) {
	Settings_CAN_Handles[1] = hcan;
}

void Settings_Update_CAN_Config(CAN_HandleTypeDef* hcan) {
	if (hcan == NULL) {
		// todo Error
		return;
	}

	// Speed
	hcan->Init.Prescaler = (HAL_RCC_GetPCLK1Freq() / Settings_CAN[0].NomSpeed) / TIME_QUANTA;

	// Silent (listen-only) mode
	if (Settings_CAN[0].ListenOnly) {
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

		Settings_Update_CAN_Config(hcan);
		HAL_CAN_Start(hcan);
	}
}
