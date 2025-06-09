# xbox-kill-switch

This Arduino sketch implements a kill switch for disabling the Xbox360. 
It can be used as part of a parental control strategy for disabling 
the xbox during unsuitable times (e.g. 06:00am before school!).

For this project I've used a ESP32-C3 miniboard.  
https://forum.arduino.cc/t/esp32-c3-supermini-pinout/1189850

You could in theory use a ESP-01S however you need two pins that
are completely under your control, and the ESP-01S has various issues
with many of the pins that could give problems, e.g. they must be at
certain levels during boot, or they spew momentary noise before you
can take control of them.  Also the ESP-01S requires an external 
5v->3.3v regulator, and this is included on the C3 miniboard.

The software connects to my wifi and queries a UDP server to see 
whether it should be enabled or disabled.  To control the kill
switch manually you can use a combination of:

This web interface:
https://github.com/bifferos/switchboard-web-ui

And this UDP server:
https://github.com/bifferos/switchboard-udp

But so long as you are OK with creating/deleting a file to 
enable/disable the xbox then you can just use the UDP server.
