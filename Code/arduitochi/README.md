Arduitochi - 'tamagotchi' for Arduino

Requisites:
-An Arduino (Almost all are compatible, see below)

-Nokia 5110 screen or similar for Arduino and the Adafruit PCD8544 and Adafruit GFX libraries ( https://github.com/adafruit/Adafruit-PCD8544-Nokia-5110-LCD-library , https://github.com/adafruit/Adafruit-GFX-Library ) 

-Three buttons (Will be connected GND and pin, without resistances)

-(Optional) a buzzer/speaker

You have to have all the files of the proyect in a folder called "arduitochi", and then, open with the Arduino IDE open arduitochi.ino

In it there is default pin for it, but you can change it inside, in arduitochi.ino there is all.

Upload it, and play!

Our goal is to provide a tamagotchi-like program for arduinos and with cheap things. Arduinos can cost from 6€, and Nokia 5110 screen is about 5€ (ebay), buttons..almost free (10 cost 1€), and buzzers from old computers, alarm clocks..

See changelog.txt for changes in the program. (Be sure to see it if you are updating!)

Compatible Arduinos (Genuine and clones):
	Arduino Nano
	Arduino UNO
	Arduino MEGA 2560
	Arduino Leonardo
	
	All arduinos and his clones with ATmega328,ATmega328P,ATmega2560,Atmega32u4.
	ATmega128 not works because low flash memory (Only 16Kb)
	In theory, all Arduinos and clones that have 32Kb of flash, and 2Kb of SRAM works
	