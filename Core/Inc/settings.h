/*
 * settings.h
 *
 *  Created on: Mar 24, 2026
 *      Author: d.pons
 */

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include "stm32f4xx_hal.h"

#define TIME_QUANTA 16
#define NUM_CAN_BUSES 2

#define CFG_BUILD_NUM 100	// v1.0

// CAN buses
extern CAN_HandleTypeDef* Settings_CAN_Handles[NUM_CAN_BUSES];

// CAN settings
typedef struct {
    uint32_t NomSpeed;
    uint32_t FdSpeed;
    uint8_t Enabled;
    uint8_t ListenOnly;
    uint8_t FdMode;
} Settings_CANTypeDef;

static const Settings_CANTypeDef Settings_CAN_Default = {0, 0, 0, 0, 0};

extern Settings_CANTypeDef Settings_CAN[NUM_CAN_BUSES];

void Settings_Init_CAN1(CAN_HandleTypeDef* hcan);
void Settings_Init_CAN2(CAN_HandleTypeDef* hcan);

void Settings_Set_CAN_Config(CAN_HandleTypeDef* hcan, Settings_CANTypeDef* settings);
void Settings_Set_CAN_Filtering(CAN_HandleTypeDef* hcan);

#endif /* INC_SETTINGS_H_ */
