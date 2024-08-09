FileFerry-Click2Flash
========================================
A handheld PCB designed for easy factory programming of your IoT product. Transfer files, binary images, security credentials, or certificates from an SD card to a target SPI flash IC or UART system at the click of a button. This repo contains all necessary files to manufacture your own samples from JLCPCB (or a vendor of your choice) and use this off the bat. You can also modify the design and build your own product.


Product Image
-------------------
![FileFerry-Click2Flash PCB](/Assets/images/PCBA_1.jpg)
The "FileFerry-Click2Flash" PCB.

![FileFerry-Click2Flash PCB with pogo pins](/Assets/images/PCBA_2.jpg)
The "FileFerry-Click2Flash" PCB with the pogo pins.

Repository Contents
-------------------
* **/Hardware** - All Eagle design files (.brd, .sch)
* **/Firmware** - Firmware that comes pre-installed on the FileFerry
* **/Manufacturing** - All manufacturing files including Gerber, BOM, PnP_XY, and prototype manufacturing files (Order BOM and PnP) that can be used to order directly from JLCPCB for a complete PCBA job.
* **/Assets** - Github assets like images and what-not.

What is "FileFerry-Click2Flash"?
--------------------------------
In it's simplest form, the "FileFerry-Click2Flash" is a low cost, portable, hardware jig that you can use to flash any external SPI flash IC (like the W25Q64JVSSIQ).
You can transfer data in one of two ways:
a. Store your file/data in the onboard micro SD card. (Example: Production firmware images, common security tokens / certificates, etc.)
b. Transfer the files from an external computer over UART using the X-modem protocol. (Example: Device specific unique keys, device specific CA certificates, UUID tokens, etc.)
You can do all of this at a click of the onboard button.

🚀 Why is "FileFerry-Click2Flash" Needed? 🛠️
---------------------------------------------
In today's fast-paced manufacturing environment, the need for efficient, reliable, and cost-effective tools is paramount. Traditional methods of programming SPI flash ICs, such as using J-link SPI programmers or bed-of-nails jigs, often involve expensive, bulky, and complex setups that are not easily adaptable to different production scenarios. 😓

This is where FileFerry-Click2Flash comes in! 🎉 It addresses the gap between high-cost, inflexible tools and the need for a versatile, portable, and user-friendly solution. Whether you are flashing firmware or transferring security credentials, or unique device identifiers, FileFerry-Click2Flash offers an ultra-low-cost alternative that is not only easy to use but also highly customizable. The ability to transfer files via an onboard SD card or over UART using the X-modem protocol provides flexibility in various production environments, making it a robust and indispensable tool for manufacturers, developers, and technicians alike. ⚙️💻

With its customizable connector interface and portable design, this tool simplifies the flashing process, reducing both the time and costs associated with traditional methods. FileFerry-Click2Flash is the modern solution to the challenges faced in programming SPI flash ICs, ensuring that the process is as seamless and efficient as possible. 🖊️💡

High Level Working
------------------
Let's look at how this works.

![High level block diagram](/Assets/images/block_diagram.png)
The "FileFerry-Click2Flash" system block diagram.

![PCB Description](/Assets/images/pcb_description.png)
The "FileFerry-Click2Flash" PCB points

There are two ways (currently supported) to transfer files to the target SPI Flash
# 1. Using the onboard micro SD card (Files need to be on the SD card)
a. Simple copy the file you want to transfer to the external SPI Flash into the SD card (Example a production binary)
b. Modify the given source code to use the file name and build the code
c. Flash the code to the "FileFerry-Click2Flash" board
d. Insert the SD Card, power up the board and click the "Flash" button for the flashing to take place
e. you can monitor the progress on the RGB LED as well as debug logs on the USB-UART

# 2. Using a PC client over USB (you will need a Software client to initiate transfer)
a. No re-programming needed.
b. Slide the "Mode Switch" S302 to "Mode 1". This will activate UART X-Modem transfer. (Current firmware supports X-modem. Can be modified to any thing else).
c. Open a terminal program of your choice. The termainal program has to support "File Transfer" functionality.
d. We use TeraTerm. So open TeraTerm.
e. Connect to the proper COM port. Use BAud as 115200,8N1.

![Teraterm Window](/Assets/images/teraterm_1_port.png)
Teraterm

f. Go to File-> Transfer -> XMODEM -> Send -> [Check Option 1K] + Select file.

![Teraterm file selection](/Assets/images/teraterm_2_xmodem.png)
File Selection

g. Select your file and click "Open".

![Teraterm file selection](/Assets/images/teraterm_3_file-select.png)
File Selection

h. The file transfer will start.

![Teraterm file selection](/Assets/images/teraterm_4_transfer.png)
File Transfer

License Information
-------------------
The hardware and firmware are released under [MIT License]([https://creativecommons.org/licenses/by-sa/4.0/](https://opensource.org/license/mit)).
The code is beerware; if you see me (or any other Fasal employee) at the local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
Read the License file for complete licensing information.
