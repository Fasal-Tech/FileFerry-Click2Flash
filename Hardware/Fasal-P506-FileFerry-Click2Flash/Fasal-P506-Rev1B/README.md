# FileFerry-Click2Flash - Hardware Files #

This directory contains all the Hardware related files for the main "FileFerry-Click2Flash" Board.

# Repository Contents #
* **/Design_Files**
	* CubeMX Files
	* Change log
	* Design Report
	* Schematic pdf
	* Schematic Source (.sch)
	* Board Source (.brd)
* **/Manufacturing_Files**
	* PCBA BOM (Production Bill of Materials)
	* PCB Assembly Instructions
	* PCB Fabrication Instructions with stackup
	* Gerber Files (.zip)
	* PCB Images and 3D STEP file
	* Pick and Place (PnP_XY) files
* **/Proto_Manufacturing**
	* Use this to get a complete PCBA build from JLCPCB.
	* Gerber Files (same as production gerber)
	* JLCPCB SMT BOM (BOM for LCSC / JLCPCB PCBA)
	* JLCPCT CPL (compoennt placement file. SMD and THT both.)
* **/Testing_Files**
	* Board bring-up Arduino Test Code
	* Board Bring-up test case and report

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
![IDE 1](Images/IDE_1.png)
![IDE 2](Images/IDE_2.png)
