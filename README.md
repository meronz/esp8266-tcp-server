# esp8266-tcp-server
A really really simple TCP server with ESP8266.

A big thanks to cnlohr and his [post](http://hackaday.com/2015/03/18/how-to-directly-program-an-inexpensive-esp8266-wifi-module)!

## Installation
After installing the ESP8266 SDK, compile it with `bash gen_misc.sh` and load it with your favourite tool.
I use esptool.py with `esptool.py write_flash 0x00000 eagle.flash.bin 0x40000 eagle.irom0text.bin`.
This `gen_misc.sh` is preconfigured to be used with the Olimex board, which uses 2MB flash. Modify it to enable interactivity.

