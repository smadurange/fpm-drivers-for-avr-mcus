FPM DRIVERS

This project contains drivers for the following fingerprint sensor modules
FPM10A for AVR microcontrollers:

 - R503
 - FPM10A

I've only tested these with ATmega328P MCU. Since ATmega328P has only one UART,
I'm using lynxzp's software UART implementation [1] for debugging. Thank you,
lynxzp!

BUILDING AND UPLOADING

make -FPM_PWD=$(pasword) hex
make -FPM_PWD=$(pasword) upload

Default password: 0x00.

[1] https://github.com/lynxzp/Software_UART_for_AVR

