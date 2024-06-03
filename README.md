# Max7219_LightPen
light pen system for eventually drawing on max7219 led arrays.

This code is not 'arduino' but written raw for the atmega328 itself. 
I use gcc and avrdude via the makefile, which can utilize the arduino bootloader.
Arduino-ize it if you like, I'd be happy to do a pull.

schematics and build photos are in the folder.

v1 is a swept scan

v2 is a binary state machine scan (famous video)

v3 is a mod-n type scan, which works out in a few cool ways that generate a 6 bit value.

Draw1 uses v2 (bit better on erros) with set and clear pixel buttons to allow drawing.



 messed up my light pen circuit for this post, so I'm just working on that...

Ask questions!
