/*
 * can_manager.h
 *
 *  Created on: Mar 30, 2026
 *      Author: d.pons
 */

#ifndef INC_CAN_MANAGER_H_
#define INC_CAN_MANAGER_H_

#include "can_frame.h"

#define BUFF_SIZE_THRESHOLD 80

void CAN_Setup_Settings();
void CAN_DisplayFrame(CAN_RxFrameTypeDef* can_frame, uint8_t which_bus);
void CAN_SendFrame(CAN_HandleTypeDef* can_handle, CAN_TxFrameTypeDef* frame);
void CAN_Loop();


#endif /* INC_CAN_MANAGER_H_ */
