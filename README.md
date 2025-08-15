# STM32 Internship Project

## Program Overview:
The purpose of the program is to be a clock and thermometer with an alert functionality where the program will post an alert if the temperature goes out of range of a cerain threshold. These thresholds can be set using a Serial Terminal. The clock can be configured either from the serial terminal or the physical button where a long press gets you to settings mode. In settings mode a single click increments the selected value by 1 and double click increments the selected value by 5. Long press switch between the hour and minute values and also exits the settings mode. The clock and temperature are displayed on a 4D7S LED with 3 second interval between them. There is also an I2C lcd which displays these values along with the alerts.

The program works with various interrupts providing various functionalities. The program doesn't have a main "while loop", instead the timer interrupts act triggers. This means the program is scalable and performance efficient.  
The program uses three different timers. TIM2 is the timer for the **clock**, TIM3 is the timer for the **display**, and TIM4 is the timer for the **button**. Because HAL calls the same callback function for all the timers, these timers all call *HAL_TIM_PeriodElapsedCallback(htim)* function. TIM2 calls every 1 second, TIM3 calls every 1 milisecond, TIM4 calls 300 miliseconds after activation. 
The program has three different main modes. **TEMP_MODE** is the mode that the program is in when displaying temperature. **CLOCK_MODE** is the mode that the program is in when displaying the clock. **SETTING_MODE** is the mode that the program is in when the user is changing the clock via the button.

**NOTE:** This project was for my internship in an embedded systems company. As someone that wasn't into embedded development, this project was kinda my learning tool. This is why the code is a bit experimental. Although the code for the specific functionalities like bpm180 and eeprom are understandble the main.c file is pretty messed up so I don't recommend trying to make sense of it.

### TIM2 Interrupt:
TIM2 interrupt is called every second. Every time TIM2 interrupt is called, *HAL_TIM_PeriodElapsedCallback(htim)* function calls the *secondPassed(htim)* function from "clock.c" to log that a second has passed. After that it also logs that a second has passed to the "main.c" function which uses it to switch between modes every 3 seconds. When the log in the "main.c" function reaches 3 seconds, the mode is changed and the program starts displaying the other mode.

### TIM3 Interrupt:
TIM3 interrupt is called every milisecond. Every time TIM3 interrupt is called, *HAL_TIM_PeriodElapsedCallback(htim)* function calls a display function from "4D7S.c" according to the mode it is in.

### TIM4 Interrupt
TIM4 interrupt is called 300 miliseconds after the user first clicks the button. When TIM4 function is called, the program evaluates how many times the user pressed the button in the *TIM4Elasped()* function in "main.c".  
Long press -> Switch Modes  
One Press -> Increment by 1  
Double Press -> Increment by 5

## 4 digit 7 segment LED: (4D7S_drive)
In order to use the 4 digit 7 segment LED the user needs to only call the function display_number(int number). This will display that number on the 7 segment LED.
The frequency for the LED is 250 Hz per digit. 
The user can use the set_mode(short modeToSet) function to set the mode of the display style.


## BPM 180 Temperature and Air Pressure Sensor: (BPM180)
(Only implemented the temperature sensor).
To use the BPM180 for temperature the user needs to call the function BPM_Read_True_Temperature(hi2c1).
This will return the temperature in 100 milicelcius. So you need to divide the return number by 10 in order to get the real degree celcius.

## NMEA and UART
To communicate with a computer or other devices, added UART functionality and NMEA protocol. NMEA protocol is usually used with CAN in marine environments. To use the protocol with UART I adapted some of the CAN reliant aspects to UART. The structs t_NMEA_Frame_Fast and t_NMEA_Segment_Fast act as datastructures which mimic the NMEA 2000's Fast Packet (Specifications at https://www.csselectronics.com/pages/nmea-2000-n2k-intro-tutorial#nmea-pgn). The construct_segment_from_string() constructs a NMEA Fast Segment from a given string. For now only 31 bytes can be sent like this however I'll increase it to the specifications limit of 223 bytes in the future. The send_single_segment_formatted() function sends the segment using UART to another computer formatted to be displayed in terminal. Normally, when sending it to a device which also has this protocol it should be sent without change however for this project since we're displaying it on a terminal I'm using the formatted version.

## RTC
Instead of a timer interrupt counting seconds and keeping track of time, RTC (real time clock) increments every second on its own with greater precision. It also keeps counting even if the program is not running as long as it has power. The RTC also does not reset when the program resets if implemented that way. It also counts while the program is off if supplied with a battery. The RTC does reset if the board ever loses power. Unfortunatelly STM32F746G-Disco board I'm using does not come with a VBAT pin to plug in a battery meaning when the power is off the RTC will reset. Using this library you can just have it run until the battery runs out by just plugging it in if you use a board with a VBAT pin, no need to change the code.

## EEPROM
Added functionality for an external eeprom that stores temperature and temperature threshold data. The data is stored in a 16 byte array where the first 6 bytes are the respective data stored in 2 bytes each (temperature, maximum threshold, minimum threshold). When storing the value is stored as a positive integer in the second byte. If the value is positive the first byte becomes 2, if its negative it becomes 0. To parse you need to do Second Byte * (First Byte - 1). The 7th byte is a corruption check (not a good one but still one) where it needs to equal 0xAB or the data is corrupted. Then, the data is encrypted with AES encryption using CBC mode. This means if any third-party acquires the eeprom they will need the key to decrypt the data. We use 32 bytes of the eeprom (which is a 24C02N with 256 byte storage). The first 16 bytes are the cipher text while the second 16 bytes is the IV. The IV is needed to decrypt and this way further implementations can randomize the IV when encrypting and store it in the eeprom so it can be decrypted with greater security. 
The AES code is from the following github repository: https://github.com/kokke/tiny-AES-c

**Note:** You need some 10 miliseconds of delay between the mem_write functions for the eeprom I'm using (24C02). You may also need it so keep it in mind. If you are going to you use it the safest way is with a delay function tied to a timer. 

# Commands
Added commands that can be written from a serial termninal. The code is splilt in two parts, the first part is in main.c in the HAL_UART_RxCpltCallback() function. This stores the inputs in a buffer until a \n new line is given. If so it calls the second part of the code in command.c where the job of that code is to identify which command was given. There are currently 5 commands:\
**set_clock dd:dd;** where dd:dd is the clock in hours:minutes that is to be set.\
**set_min_threshold n;** where n is the minimum threshold to be set where -256 < n < 256\
**set_max_threshold n;** where n is the maximum threshold to be set where -256 < n < 256\
**get_clock;** gets the current clock in the program.\
**get_temperature;** gets the current temperature in the program.\
**NOTE:** All commands need to be followed by a semicolon or the program won't be able to identify them.


The set_max_threshold and set_min_threshold commands directly writes to the eeprom to store these variables while get_temperature directly reads from the eeprom to get thetemperature.

# Images
![20250812_153758](https://github.com/user-attachments/assets/889d7415-df04-44ca-8c5d-46e137e3e9cf)
![20250815_141833](https://github.com/user-attachments/assets/3b057e8d-68ac-4529-94f9-a3c5f82cff96)
