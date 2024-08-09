# 🚀 FileFerry-Click2Flash

A compact and portable tool designed for easy factory programming of your IoT product. 🌐 Transfer files, binary images, security credentials, or certificates from an SD card or from a Serial terminal over USB to a target SPI flash IC with just a click of a button. 🎯

This repository contains all the necessary files to manufacture your own samples from JLCPCB (or a vendor of your choice) and start using it right away

The included firmware source code and compiled binaries can be used right out of the box! 🛠️ You can also modify the design and code and build your own custom product. ✨

## Product Image

![FileFerry-Click2Flash PCB](/Assets/images/PCBA_1.jpg)
The "FileFerry-Click2Flash" PCB.

![FileFerry-Click2Flash PCB with pogo pins](/Assets/images/PCBA_2.jpg)
The "FileFerry-Click2Flash" PCB with the pogo pins.

## 📁 Repository Contents

* **/Hardware 🛠️**
  * All Eagle design files (.brd, .sch)
* **/Firmware 💻**
  * Pre-compiled binaries to flash the board
  * Firmware source code (STM32Cube IDE, HAL) that you can use to modify the functionality
* **/Manufacturing 🏭**
  * All manufacturing files, including Gerber, BOM, PnP_XY, and prototype manufacturing files (Order BOM and PnP) that can be used to order directly from JLCPCB for a complete PCBA job.
* **/Assets 📸**
  * GitHub assets like images and other resources
* **/Demo 📸**
  * Demonstration resources to compile the firmware, flash it to the "FileFerry-Click2Flash" board and use it.

## 🔍 What is FileFerry-Click2Flash?

In its simplest form, FileFerry-Click2Flash is a low-cost, portable hardware jig that allows you to flash any external SPI flash IC (like the W25Q64JVSSIQ). 🛠️

You can transfer data in one of two ways:

1. Store your file/data in the onboard micro SD card. 💾
    1. Example: Production firmware images, common security tokens, certificates, etc.
2. Transfer the files from an external computer over UART using the X-modem protocol. 🔄
    1. Example: Device-specific unique keys, device-specific CA certificates, UUID tokens, etc.

You can do all of this with just a click of the onboard button! 🎯

## 🚀 Why is "FileFerry-Click2Flash" Needed? 🛠️

In today's fast-paced manufacturing environment, the need for efficient, reliable, and cost-effective tools is paramount. Traditional methods of programming SPI flash ICs, such as using J-link SPI programmers or bed-of-nails jigs, often involve expensive, bulky, and complex setups that are not easily adaptable to different production scenarios. 😓

This is where FileFerry-Click2Flash comes in! 🎉 It addresses the gap between high-cost, inflexible tools and the need for a versatile, portable, and user-friendly solution. Whether you are flashing firmware or transferring security credentials, or unique device identifiers, FileFerry-Click2Flash offers an ultra-low-cost alternative that is not only easy to use but also highly customizable. The ability to transfer files via an onboard SD card or over UART using the X-modem protocol provides flexibility in various production environments, making it a robust and indispensable tool for manufacturers, developers, and technicians alike. ⚙️💻

With its customizable connector interface and portable design, this tool simplifies the flashing process, reducing both the time and costs associated with traditional methods. FileFerry-Click2Flash is the modern solution to the challenges faced in programming SPI flash ICs, ensuring that the process is as seamless and efficient as possible. 🖊️💡

## 💰 Manufacturing Cost Breakdown

We got 5 quantity PCBA samples from JLCPCB. (We are in no way affiliated with JLCPCB—it's just convenient to manufacture quick prototypes with them!) 🚀

* Total cost with Shipping (PCB + PCBA): $132.49 📦
* Landing cost with import duties, customs, etc.: INR 14,000.00 (for 5 pieces) 💸
* Cost per board: INR 2,800.00 (includes 6 extra pogo pins, hand soldered later) 🛠️
* At a cost of INR 2,800.00 (~ USD 33.00), we have a SPI flasher with additional functionalities like UART transfer.
  
🔄 Compare this with a J-Flash SPI programmer at ~ USD 330.00 or a Bed of nails test jig at ~ USD 500.00.
🤔 Which would you choose?

## High Level Working

Let's look at how this works.

![High level block diagram](/Assets/images/block_diagram.png)
The "FileFerry-Click2Flash" system block diagram.

![PCB Description](/Assets/images/pcb_description.png)
The "FileFerry-Click2Flash" PCB points

### There are two ways (currently supported) to transfer files to the target SPI Flash

### 1. Using the onboard micro SD card (Files need to be on the SD card)

  1. Simple copy the file you want to transfer to the external SPI Flash into the SD card (Example a production binary)
  2. Modify the given source code to use the file name and build the code
  3. Flash the code to the "FileFerry-Click2Flash" board
  4. Insert the SD Card, power up the board and click the "Flash" button for the flashing to take place
  5. you can monitor the progress on the RGB LED as well as debug logs on the USB-UART

### 2. Using a PC client over USB (you will need a Software client to initiate transfer)

  1. No re-programming needed.
  2. Slide the "Mode Switch" S302 to "Mode 1". This will activate UART X-Modem transfer. (Current firmware supports X-modem. Can be modified to any thing else).
  3. Open a terminal program of your choice. The terminal program has to support "File Transfer" functionality.
  4. We use TeraTerm. So open TeraTerm.
  5. Connect to the proper COM port. Use BAud as 115200,8N1.
  ![Teraterm Window](/Assets/images/teraterm_1_port.png)
  Teraterm Open

  6. Go to File-> Transfer -> XMODEM -> Send -> [Check Option 1K] + Select file.
  ![Teraterm file selection](/Assets/images/teraterm_2_xmodem.png)
  Transfer setting

  7. Select your file and click "Open".
  ![Teraterm file selection](/Assets/images/teraterm_3_file-select.png)
  File Selection

  8. The file transfer will start.
  ![Teraterm file selection](/Assets/images/teraterm_4_transfer.png)
  File Transfer


## What type of Files can I transfer?
The sky is the limit really. Some common examples are:

1. Transferring the production "Golden Image" of the product where the end product's bootloader performs a DFU from the binaries available in the Flash IC. (You can extend this for implementing OTA).
2. Transferring unique device CA certificates from a PC based on the device serial number. (Scan barcode, generate unique CA certificate in the PC application, transfer it to the device over USB UART).
3. Transferring audio files, JSON configuration files, etc.


## Can I modify the hardware?

You have the design files with you. You can do anything! If you are a student or a DIY'er, here are some ideas to use the hardware:

1. Use as a STM32F103 development kit
2. Use it as a data-logger. Examples:
   * Connect an I2C sensor (there is a QWIIC port for easy connection. There is also a JST port with controlled power) and modify the STM32 code to read from the sensor and store it in the onboard SD card.
   * You can connect a SPI sensor as well.
3. Use the additional UART2 port to connect a GPS receiver and modify the STM32 code to receive the GPS NMEA strings, parse it and store it in the onboard SD card.
4. RFID access control anyone? Interface a RFID reader over UART/I2C/SPI (RC522 or PN532 or similar), have a master list of RFID numbers stored in the SD card. Connect a relay to a free GPIO and give access control to the room.

Limitless possibilities!


## 📜 License Information
The hardware and firmware are released under the MIT License. 📝 The code is beerware; if you see me (or any other Fasal employee) at the local, and you've found our code helpful, please buy us a round! 🍻

Distributed as-is; no warranty is given. ⚠️ Read the License file for complete licensing information.
