# uLisp KeyboardFeatherWing
A version of uLisp to convert Feather ARM boards with the KeyboardFeatherWing into a self-contained handheld Lisp computer.
It's based on uLisp T-Deck with some minor modifications: This version relies on the BBQ10 keyboard version by solder.party
and the ILI9341 display driver as well as the Adafruit NeoPixel library, the Adafruit STMPE610 library and the RFM69 library by LowPowerLab.

Currently tested with Adafruit Feather M0 (Radio), Feather M4 Express and Feather RP2040.

SD supported and tested on Feather M0 and Feather M4.
* SD card must be formatted with FAT (not FAT32).
* SD unsupported on Feather RP2040 when using the (default) LittleFS mechanism.

Built-in NeoPixel supported by default and tested on Feather M0, Feather M4 and Feather RP2040. For information see:
[uLisp NeoPixel Extension](http://www.ulisp.com/show?4GMV)

Touch screen supported and tested on Feather M0 and Feather M4. It does not work with the RP2040, probably due to some SPI issues I cannot solve.

RFM69 support tested on Adafruit Feather M0 Radio.

Both NeoPixel, touch screen and RFM69 support are part of the single extension file based on the uLisp NeoPixel Extension mentioned above. For uLisp functions made available this way please see the extension file itself and the documentation of the drivers wrapped within them.
Notes for RFM69:
* Sender and receiver must share the same net id (integer number).
* To address the correct receiver, the sender needs to specify the receiver's node id (integer number) when transmitting.
* Maximum payload length 65 characters in a packet.
* To use a separate RFM69 module with other MCUs than the Feather M0 Radio (M4, for example) edit the constants for the module communication pins. This should work but is currently untested.
* Source code contains possibility to use RadioHead library instead of LowPowerLab library BUT this is currently not working, presumably because of internal issues within RadioHead library or incompatibilities with the Feather ecosystem, see here: 
[Adafruit forums] (https://forums.adafruit.com/viewtopic.php?p=973656#p973656)  
(Phenomenon is the same as described there, so it's not (only) an RP2040 issue.)

Special keys now functional: 
* Outer left: History mechanism - call up last line entered into the *local* KBFW-REPL (i.e. not via serial connection)
* Inner left: <
* Inner right: >
* Outer right: Clear screen

Keyboard translations:
* $ translates to =
* [microphone] translates to ~
* _ translates to \

(@ not translated)

Requires installation of Adafruit ILI9341 library in Arduino environment to compile, in addition the BBQ10 keyboard library by solder.party, the Adafruit NeoPixel library and the Adafruit STMPE610 library. To use the RFM69 module on the Feather M0 Radio you also need to install the RFM69 library by LowPowerLab.

For more information see [LilyGO T-Deck uLisp Machine](http://www.ulisp.com/show?4JAO).
