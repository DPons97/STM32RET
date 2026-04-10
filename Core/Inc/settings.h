/*
 * settings.h
 *
 *  Created on: Mar 24, 2026
 *      Author: d.pons
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "stm32f4xx_hal_can.h"

#define TIME_QUANTA 16
#define NUM_CAN_BUSES 2

#define CFG_BUILD_NUM 100	// v1.0

// CAN buses
CAN_HandleTypeDef* Settings_CAN_Handles[NUM_CAN_BUSES] = {NULL, NULL};

// CAN settings
typedef struct {
    uint32_t NomSpeed;
    uint32_t FdSpeed;
    uint8_t Enabled;
    uint8_t ListenOnly;
    uint8_t FdMode;
} Settings_CANTypeDef;

const Settings_CANTypeDef Settings_CAN_Default = {0, 0, 0, 0, 0};

Settings_CANTypeDef Settings_CAN[NUM_CAN_BUSES] = {Settings_CAN_Default, Settings_CAN_Default};

void Settings_Init_CAN1_Reference(CAN_HandleTypeDef* hcan);
void Settings_Init_CAN2_Reference(CAN_HandleTypeDef* hcan);

void Settings_Set_CAN_Config(CAN_HandleTypeDef* hcan, Settings_CANTypeDef* settings);

#endif /* INC_SETTINGS_H_ */
