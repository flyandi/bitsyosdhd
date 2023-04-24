BitsyOSD HD
=======

*** WORK IN PROGRESS **

BitsyOSD goes HD. BitsyOSD is a feature-rich standalong OSD firmware specficially designed for aerial vehicles that don't really need a flight controller such as fixed wings but still want digital OSD, ARM state and other features on their Caddx Vista and DJI Air units (O1 and O3). 

Inspired by https://github.com/ramiss/arduino_DJI_03_RC_ARM it uses the serial rich SAMD21 platform.

## Features

- O3 ARM State so you can get the full power
- VBAT Display - YES
- PAN Servo Support
- More to come


## Get Started

1. Install the libs used for this one via the Arduino IDE:

- ReefwingMSP
- ReefwingSBUS

2. Wiring - I use the [XIAO SAMD21](https://www.seeedstudio.com/Seeeduino-XIAO-Arduino-Microcontroller-SAMD21-Cortex-M0+-p-4426.html) board and the wiring is for that specific chip:

5V to Pan Servo and RX 5V  (that's where the board will be powered from)
GND to Pan Servo and RX GND
A9 RX SBUS Signal
A8 Servo Output
A7 TX to O3
A6 RX to O3

3. Modify the XF Output Map to have CH1 as SBUS - that's it. It uses CH6 for the PAN and CH7 for ARM. Setup your remote mixing like that or change in the code.

4. Power on and test it out..

---

 This is a prototype and use is on your own terms. Good luck!


