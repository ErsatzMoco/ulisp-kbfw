# uLisp KeyboardFeatherWing
A version of uLisp to convert Feather ARM boards with the KeyboardFeatherWing into a self-contained handheld Lisp computer.
It's based on uLisp T-Deck with some minor modifications: This version relies on the BBQ10 keyboard version by solder.party
and the ILI9341 display driver as well as the Adafruit NeoPixel library and the Adafruit STMPE610 library.

Currently tested with Adafruit Feather M0, Feather M4 Express and Feather RP2040.

SD supported and tested on Feather M0 and Feather M4.
* SD card must be formatted with FAT (not FAT32).
* SD unsupported on Feather RP2040 when using the (default) LittleFS mechanism.

Built-in NeoPixel supported by default and tested on Feather M0, Feather M4 and Feather RP2040. For information see:
[uLisp NeoPixel Extension](http://www.ulisp.com/show?4GMV)

Touch screen supported and tested on Feather M0 and Feather M4. It does not work with the RP2040, probably due to some SPI issues I cannot solve.

Both NeoPixel and touch screen support are now part of the single extension file based on the uLisp NeoPixel Extension mentioned above. For uLisp functions made available this way please see the extension file itself and the documentation of the drivers wrapped in them.

Special keys now functional: 
* Outer left: Toggle keyboard backlight
* Inner left: <
* Inner right: >
* Outer right: Clear screen

Keyboard translations:
* $ translates to =
* [microphone] translates to ~
* _ translates to \

(@ not translated)

Requires installation of Adafruit ILI9341 library in Arduino environment as well as the BBQ10 keyboard library by solder.party to compile.

For more information see [LilyGO T-Deck uLisp Machine](http://www.ulisp.com/show?4JAO).
