#include "command.h"
#include "main.h"

char setClockCommand[] = {"set_clock"};
char setMinThresholdCommand[] = {"set_min_threshold"};
char setMaxThresholdCommand[] = {"set_max_threshold"};
char getClockCommand[] = {"get_clock"};
char getTemperatureCommand[] = {"get_temperature"};


char *commandList[] = {setClockCommand, setMinThresholdCommand, setMaxThresholdCommand, getClockCommand, getTemperatureCommand};

UART_HandleTypeDef *pHuart;
I2C_HandleTypeDef *pHi2c1;

int minThreshold = -99;
int maxThreshold = 99;

int testCommands = 0;

void setMinThreshold(int valToSet) {
	minThreshold = valToSet;
        eeprom_store_temperature(realTemperature, maxThreshold, minThreshold);
}

void setMaxThreshold(int valToSet) {
	maxThreshold = valToSet;
        eeprom_store_temperature(realTemperature, maxThreshold, minThreshold);
}

void executeCommand(short matched_id, int value) {
  int tempData[3] = {};
	switch (matched_id) {
		case 0:
                        rtc_set_time(value / 100, value %100, 0);
			break;
		case 1:
			setMinThreshold(value);
			break;
		case 2:
			setMaxThreshold(value);
			break;
		case 3:
			message_clock(pHuart);
			break;
		case 4:
                        eeprom_get_temperature(tempData);
			message_temperature(pHuart, tempData[0]);
			break;
	}
}

bool commandIdentify(char command[]) {
	short matched_id = -1;
	for (int id = 0; id < COMMAND_COUNT; id++) {
		testCommands++;
		for (int i = 0; i < strlen(command); i++) {
			if (command[i] == ' ' && i == strlen(commandList[id])) {matched_id = id; break;} //If we get to a whitespace, matched!
			if (command[i] != commandList[id][i]) {break;}
			}
	}
	if (matched_id == -1) {return false;}
	else {
		testCommands++;
		char * token = strtok(command, " ");
		token = strtok(NULL , " ");
		int value = atoi(token);
		executeCommand(matched_id, value);
		return true;
	}
}

void commanndSetChannels(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c1) {
	pHuart = huart;
	pHi2c1 = hi2c1;
}