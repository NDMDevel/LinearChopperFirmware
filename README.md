# Linear Chopper Firmware

>Source code developed in the context of [SWTOMP](http://ciemat.swtomp.es) project

## Introduction

This repository contains the firmware (source code) of the two microcontrollers placed in the board hosted in the following public and open hardware repository: [LinearChopper](https://cadlab.io/project/24950/master/files).  

## Description

There are two software projects, one for the microcontroller PIC16F1705 which implements the overvoltage protection, and the other for the ESP8266 that provides WiFi connectivity.  
The project for the PIC was developed in the MPLABX IDE, using the MCC as base hardware driver.  
The ESP8266 project uses the Arduino framework.  

## Firmware Implementation details

The firmware has been tested in the field with satisfactory results, but the source code organization is still in beta state.  
  
TODO:
- Remove hard coded constants
- Implement more middle layers (HALs[^1]) between the top level application (Overvoltage protection) and the hardware. (The current implementation access directly to MCU's registers.)
- Improve the communication protocol between the 16F1705 and the ESP8266 (at least implement some kind of standar way to introduce a new field in the webpage and the corresponding communication command).

## Resource utilization

Currently, the 16F1705 is almost all the time busy doing the OVP[^2] control. If more features are required, a more powerfull MCU will be needed.  

[^1]: Hardware Abstraction Layers
[^2]: Over Voltage Protection
