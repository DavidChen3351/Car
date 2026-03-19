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

#define BYTES_OF_FRAME (uint8_t)SBUS_FRAME_LENGTH	   // number of bytes a frame contains
#define FRAME_TOTAL_CH (uint8_t)16	   // total channels of a frame
#define FRAME_START_BYTE (uint8_t)0x0fU // first byte in frame in hex
#define FRAME_END_BYTE (uint8_t)0x00U   // last byte in frame in hex

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
	uint8_t CH_Index = 0;
	uint8_t bufferIndex = 1; // index 0 is the start frame
	for (uint8_t i = 0; i < FRAME_TOTAL_CH; i++)
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
		CH_Index++;
	}
	return;
}
