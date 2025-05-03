FPM10A FINGERPRINT MODULE

This is a UART-based driver for FPM10A fingerprint module for AVR
microcontrollers. I've tested with ATmega328P.

Since ATmega328P has only one hardware UART, I'm using lynxzp's software UART
implementation [1] for debugging purposes. Thank you, lynxzp!

BUILDING AND UPLOADING

make -FPM_PWD=$(pasword) hex
make -FPM_PWD=$(pasword) upload

Default password: 0x00.

[1] https://github.com/lynxzp/Software_UART_for_AVR

