# STM32 Internship Project

4 digit 7 segment LED: (7D4S_drive)
In order to use the 4 digit 7 segment LED the user needs to only call the function display_number(int number). This will display that number on the 7 segment LED.
The frequency for the LED is 250 Hz per digit. 
The user can use the set_mode(short modeToSet) function to set the mode of the display style.

0 -> TEMP_MODE: This is temperature mode. Digit 2-3 is used for displaying a number while Digit 1 is (-) in case of negative numbers and Digit 4 is used for the symbol 'C'. Supports numbers -99 to 99.
1 -> COUNT_MODE: This is count mode. All digits represent the digits of the number that will be displayed. Supports numbers 0 to 9999.


BPM 180 Temperature and Air Pressure Sensor:
(Only implemented the temperature sensor).
To use the BPM180 for temperature the user needs to call the function BPM_Read_True_Temperature(hi2c1).
This will return the temperature in 100 milicelcius. So you need to divide the return number by 10 in order to get the real degree celcius.