#include "NMEA.h"
#include <stdlib.h>

uint8_t payloadRemaining = 0;
uint8_t stm32_pgn = 15;
uint8_t pc_pgn = 2;

/**
 * Actual NMEA protocol for fast frames (without CAN since we are using UART)
 */
void send_single_frame(UART_HandleTypeDef *huart, t_NMEA_Frame_Fast NMEA_frame) {
	int i = 1;
	uint8_t src_pgn = NMEA_frame.src_pgn;
	uint8_t dest_gpn = NMEA_frame.dest_pgn;
	uint8_t sequence_num = NMEA_frame.data_bytes[0] >> 4;
	uint8_t frame_num = NMEA_frame.data_bytes[0] && (0x0F);
	if (sequence_num == 0) {payloadRemaining = NMEA_frame.data_bytes[1]; i++;}
	if (payloadRemaining >= 8) {
		HAL_UART_Transmit(huart, NMEA_frame.data_bytes, 8, 100);
	} else {
		HAL_UART_Transmit(huart, NMEA_frame.data_bytes, payloadRemaining, 100); //idk if this works
	}
}

/**'
 * Only sends what Terra Term can read. NMEA is more of a representation.
 */
void send_single_frame_formatted(UART_HandleTypeDef *huart, t_NMEA_Frame_Fast *NMEA_frame) {
	int i = 1;
	uint8_t data_byte[1];
	uint8_t src_pgn = NMEA_frame->src_pgn;
	uint8_t dest_gpn = NMEA_frame->dest_pgn;
	uint8_t sequence_num = (NMEA_frame->data_bytes[0] >> 4); //Byte 1 left letter
	uint8_t frame_num = (NMEA_frame->data_bytes[0] && (0x0F)); //Byte 1 right letter
	if (frame_num == 0) {payloadRemaining = NMEA_frame->data_bytes[1]; i++;}
	while (i < 8 && payloadRemaining > 0) {
		data_byte[0] = NMEA_frame->data_bytes[i];
		HAL_UART_Transmit(huart, data_byte, 1, 100);
		i++;
		payloadRemaining -= 1;
	}
}

void send_single_segment_formatted(UART_HandleTypeDef *huart, t_NMEA_Segment_Fast *NMEA_segment) {
	for (int frameNum = 0; frameNum < 4; frameNum++) {
		send_single_frame_formatted(huart, &NMEA_segment->frames[frameNum]);
	}
	free(NMEA_segment);
}

t_NMEA_Segment_Fast* construct_segment_from_string(char stringToConvert[27], uint8_t seq, uint8_t size) {
	uint8_t count = 0;
	t_NMEA_Segment_Fast* segment = malloc(sizeof(t_NMEA_Segment_Fast));
	for (int frameNum = 0; frameNum < 4;frameNum++) {
		t_NMEA_Frame_Fast frame;
		uint8_t byte1 = (seq << 4) | (frameNum & 0x0F);
		frame.src_pgn = stm32_pgn;
		frame.dest_pgn = pc_pgn;
		frame.data_bytes[0] = byte1;
		for (int i = 1; i < 8; i++) {
			if (i == 1 && frameNum == 0) {frame.data_bytes[1] = size; continue;}
			if (count < size) {
				frame.data_bytes[i] = stringToConvert[count];
				count++;
			} else {
				frame.data_bytes[i] = 0xFF;
			}
		}
		segment->frames[frameNum] = frame;
	}
	return segment;
}
