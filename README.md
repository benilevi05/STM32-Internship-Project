# STM32 Internship Project

## Program Overview:
The program works with various interrupts providing various functionalities. The program doesn't have a main "while loop", instead the timer interrupts act triggers. This means the program is scalable and performance efficient.  
The program uses three different timers. TIM2 is the timer for the **clock**, TIM3 is the timer for the **display**, and TIM4 is the timer for the **button**. Because HAL calls the same callback function for all the timers, these timers all call *HAL_TIM_PeriodElapsedCallback(htim)* function. TIM2 calls every 1 second, TIM3 calls every 1 milisecond, TIM4 calls 300 miliseconds after activation. 
The program has three different main modes. **TEMP_MODE** is the mode that the program is in when displaying temperature. **CLOCK_MODE** is the mode that the program is in when displaying the clock. **SETTING_MODE** is the mode that the program is in when the user is changing the clock via the button.

### TIM2 Interrupt:
TIM2 interrupt is called every second. Every time TIM2 interrupt is called, *HAL_TIM_PeriodElapsedCallback(htim)* function calls the *secondPassed(htim)* function from "clock.c" to log that a second has passed. After that it also logs that a second has passed to the "main.c" function which uses it to switch between modes every 3 seconds. When the log in the "main.c" function reaches 3 seconds, the mode is changed and the program starts displaying the other mode.

### TIM3 Interrupt:
TIM3 interrupt is called every milisecond. Every time TIM3 interrupt is called, *HAL_TIM_PeriodElapsedCallback(htim)* function calls a display function from "4D7S.c" according to the mode it is in.

### TIM4 Interrupt
TIM4 interrupt is called 300 miliseconds after the user first clicks the button. When TIM4 function is called, the program evaluates how many times the user pressed the button in the *TIM4Elasped()* function in "main.c".  
Long press -> Switch Modes  
One Press -> Increment by 1  
Double Press -> Increment by 5

## 4 digit 7 segment LED: (7D4S_drive)
In order to use the 4 digit 7 segment LED the user needs to only call the function display_number(int number). This will display that number on the 7 segment LED.
The frequency for the LED is 250 Hz per digit. 
The user can use the set_mode(short modeToSet) function to set the mode of the display style.


## BPM 180 Temperature and Air Pressure Sensor: (BPM180)
(Only implemented the temperature sensor).
To use the BPM180 for temperature the user needs to call the function BPM_Read_True_Temperature(hi2c1).
This will return the temperature in 100 milicelcius. So you need to divide the return number by 10 in order to get the real degree celcius.

## NMEA and UART
To communicate with a computer or other devices, added UART functionality and NMEA protocol. NMEA protocol is usually used with CAN in marine environments. To use the protocol with UART I adapted some of the CAN reliant aspects to UART. The structs t_NMEA_Frame_Fast and t_NMEA_Segment_Fast act as datastructures which mimic the NMEA 2000's Fast Packet (Specifications at https://www.csselectronics.com/pages/nmea-2000-n2k-intro-tutorial#nmea-pgn). The construct_segment_from_string() constructs a NMEA Fast Segment from a given string. For now only 31 bytes can be sent like this however I'll increase it to the specifications limit of 223 bytes in the future. The send_single_segment_formatted() function sends the segment using UART to another computer formatted to be displayed in terminal. Normally, when sending it to a device which also has this protocol it should be sent without change however for this project since we're displaying it on a terminal I'm using the formatted version.
