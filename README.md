# SrDuino
SrDuino is an open-source project that has adapted several games created by other users to work on a simple and inexpensive platform to facilitate the dissemination and development of new games and software.
The SrDuino V0.1 is made using a UNO arduino, a Nokia 5110 5Volts screen, 2 potentiometers and 4 push buttons, but this can evolve according to the need of the new codes.

Some codes are commented in portuguese from brazil.
If you develop new ones, you can post here.


The pins used for the screen are:
#define PIN_SCE   11

#define PIN_RESET 12

#define PIN_DC    10

#define PIN_SDIN  9

#define PIN_SCLK  8

The 5Volts of the potentiometer is connected to pin 2, enabling the activation of the potentiometer only when necessary

The buzzer is on port 3(In this assembly I did not use it, but it is available on the diagrma)

The push buttons are in the pins: 
#define LEFT 7
#define DOWN 5
#define RIGHT 4
#define UP 6

I am supplying power through the Usb port but you can use a battery
<div class="imagemMateria imgC">
    <img width="" height="" title="SrDuino" alt="SrDuino"
         src="https://github.com/gsrbr/SrDuino/blob/master/Code/SrDuino.png"/>
    <p>SrDuino</p> []
</div>
