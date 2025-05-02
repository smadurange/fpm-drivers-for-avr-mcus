ZFM20 FINGERPRINT MODULE

This is a UART-based driver for ZFM20 family of fingerprint modules for AVR
microcontrollers. I wrote this for and tested with ATmega328P. It should work
with other AVR microcontrollers as well.

Since ATmega328P has only one hardware UART, I'm using lynxzp's software UART
implementation [1] for debugging purposes.

[1] https://github.com/lynxzp/Software_UART_for_AVR
