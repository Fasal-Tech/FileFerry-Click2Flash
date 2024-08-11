# FileFerry-Click2Flash - Testing Files #

This directory will contains the board bring-up report as well as the Arduino testing code

# Test Code #
Software: Arduino IDE 2.3.0
STM32 Core: Official STM32 Arduino Core

# Repository Contents #
* The board Bring-up Code contains the arduino code as well as compined binaries
* The Board Bring-up report is a word document having the test conditions of the bring-up

## Why Arduino? ##
* This code is written by a Electronics Engineer.
* The production code is in the official STM32Cube IDE
* Arduino is quick to tinker, for the sake of board bring-up. It is easy to use and this electronics engineer believes that it is more than sufficient to test a new PCBA for bring-up activities.

## Wroking ##
The Arduino code (Sketch) is simple. It does the following:
* Prints a welcome message
* Blinks the RGB LED through various colors (7 color combination)
* Increments a counter
* Interrupt function that prints a "Button pressed" messade everytime the user button is pressed

## Screenshots ##
![IDE 1](/Images/IDE1.png)
![IDE 2](/Images/IDE2.png)