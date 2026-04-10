/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __GVRET_COMM_H
#define __GVRET_COMM_H

/* Includes ------------------------------------------------------------------*/
#include "comm_buffer.h"

#include "stm32f4xx_hal.h"

typedef enum {
    IDLE,
    GET_COMMAND,
    BUILD_CAN_FRAME,
    TIME_SYNC,
	GET_DIG_INPUTS,
    GET_ANALOG_INPUTS,
    SET_DIG_OUTPUTS,
    SETUP_CANBUS,
    GET_CANBUS_PARAMS,
    GET_DEVICE_INFO,
    SET_SINGLEWIRE_MODE,
    SET_SYSTYPE,
    ECHO_CAN_FRAME,
    SETUP_EXT_BUSES
} GVRET_State;

typedef enum {
	PROTO_BUILD_CAN_FRAME = 0,
	PROTO_TIME_SYNC = 1,
	PROTO_DIG_INPUTS = 2,
	PROTO_ANA_INPUTS = 3,
	PROTO_SET_DIG_OUT = 4,
	PROTO_SETUP_CANBUS = 5,
	PROTO_GET_CANBUS_PARAMS = 6,
	PROTO_GET_DEV_INFO = 7,
	PROTO_SET_SW_MODE = 8,
	PROTO_KEEPALIVE = 9,
	PROTO_SET_SYSTYPE = 10,
	PROTO_ECHO_CAN_FRAME = 11,
	PROTO_GET_NUMBUSES = 12,
	PROTO_GET_EXT_BUSES = 13,
	PROTO_SET_EXT_BUSES = 14,
	PROTO_BUILD_FD_FRAME = 20,
	PROTO_SETUP_FD = 21,
	PROTO_GET_FD = 22,
} GVRET_Protocol;



void GVRET_Init_CAN1_Reference(CAN_HandleTypeDef* hcan);
void GVRET_Init_CAN2_Reference(CAN_HandleTypeDef* hcan);

COMM_BufferTypeDef* GVRET_Get_Serial_Buffer();

void GVRET_Process_Incoming_Byte(uint8_t in_byte);
uint8_t GVRET_Checksum_Calc(uint8_t *buffer, int length);

#endif /* __GVRET_COMM_H__ */
