#ifndef NMEA_H
#define NMEA_H

#include <stdint.h>
#include "stm32f7xx_hal.h"

/**
 * Packet total: 10 bytes.
 * Real NMEA Fast packets are 12 bytes with CAN.
 */
typedef struct {
	uint8_t src_pgn; //1 Byte for source pgn
	uint8_t dest_pgn; //1 Byte for destination pgn
	//Byte 1 in data bytes is sequence num followed by frame num.
	//Byte 2 in data bytes is amount of bytes the payload has if frame num = 0
	uint8_t data_bytes[8];
} t_NMEA_Frame_Fast;

typedef struct {
	t_NMEA_Frame_Fast frames[4];
} t_NMEA_Segment_Fast;

void send_single_segment_formatted(UART_HandleTypeDef *huart, t_NMEA_Segment_Fast *NMEA_segment);
t_NMEA_Segment_Fast* construct_segment_from_string(char stringToConvert[27], uint8_t seq, uint8_t size);

#endif
