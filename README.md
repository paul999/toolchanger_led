E3D toolchanger LEDs based on tool temperatur
=============================================

This code shows for each tool on a toolchanger 3 LEDs based on the temperature for that tool.
* For temperature under 30C the LEDs are green.
* For temperature between 30C and 75C the LEDs are orange.
* For temperature above 75C the LEDs are red.
LEDs between the tools are always shown as white.

Please note that I won't be providing support on this code. If you want to use it, you will need to figure out yourself how to get it to work.

## Requirements
The code is written for a standard ESP32 dev board.

The LED strip is connected to D21 (If you use a different pin, you will need to update the code)

The Vin and GND pins are connected via a stepdown converter to the power supply for the toolchanger.

Do not forget to update your network settings before running it, and also update the IP of the toolchanger.
