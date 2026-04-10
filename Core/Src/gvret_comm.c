/*
 * gvret_comm.c
 *
 *  Created on: Mar 20, 2026
 *      Author: d.pons
 */

/* Includes ------------------------------------------------------------------*/
#include "gvret_comm.h"

#include "comm_buffer.h"
#include "can_manager.h"
#include "can_frame.h"
#include "settings.h"


// FSM
GVRET_State State = IDLE;

// Settings
uint8_t Use_Binary_Serial_Comm = 0;

// Buffers
COMM_BufferTypeDef GVRET_Serial_Buffer;

int Step = 0;
uint8_t Buff[20];
uint32_t Build_Int = 0;

uint8_t Out_Bus = 0;
CAN_RxFrameTypeDef Build_Out_RxFrame;
CAN_TxFrameTypeDef Build_Out_TxFrame;

COMM_BufferTypeDef* GVRET_Get_Serial_Buffer() {
	return &GVRET_Serial_Buffer;
}

void GVRET_Process_Incoming_Byte(uint8_t byte) {
	uint32_t busSpeed = 0;
	uint32_t now = HAL_GetTick();

	uint8_t temp8;
	uint16_t temp16;

	switch (State) {
	case IDLE:
		if(byte == 0xF1)
		{
			State = GET_COMMAND;
		}
		else if(byte == 0xE7)
		{
			Use_Binary_Serial_Comm = 1;
		}
		else
		{
			// console.rcvCharacter((uint8_t) byte);
		}
		break;
	case GET_COMMAND:
		switch(byte)
		{
		case PROTO_BUILD_CAN_FRAME:
			State = BUILD_CAN_FRAME;
			Buff[0] = 0xF1;
			Step = 0;
			Build_Out_TxFrame = CAN_TxFrame_Default;
			break;
		case PROTO_TIME_SYNC:
			State = TIME_SYNC;
			Step = 0;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 1; //time sync
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t) (now & 0xFF);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t) (now >> 8);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t) (now >> 16);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t) (now >> 24);
			break;
		case PROTO_DIG_INPUTS:
			//immediately return the data for digital inputs
			temp8 = 0; //getDigital(0) + (getDigital(1) << 1) + (getDigital(2) << 2) + (getDigital(3) << 3) + (getDigital(4) << 4) + (getDigital(5) << 5);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 2; //digital inputs
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp8;
			temp8 = GVRET_Checksum_Calc(Buff, 2);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++]  = temp8;
			State = IDLE;
			break;
		case PROTO_ANA_INPUTS:
			//immediately return data on analog inputs
			temp16 = 0;// getAnalog(0);  // Analogue input 1
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 3;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp16 = 0;//getAnalog(1);  // Analogue input 2
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp16 = 0;//getAnalog(2);  // Analogue input 3
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp16 = 0;//getAnalog(3);  // Analogue input 4
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp16 = 0;//getAnalog(4);  // Analogue input 5
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp16 = 0;//getAnalog(5);  // Analogue input 6
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp16 = 0;//getAnalog(6);  // Vehicle Volts
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp16 & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (uint8_t)(temp16 >> 8);
			temp8 = GVRET_Checksum_Calc(Buff, 9);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = temp8;
			State = IDLE;
			break;
		case PROTO_SET_DIG_OUT:
			State = SET_DIG_OUTPUTS;
			Buff[0] = 0xF1;
			break;
		case PROTO_SETUP_CANBUS:
			State = SETUP_CANBUS;
			Step = 0;
			Buff[0] = 0xF1;
			Build_Int = 0;
			break;
		case PROTO_GET_CANBUS_PARAMS:
			//immediately return data on canbus params
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 6;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[0].Enabled + ((unsigned char) Settings_CAN[0].ListenOnly << 4);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[0].NomSpeed;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[0].NomSpeed >> 8;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[0].NomSpeed >> 16;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[0].NomSpeed >> 24;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[1].Enabled + ((unsigned char) Settings_CAN[1].ListenOnly << 4);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[1].NomSpeed;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[1].NomSpeed >> 8;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[1].NomSpeed >> 16;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = Settings_CAN[1].NomSpeed >> 24;
			State = IDLE;
			break;
		case PROTO_GET_DEV_INFO:
			//immediately return device information
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 7;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = CFG_BUILD_NUM & 0xFF;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = (CFG_BUILD_NUM >> 8);
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0x20;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0; //was single wire mode. Should be rethought for this board.
			State = IDLE;
			break;
		case PROTO_SET_SW_MODE:
			Buff[0] = 0xF1;
			State = SET_SINGLEWIRE_MODE;
			Step = 0;
			break;
		case PROTO_KEEPALIVE:
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0x09;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xDE;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xAD;
			State = IDLE;
			break;
		case PROTO_SET_SYSTYPE:
			Buff[0] = 0xF1;
			State = SET_SYSTYPE;
			Step = 0;
			break;
		case PROTO_ECHO_CAN_FRAME:
			State = ECHO_CAN_FRAME;
			Buff[0] = 0xF1;
			Step = 0;
			Build_Out_RxFrame = CAN_RxFrame_Default;
			break;
		case PROTO_GET_NUMBUSES:
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 12;

			// todo Check this is the max num buses and not enabled ones
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = NUM_CAN_BUSES;
			State = IDLE;
			break;
		case PROTO_GET_EXT_BUSES:
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++]  = 0xF1;
			GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++]  = 13;
			for (int u = 2; u < 17; u++) GVRET_Serial_Buffer.COMM_Buffer[GVRET_Serial_Buffer.COMM_Buffer_Len++] = 0;
			Step = 0;
			State = IDLE;
			break;
		case PROTO_SET_EXT_BUSES:
			State = SETUP_EXT_BUSES;
			Step = 0;
			Buff[0] = 0xF1;
			Build_Int = 0;
			break;
		}
		break;
	case BUILD_CAN_FRAME:
		Buff[1 + Step] = byte;
		switch(Step)
		{
		case 0:
			Build_Out_TxFrame.Header.StdId = byte;
			break;
		case 1:
			Build_Out_TxFrame.Header.StdId |= byte << 8;
			break;
		case 2:
			Build_Out_TxFrame.Header.StdId |= byte << 16;
			break;
		case 3:
			Build_Out_TxFrame.Header.StdId |= byte << 24;
			if(Build_Out_TxFrame.Header.StdId > 0x7FF) {			// if received id is greater than max std id
				Build_Out_TxFrame.Header.IDE = CAN_ID_EXT;		// Set extended frame id
				Build_Out_TxFrame.Header.ExtId = Build_Out_TxFrame.Header.StdId;
				Build_Out_TxFrame.Header.StdId = 0;
			} else Build_Out_TxFrame.Header.IDE = CAN_ID_STD;
			break;
		case 4:
			Out_Bus = byte & 1;
			break;
		case 5:
			Build_Out_TxFrame.Header.DLC = byte & 0xF;
			if(Build_Out_TxFrame.Header.DLC > 8)
			{
				Build_Out_TxFrame.Header.DLC = 8;
			}
			break;
		default:
			if(Step < Build_Out_TxFrame.Header.DLC + 6)
			{
				Build_Out_TxFrame.Data[Step - 6] = byte;
			}
			else
			{
				State = IDLE;

				Build_Out_TxFrame.Header.RTR = CAN_RTR_DATA;
				Build_Out_TxFrame.Header.TransmitGlobalTime = DISABLE;
				if (Out_Bus < NUM_CAN_BUSES) CAN_SendFrame(Settings_CAN_Handles[Out_Bus], &Build_Out_TxFrame);
			}
			break;
		}
		Step++;
		break;
	case TIME_SYNC:
		State = IDLE;
		break;
	case GET_DIG_INPUTS:
		// nothing to do
		break;
	case GET_ANALOG_INPUTS:
		// nothing to do
		break;
	case SET_DIG_OUTPUTS: //todo: validate the XOR byte
		// todo Ignore for now
		/*Buff[1] = byte;
		//temp8 = checksumCalc(Buff, 2);
		for(int c = 0; c < 8; c++){
			if(byte & (1 << c)) setOutput(c, true);
			else setOutput(c, false);
		}*/
		State = IDLE;
		break;
	case SETUP_CANBUS: //todo: validate checksum
		switch(Step)
		{
		case 0:
			Build_Int = byte;			// Copy byte 0 of nomSpeed (8 bit)
			break;
		case 1:
			Build_Int |= byte << 8;		// Copy byte 1 of nomSpeed (16 bit)
			break;
		case 2:
			Build_Int |= byte << 16;	// Copy byte 2 of nomSpeed (24 bit)
			break;
		case 3:
		{
			Build_Int |= byte << 24;	// Copy byte 3 of nomSpeed (32 bit)

			busSpeed = Build_Int & 0xFFFFF;		// Filter-out only 20 LSB (2^19 is not enough for 1MHz)
			if(busSpeed > 1000000) busSpeed = 1000000;		// Cap at 1MHz


			Settings_CANTypeDef settings_can;
			if(Build_Int > 0)
			{
				if(Build_Int & 0x80000000ul) //signals that enabled and listen only status are also being passed
				{
					if(Build_Int & 0x40000000ul)
					{
						settings_can.Enabled = 1;
					} else
					{
						settings_can.Enabled = 0;
					}
					if(Build_Int & 0x20000000ul)
					{
						settings_can.ListenOnly = 1;
					} else
					{
						settings_can.ListenOnly = 0;
					}
				} else
				{
					//if not using extended status mode then just default to enabling - this was old behavior
					settings_can.Enabled = 1;
				}
				//CAN0.set_baudrate(Build_Int);
				settings_can.NomSpeed = busSpeed;

			} else { //disable first canbus
				settings_can.Enabled = 0;
			}

			Settings_Set_CAN_Config(Settings_CAN_Handles[0], &settings_can);
			break;
		}
		case 4:
			Build_Int = byte;
			break;
		case 5:
			Build_Int |= byte << 8;
			break;
		case 6:
			Build_Int |= byte << 16;
			break;
		case 7:
		{
			Build_Int |= byte << 24;

			busSpeed = Build_Int & 0xFFFFF;		// Filter-out only 20 LSB (2^19 is not enough for 1MHz)
			if(busSpeed > 1000000) busSpeed = 1000000;		// Cap at 1MHz

			Settings_CANTypeDef settings_can;
			if(Build_Int > 0 && Settings_CAN_Handles[1] != NULL)
			{
				if(Build_Int & 0x80000000ul) //signals that enabled and listen only status are also being passed
				{
					if(Build_Int & 0x40000000ul)
					{
						settings_can.Enabled = 1;
					} else
					{
						settings_can.Enabled = 0;
					}
					if(Build_Int & 0x20000000ul)
					{
						settings_can.ListenOnly = 1;
					} else
					{
						settings_can.ListenOnly = 0;
					}
				} else
				{
					//if not using extended status mode then just default to enabling - this was old behavior
					settings_can.Enabled = 1;
				}
				//CAN1.set_baudrate(Build_Int);
				settings_can.NomSpeed = busSpeed;
			} else { //disable first canbus
				settings_can.Enabled = 0;
			}

			Settings_Set_CAN_Config(Settings_CAN_Handles[1], &settings_can);

			State = IDLE;
			//now, write out the new canbus settings to EEPROM
			//EEPROM.writeBytes(0, &settings, sizeof(settings));
			//EEPROM.commit();
			//setPromiscuousMode();
			break;
		}
		}
		Step++;
		break;
	case GET_CANBUS_PARAMS:
		// nothing to do
		break;
	case GET_DEVICE_INFO:
		// nothing to do
		break;
	case SET_SINGLEWIRE_MODE:
		if(byte == 0x10){
		} else {
		}
		//EEPROM.writeBytes(0, &settings, sizeof(settings));
		//EEPROM.commit();
		State = IDLE;
		break;
	case SET_SYSTYPE:
		// todo Ignore for now
		// settings.systemType = byte;
		//EEPROM.writeBytes(0, &settings, sizeof(settings));
		//EEPROM.commit();
		//loadSettings();
		State = IDLE;
		break;
	case ECHO_CAN_FRAME:
		Buff[1 + Step] = byte;
		switch(Step)
		{
		case 0:
			Build_Out_RxFrame.Header.StdId = byte;
			break;
		case 1:
			Build_Out_RxFrame.Header.StdId |= byte << 8;
			break;
		case 2:
			Build_Out_RxFrame.Header.StdId |= byte << 16;
			break;
		case 3:
			Build_Out_RxFrame.Header.StdId |= byte << 24;

			if(Build_Out_RxFrame.Header.StdId > 0x7FF) {			// if received id is greater than max std id
				Build_Out_RxFrame.Header.IDE = CAN_ID_EXT;		// Set extended frame id
				Build_Out_RxFrame.Header.ExtId = Build_Out_RxFrame.Header.StdId;
				Build_Out_RxFrame.Header.StdId = 0;
			} else Build_Out_RxFrame.Header.IDE = CAN_ID_STD;
			break;
		case 4:
			Out_Bus = byte & 1;
			break;
		case 5:
			Build_Out_RxFrame.Header.DLC = byte & 0xF;
			if(Build_Out_RxFrame.Header.DLC > 8) Build_Out_RxFrame.Header.DLC = 8;
			break;
		default:
			if(Step < Build_Out_RxFrame.Header.DLC + 6)
			{
				Build_Out_RxFrame.Data[Step - 6] = byte;
			}
			else
			{
				State = IDLE;

				CAN_DisplayFrame(&Build_Out_RxFrame, Out_Bus);
			}
			break;
		}
		Step++;
		break;
	case SETUP_EXT_BUSES: //setup enable/listenonly/speed for SWCAN, Enable/Speed for LIN1, LIN2
		switch(Step)
		{
		case 0:
			Build_Int = byte;
			break;
		case 1:
			Build_Int |= byte << 8;
			break;
		case 2:
			Build_Int |= byte << 16;
			break;
		case 3:
			Build_Int |= byte << 24;
			break;
		case 4:
			Build_Int = byte;
			break;
		case 5:
			Build_Int |= byte << 8;
			break;
		case 6:
			Build_Int |= byte << 16;
			break;
		case 7:
			Build_Int |= byte << 24;
			break;
		case 8:
			Build_Int = byte;
			break;
		case 9:
			Build_Int |= byte << 8;
			break;
		case 10:
			Build_Int |= byte << 16;
			break;
		case 11:
			Build_Int |= byte << 24;
			State = IDLE;
			//now, write out the new canbus settings to EEPROM
			//EEPROM.writeBytes(0, &settings, sizeof(settings));
			//EEPROM.commit();
			//setPromiscuousMode();
			break;
		}
		Step++;
		break;
	}
}

uint8_t GVRET_Checksum_Calc(uint8_t *buffer, int length) {
	uint8_t valu = 0;
	for (int c = 0; c < length; c++) {
		valu ^= buffer[c];
	}
	return valu;
}

