/*
 * can_frame.h
 *
 *  Created on: Mar 29, 2026
 *      Author: d.pons
 */

#ifndef SRC_CAN_FRAME_H_
#define SRC_CAN_FRAME_H_

#include "stm32f4xx_hal_can.h"

typedef struct {
	/**
	 * Contains the header of the CAN frame:
	 *     - IDE: CAN_identifier_type type of ID used for this frame (std or extended)
	 *     - StdId: Standard section of the frame ID. Check IDE if equals to CAN_ID_STD
	 *     - ExtId: Extended section of the frame ID. Check IDE if equals to CAN_ID_EXT
	 *     - RTR: Type of frame for the message that will be transmitted. Can be:
	 *     		  CAN_RTR_DATA for standard data frames
	 *     		  CAN_RTR_REMOTE to request data from another CAN node
	 *     - DLC: Data length of frame (max 8 byte)
	 *     - FilterMatchIndex: Index of filter matching this ID
	 */
	CAN_RxHeaderTypeDef Header;

	/**
	 * Frame data frame payload
	 */
	uint8_t Data[8];
} CAN_RxFrameTypeDef;

CAN_RxFrameTypeDef CAN_RxFrame_Default;

typedef struct {
	/**
	 * Contains the header of the CAN frame:
	 *     - IDE: CAN_identifier_type type of ID used for this frame (std or extended)
	 *     - StdId: Standard section of the frame ID. Check IDE if equals to CAN_ID_STD
	 *     - ExtId: Extended section of the frame ID. Check IDE if equals to CAN_ID_EXT
	 *     - RTR: Type of frame for the message that will be transmitted. Can be:
ß	 *     		  CAN_RTR_DATA for standard data frames
	 *     		  CAN_RTR_REMOTE to request data from another CAN node
	 *     - DLC: Data length of frame (max 8 byte)
	 *     - TransmitGlobalTime: Specifies whether the timestamp counter value captured on start of frame transmission is sent.
	 *     						 In such case, the timestamp occupies Data[6] and Data[7] bytes
	 */
	CAN_TxHeaderTypeDef Header;

	/**
	 * Frame data frame payload
	 */
	uint8_t Data[8];
} CAN_TxFrameTypeDef;

CAN_TxFrameTypeDef CAN_TxFrame_Default;


#endif /* SRC_CAN_FRAME_H_ */
