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
/**
* Executes a given command. Also takes into consideration the value for the set commands. These are 0 for get commands and don't get used.
* @param matched_id The id of the command matched.
* @param value The value that was passed for the set command. 0 for get commands and not used.
*/
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

/**
* Identifies a given command and calls the execution of the command.
* @param command[] The command as a string (char array).
* @return if the command was able to be identified. 
 */
bool commandIdentify(char command[]) {
      short matched_id = -1;
      for (int id = 0; id < COMMAND_COUNT; id++) {
              testCommands++;
              for (int i = 0; i < strlen(command); i++) {
                      if (command[i] == ' ' && i == strlen(commandList[id]) && id <= 2) {matched_id = id; break;} //If we get to a whitespace, matched! (Set commands,)
                      if (command[i] == ';' && i == strlen(commandList[id]) && id >= 3) {matched_id = id; break;} //Get commands.
                      if (command[i] != commandList[id][i]) {break;}
                      }
      }
      if (matched_id == -1) {return false;} //Not identified
      if (matched_id >= 3) {executeCommand(matched_id, 0); return true;} //Get command
      char * token = strtok(command, " ");
      token = strtok(NULL , " ");
      if (token[strlen(token) - 1] == ';' ) {
        token = strtok(token , ";");
        int value = atoi(token);
        executeCommand(matched_id, value);
        return true;
      } else {
        return false;
      }        
}

void commanndSetChannels(UART_HandleTypeDef *huart, I2C_HandleTypeDef *hi2c1) {
	pHuart = huart;
	pHi2c1 = hi2c1;
}