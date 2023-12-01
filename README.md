# uLisp KeyboardFeatherWing
A version of uLisp to convert Feather ARM boards with the KeyboardFeatherWing into a self-contained handheld Lisp computer.
It's based on uLisp T-Deck with some minor modifications: This version relies on the BBQ10 keyboard version by solder.party
and the ILI9341 display driver.
Currently tested with Adafruit Feather M0 and Feather M4 Express - SD functions not tested yet.
Requires installation of Adafruit ILI9341 library in Arduino environment as well as the BBQ10 keyboard library by solder.party to compile.

Local "escape" to terminate a running uLisp program is the built-in joystick (press down).

For more information see [LilyGO T-Deck uLisp Machine](http://www.ulisp.com/show?4JAO).
