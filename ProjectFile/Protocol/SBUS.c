/*
*This file SBUS.c is used to handle received SBUS data stored in "buffer"
*SBUS is a complex protocol,search SBUS on web for more information
*
*This file require one frame data of SBUS,which is of total 25 bytes,
*includeing startByte and endByte are stored in "Buffer".
*Then store 16 Channels of data in the pointer
*
*How the Buffer work in detail is not the concern of this file.
*User should implement these detials in the header file "SBUS.h".
*to do:
*define bufferReadByte(x)
*define bufferPara
*include headerfile for buffer if needed

*for example: if Buffer is a uint8_t array,
*define bufferReadByte(x) buffer[x]
*define bufferPara uint8_t* buffer 
*/

#define SBUS_c
#include "SBUS.h"

#define BIT_MASK 0x7ffU // mask for 11 bits 
#define BITS_OF_BYTE (uint8_t)8		// a byte is 8 bit
#define BITS_OF_CHANNEL (uint8_t)11 // each channel contains 11 bits

#define BYTES_OF_FRAME (uint8_t)SBUS_FRAME_BYTE_LENGTH	   // number of bytes a frame contains
#define FRAME_TOTAL_CH (uint8_t)SBUS_TOTAL_CH	   // total channels of a frame
#define FRAME_START_BYTE (uint8_t)0x0fU // first byte in frame in hex
#define FRAME_END_BYTE (uint8_t)0x00U   // last byte in frame in hex

#define STATE_BYTE_INDEX (uint8_t)22 //the state byte is the (22 + 1) byte
#define FRAME_LOST_BIT_MASK 0x4U //second bit of state byte
#define FAIL_SAFE_BIT_MASK 0x8U	//third bit of state byte


inline bool SBUS_ValidFrame(bufferPara)
{
	if (bufferReadByte(0) == FRAME_START_BYTE && bufferReadByte(BYTES_OF_FRAME - 1) == FRAME_END_BYTE)
	{
		return true;
	}
	else
	{
		return false;
	}
}

inline void SBUS_Process(bufferPara, uint16_t *CH)
{
	uint8_t rightPartInBits = 0; // number of bits already computed in current byte,initially is 0
	uint8_t CH_Index;
	uint8_t bufferIndex = 1; // index 0 is the start frame

	uint8_t stateByte = bufferReadByte(STATE_BYTE_INDEX);
	CH[FRAME_LOST_CH_INDEX] = (stateByte & FRAME_LOST_BIT_MASK) == 0x1U ? 0xffU : 0x0U; 
	CH[FAIL_SAFE_CH_INDEX] = (stateByte & FAIL_SAFE_BIT_MASK) == 0x1U ? 0xffU : 0x0U; 

	for (CH_Index = 0; CH_Index < FRAME_TOTAL_CH; CH_Index++)
	{
		uint8_t leftPartInBits;

		leftPartInBits = BITS_OF_BYTE - rightPartInBits;
		rightPartInBits = BITS_OF_CHANNEL - leftPartInBits;

		uint16_t LeftPart;
		uint16_t RightPart;

		if (rightPartInBits > BITS_OF_BYTE)
		{
			uint16_t MiddlePart;
			rightPartInBits -= BITS_OF_BYTE;

			LeftPart = (uint16_t)bufferReadByte(bufferIndex) >> (BITS_OF_BYTE - leftPartInBits);
			bufferIndex++;
			MiddlePart = (uint16_t)bufferReadByte(bufferIndex) << (leftPartInBits);
			bufferIndex++;
			RightPart = (uint16_t)bufferReadByte(bufferIndex) << (leftPartInBits + BITS_OF_BYTE);

			CH[CH_Index] = (LeftPart | MiddlePart | RightPart) & BIT_MASK;
		}
		else
		{
			LeftPart = (uint16_t)bufferReadByte(bufferIndex) >> (BITS_OF_BYTE - leftPartInBits);
			bufferIndex++;
			RightPart = (uint16_t)bufferReadByte(bufferIndex) << leftPartInBits;

			CH[CH_Index] = (LeftPart | RightPart) & BIT_MASK;
		}
	}
	return;
}
