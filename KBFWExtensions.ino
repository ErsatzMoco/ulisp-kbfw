/*
  Keyboard FeatherWing uLisp Extension - Version 0.3 - 14th March 2024

  Based on:
  NeoPixel uLisp Extension - Version 1a - 22nd May 2023
  See http://www.ulisp.com/show?4GMV
*/

// #define radiohead // Outcomment this to switch to LowPowerLab RFM69 library
                     // CURRENTLY MANDATORY BECAUSE OF UNSOLVED ISSUES WITHIN RADIOHEAD LIBRARY --- USE LOWPOWERLAB LIBRARY FOR NOW.

#include <Adafruit_NeoPixel.h>
#if defined kbfw
  #include <Adafruit_STMPE610.h>
#endif
#if defined radiohead
  #include <RH_RF69.h>
#else
  #include <RFM69.h>
#endif
#if defined servolib
  #include <Servo.h>
#endif

#if defined(ARDUINO_PYBADGE_M4) || defined(ARDUINO_PYGAMER_M4)
  #define NEOPIXEL_NUM 5
  #define PIN_NEOPIXEL 8
#elif defined(ARDUINO_NRF52840_CIRCUITPLAY)
  // Defined in core
#elif defined(ARDUINO_QTPY_M0)
  #define NEOPIXEL_NUM 1
#elif defined(ARDUINO_GEMMA_M0) // For NeoPixel Trinkey
  #define NEOPIXEL_NUM 4
  #define PIN_NEOPIXEL 9
#elif defined(ARDUINO_FEATHER_M4)
  #define NEOPIXEL_NUM 1
  #define PIN_NEOPIXEL 8
#endif

// #define NEOPIXEL_NUM 1      //uncomment these two lines when using external NeoPixels - fill in appropriate values
// #define PIN_NEOPIXEL 11


#if defined(kbfw)
  #define PIN_STMPE_CS 6
  #define TS_MINX 150
  #define TS_MINY 130
  #define TS_MAXX 3800
  #define TS_MAXY 4000
  
  #define NEOPIXEL_NUM 1
  #define PIN_NEOPIXEL 11
#endif

#if defined(rfm69)
  #if defined radiohead
    #define FREQUENCY 868.0
  #else
    // #define FREQUENCY RF69_868MHZ
    #define FREQUENCY RF69_433MHZ
  #endif
  #define ENCRYPTKEY "My@@@Encrypt@@@@" //exactly the same 16 characters/bytes on all nodes!
  #define IS_RFM69HCW true // set to 'true' only if you are using an RFM69HCW module like on Feather M0 Radio

  // for Feather M0 Radio
  #if defined(ADAFRUIT_FEATHER_M0)
    #define RFM69_CS 8
    #define RFM69_IRQ 3
    #define RFM69_IRQN 3 // Pin 3 is IRQ 3!
    #define RFM69_RST 4
  // for Feather M4 etc with external RFM69 module
  #elif defined(ARDUINO_FEATHER_M4)
    #define RFM69_CS A2
    #define RFM69_IRQ A1
    #define RFM69_IRQN 5 // Pin A1 is IRQ 5!
    #define RFM69_RST A0
  // for Pimoroni Tiny 2040 with external RFM69 module
  #elif defined(ARDUINO_PIMORONI_TINY2040)
    #define RFM69_CS 1
    #define RFM69_IRQ A1 // RP2040 IRQ number defined in library call!
    #define RFM69_RST A0
  #endif
#endif


#if defined NEOPIXEL_NUM
  Adafruit_NeoPixel pixels(NEOPIXEL_NUM, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
#endif

#if defined(kbfw)
  Adafruit_STMPE610 touch(PIN_STMPE_CS);
#endif

#if defined(rfm69)

    #define PACKETLENGTH 65
    uint8_t pctlen = PACKETLENGTH+1;          //store pctlen globally for access via RadioHead library -- gets changed according to received byte packets
    char *packet = (char*)malloc(PACKETLENGTH+1);    //reserve global buffer memory for send and receive once -- PACKETLENGTH char bytes plus \0

  #if defined radiohead
    RH_RF69 radio(RFM69_CS, RFM69_IRQ);
  #else
    #if defined(ARDUINO_PIMORONI_TINY2040)
      int irqn digitalPinToInterrupt(RFM69_IRQ);
      RFM69 radio(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, irqn);
    #else
      RFM69 radio(RFM69_CS, RFM69_IRQ, IS_RFM69HCW, RFM69_IRQN);
    #endif
  #endif  
#endif

#if defined servolib
    struct ulservo {
      int snum;
      int pin;
      struct ulservo* nextservo;
      Servo servo;
    };

    struct ulservo* servolist = NULL;
    struct ulservo* curservo = NULL;
#endif

#if defined NEOPIXEL_NUM
/*
  (pixels-begin)
  Configures the NeoPixel pin for output.
*/
object *fn_PixelsBegin (object *args, object *env) {
  (void) args, (void) env;
  pixels.begin();
  return nil;
}

/*
  (pixels-clear)
  Sets all pixel colors to off.
*/
object *fn_PixelsClear (object *args, object *env) {
  (void) env;
  pixels.clear();
  return nil;
}

/*
  (pixels-fill [rgbw] [first] [fill])
  Fills all or part of the NeoPixel strip with a fixed 32-bit packed RGB or RGBW value (default 0).
  first, default 0, the first NeoPixel to fill.
  fill, default all, the number of NeoPixels to fill.
*/
object *fn_PixelsFill (object *args, object *env) {
  (void) env;
  uint32_t rgbw = 0;
  int first = 0, fill = 0;
  if (args != NULL) {
    first = checkinteger(first(args));
    args = cdr(args);
    if (args != NULL) {
      fill = checkinteger(first(args));
    }
  }
  pixels.fill(rgbw, first, fill);
  return nil;
}

/*
  (pixels-set-pixel-color index rgbw)
  (pixels-set-pixel-color index red green blue [white])
  Set a pixel's color using either a 32-bit packed RGB or RGBW value,
  or separate red, green, blue, and optionally white components.
*/
object *fn_PixelsSetPixelColor (object *args, object *env) {
  (void) env;
  int nargs = listlength(args);
  int i = checkinteger(first(args));
  if (nargs == 2) pixels.setPixelColor(i, checkinteger(second(args)));
  else {
    int w = 0;
    args = cdr(args);
    int r = checkinteger(first(args)), g = checkinteger(second(args)), b = checkinteger(third(args));
    args = cddr(cdr(args));
    if (args != NULL) w = checkinteger(first(args));
    pixels.setPixelColor(i, r, g, b, w);
  }
  return nil;
}

/*
  (pixels-color red green blue [white])
  Converts separate red, green, blue, and optionally white values into
  a single packed 32-bit RGB or RGBW color.
*/
object *fn_PixelsColor (object *args, object *env) {
  (void) env;
  int w = 0;
  int r = checkinteger(first(args)), g = checkinteger(second(args)), b = checkinteger(third(args));
  args = cddr(cdr(args));
  if (args != NULL) w = checkinteger(first(args));
  return number(pixels.Color(r, g, b, w));
}

/*
  (pixels-color-hsv hue sat val)
  Converts separate hue (0 to 65535), saturation (0 to 255), and value (0 to 255) values into
  a single packed 32-bit RGB or RGBW color.
*/
object *fn_PixelsColorHSV (object *args, object *env) {
  (void) env;
  int hue = checkinteger(first(args)), sat = checkinteger(second(args)), val = checkinteger(third(args));
  return number(pixels.ColorHSV(hue, sat, val));
}

/*
  (pixels-show)
  Transmits the pixel data to the NeoPixels.
*/
object *fn_PixelsShow (object *args, object *env) {
  (void) env;
  pixels.show();
  return nil;
}

/*
  (pixels-rainbow [first-hue] [cycles] [saturation] [brightness] [gammify])
  Fills the NeoPixel strip with one or more cycles of hues. 
  first-hue, default 0, is the hue of the first pixel (0 to 65535).
  cycles, default 1, is the number of cycles.
  saturation, default 255, is the saturation (0 to 255).
  brightness, default 255, is the brightness (0 to 255).
  gammify, default true, applies gamma correction to colours.
*/
object *fn_PixelsRainbow (object *args, object *env) {
  (void) env;
  int firstHue = 0, cycles = 1, saturation = 255, brightness = 255, gammify = 1;
  if (args != NULL) {
    firstHue = checkinteger(first(args));
    args = cdr(args);
    if (args != NULL) {
      cycles = checkinteger(first(args));
      args = cdr(args);
      if (args != NULL) {
        saturation = checkinteger(first(args));
        args = cdr(args);
        if (args != NULL) {
          brightness = checkinteger(first(args));
          args = cdr(args);
          if (args != NULL) {
            gammify = (first(args) == nil) ? false : true;
          }
        }
      }
    }
  }
  pixels.rainbow(firstHue, cycles, saturation, brightness, gammify);
  return nil;
}
#endif


#if defined(kbfw)
/*
  (force-tft)
  Call to regain TFT control and responsiveness after SPI use with other modules.
*/
object *fn_ForceTFT (object *args, object *env) {
  (void) args, (void) env;
  forceTFT();
  return tee;
}

/* put into separate function for easier internal call */
void forceTFT(void) {
  tft.begin();
  tft.setRotation(1);
}


/*
  (touch-begin)
  Initialize touch screen controller.
*/
object *fn_TouchBegin (object *args, object *env) {
  (void) args, (void) env;
  touch.begin();
  #if defined autoforcetft
    forceTFT();
  #endif
  return nil;
}


/*
  Helper function:
  Prepare SDI for touch screen query. Not accessible via uLisp.
  (Not sure why this process is necessary - TFT driver probably changes SPI settings)
*/
void prepareTouch () {
  pinMode(PIN_STMPE_CS, OUTPUT);
  digitalWrite(PIN_STMPE_CS, HIGH);
  SPI.begin();
  static SPISettings mySPISettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);
}


/*
  (touch-get-point)
  Return touched x,y position as a list.
*/
object *fn_TouchGetPoint (object *args, object *env) {
  (void) args, (void) env;

  prepareTouch();
  TS_Point p = touch.getPoint();
  int myx = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
  int myy = map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);

  object *px = number(myx);
  object *py = number(myy);

  #if defined autoforcetft
    forceTFT();
  #endif

  return cons(px, cons(py, NULL));
}


/*
  (touch-wait-for-touch)
  Wait for touch screen response and return touched x,y position as a list.
  Convenience function, not included in driver. (Blocking!)
*/
object *fn_TouchWaitForTouch (object *args, object *env) {
  (void) args, (void) env;

  prepareTouch();
  while (touch.bufferEmpty()) {
    ;
  }
  TS_Point p = touch.getPoint();
  int myx = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());
  int myy = map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);

  object *px = number(myx);
  object *py = number(myy);

  #if defined autoforcetft
    forceTFT();
  #endif

  return cons(px, cons(py, NULL));
}


/*
  (touch-touched)
  Check if something is touching the screen.
*/
object *fn_TouchTouched (object *args, object *env) {
  (void) args, (void) env;
  prepareTouch();
  boolean touched = touch.touched();

  #if defined autoforcetft
    forceTFT();
  #endif

  if (touched) {
    return tee;
  }
  else {
    return nil;
  }
}



/*
  (touch-buffer-empty)
  Check if touch screen buffer is empty.
*/
object *fn_TouchBufferEmpty (object *args, object *env) {
  (void) args, (void) env;
  prepareTouch();
  boolean empty = touch.bufferEmpty();

  #if defined autoforcetft
    forceTFT();
  #endif

  if (empty) {
    return tee;
  }
  else {
    return nil;
  }
}


/*
  (touch-buffer-size)
  Returns the size of the touch screen's FIFO buffer.
*/
object *fn_TouchBufferSize (object *args, object *env) {
  (void) args, (void) env;
  prepareTouch();
  uint8_t bsize = touch.bufferSize();

  #if defined autoforcetft
    forceTFT();
  #endif

  return number(bsize);
}


/*
  (keyboard-get-key)
  Check for key and return keycode if BBQ10 key was recognized with requested state (up, down, pressed).
  Keycodes for special keys: 
  18  = outer right
  6   = outer left
  17  = inner left
  7   = inner right
  1   = joystick up
  2   = joystick down
  3   = joystick left
  4   = joystick right
  5   = joystick pressed
*/
object *fn_KeyboardGetKey (object *args, object *env) {
  (void) env;
  int reqstate = 3;
  if (args != NULL) {
    int mystate = checkinteger(first(args));
    reqstate = constrain(mystate, 1, 3);
  }

  Wire.requestFrom(0x1F, 1);
  if (Wire.available()) {
    const BBQ10Keyboard::KeyEvent key_e = keyboard.keyEvent();
    char temp = key_e.key;
    if ((temp > 0) && (temp < 255)) {
      if (key_e.state == reqstate) {
        return number(temp);
      }
    }
  }
  return nil;
}
#endif


#if defined(rfm69)
/*
  Helper function:
  Prepare SDI for radio use. Not accessible via uLisp.
  (Not sure why this process is necessary - RFM69 driver probably changes SPI settings)
*/
void radioON () {
  SPI.begin();
  pinMode(PIN_RADIO_CS, OUTPUT);
  digitalWrite(PIN_RADIO_CS, LOW);
}

/*
  Helper function:
  Prepare SDI for other use. Not accessible via uLisp.
  (Not sure why this process is necessary - RFM69 driver probably changes SPI settings)
*/
void radioOFF () {
  pinMode(PIN_RADIO_CS, OUTPUT);
  digitalWrite(PIN_RADIO_CS, HIGH);
  SPI.begin();
  static SPISettings mySPISettings = SPISettings(1000000, MSBFIRST, SPI_MODE0);

  #if defined autoforcetft
    forceTFT();
  #endif
}

/*
  (rfm69-begin)
  Start RFM69 radio module with pin and IRQ number assignments and reset it.
*/
object *fn_RFM69Begin (object *args, object *env) {
  (void) env;
 
  #if defined radiohead
    (void) args, (void) env;  //Radiohead library by default without sender/receiver and without network ID
  #else  
    int nodeid = checkinteger(first(args));
    int netid = checkinteger(second(args));
  #endif
  #if defined(rfm69) && defined(ARDUINO_PIMORONI_TINY2040)
    pinMode(RFM69_IRQ, INPUT_PULLUP); 
  #endif

  // Hard Reset the RFM module
  pinMode(RFM69_RST, OUTPUT);
  digitalWrite(RFM69_RST, LOW);

  // manual reset
  digitalWrite(RFM69_RST, HIGH);
  delay(100);
  digitalWrite(RFM69_RST, LOW);
  delay(100);

  pfun_t pf = pserial;

  #if defined radiohead
    bool result = radio.init();
    if (!result)
    {
        pfstring("RH_RFM69 - Module init failed!", pf);
        return nil;
    }
    else
    {
        radio.setFrequency(FREQUENCY);
    }
  #else
    while(!radio.initialize(FREQUENCY, nodeid, netid))
    {
        pfstring("try to init...", pf);
        delay(100);
    }
  #endif

  if (IS_RFM69HCW) // Only for RFM69HCW & HW!
  {
      #if defined radiohead
        radio.setTxPower(13, true);
      #else
        radio.setHighPower(); 
      #endif
  }

  #if !defined radiohead
    radio.setPowerLevel(31); // power output ranges from 0 (5dBm) to 31 (20dBm)
    radio.encrypt(ENCRYPTKEY);
  #else
    radio.setEncryptionKey((uint8_t*)ENCRYPTKEY);
  #endif
  
  
  radioOFF();
  
  if (IS_RFM69HCW)
  {
      pfstring("RFM69 HCW initialized!\n", pf);
  }
  else
  {
      pfstring("RFM69 initialized!\n", pf);
  }
  pint(FREQUENCY, pf);
  #if !defined radiohead
    pserial(' ');
    pint(nodeid, pf);
    pserial(' ');
    pint(netid, pf);
  #endif
  pserial('\n');

  return nil;
}


/*
  (rfm69-send)
  Send string data package to specified receiver ID optionally requesting hardware acknowledge.
*/
object *fn_RFM69Send (object *args, object *env) {
  (void) env;
  
  if (args != NULL) {

    int receiver;
    bool ack = false;

    #if !defined radiohead
      receiver = checkinteger(first(args));
      args = cdr(args);
    #endif
    cstring(first(args), packet, PACKETLENGTH+1);   //build c-string from uLisp string, therefore includes additional '\0'

    #if !defined radiohead
      args = cdr(args);
      if (args != NULL) {
        ack = (first(args) == nil) ? false : true;
        }
    #endif
      
    radioON();
    #if defined radiohead
      bool result = radio.send((uint8_t*)packet, strlen(packet)+1);   //use *real* packet length for send, i.e. string may be shorter than PACKETLENGTH. '\0' sent too!
      if (!result)
      {
        radioOFF();
        pstring("RH_RFM69 send failed!", (pfun_t)pserial);
        return nil;   
      }
    #else
      radio.send(receiver, packet, strlen(packet)+1, ack);  //use *real* packet length for send, i.e. string may be shorter than PACKETLENGTH. '\0' sent too!
    #endif
    radioOFF();
    pstring(packet, (pfun_t)pserial);
    return tee;    
  }
  radioOFF();
  return nil;
}

/*
  (rfm69-receive)
  Retrieve string data package if something has been received.
*/
object *fn_RFM69Receive (object *args, object *env) {
  (void) env; (void) args;

  radioON();

  #if defined radiohead
    if (radio.available())
    {      
          bool result = radio.recv((uint8_t*)packet, &pctlen);   // RadioHead lib stores length of received packet in predefined global variable
          radioOFF();
          if (result)
          {
            packet[pctlen] = 0;  // add null terminating string for conversion into uLisp string
            return lispstring(packet);
          }
          else
          {
            pstring("RH_RFM69 receive failed!", (pfun_t)pserial);
            return nil;
          }
          
    }
  #else
    if (radio.receiveDone())
    {
          radioOFF();
          return lispstring((char*)radio.DATA);
    }
  #endif
  radioOFF();
  return nil;
}

/*
  (rfm69-get-rssi)
  Obtain signal strength reported at last transmit.
*/
object *fn_RFM69GetRSSI (object *args, object *env) {
  (void) env; (void) args;

  #if defined radiohead
    object* rssi = number(radio.rssiRead());
  #else
    object* rssi = number(radio.RSSI);
  #endif

  radioOFF();
  return rssi;
}
#endif


#if defined(servolib)
/*
  (servo-attach)
  Attach servo snum to pin. Optionally define new pulse width min/max in microseconds.
*/
object *fn_ServoAttach (object *args, object *env) {
  (void) env;
 
  int snum = checkinteger(first(args));   // zero based index!;
  int pin = checkinteger(second(args));
  int usmin = 544;
  int usmax = 2400;

  args = cdr(args);
  args = cdr(args);
  if (args != NULL) {
    usmin = checkinteger(first(args));
    args = cdr(args);
    if (args != NULL) {
      usmax = checkinteger(first(args));
    }
  }

  if(servolist == NULL) {
    if((servolist = (struct ulservo*)malloc(sizeof(struct ulservo))) == NULL) {
       pfstring("Out of memory", (pfun_t)pserial);
       return nil;
    }

    servolist->snum = snum;
    servolist->pin = pin;
    servolist->nextservo = NULL;
    servolist->servo = Servo();
    servolist->servo.attach(pin, usmin, usmax);
  }
  else {
    curservo = servolist;
    while(curservo->nextservo != NULL) {
      if ((curservo->snum == snum) || (curservo->pin == pin)) {
        pfstring("Servo number or pin already in use!", (pfun_t)pserial);
        return nil;
      }
      curservo = curservo->nextservo;
    }

    if ((curservo->snum == snum) || (curservo->pin == pin)) {
      pfstring("Servo number or pin already in use!", (pfun_t)pserial);
      return nil;
    }

    if((curservo->nextservo = (struct ulservo*)malloc(sizeof(struct ulservo))) == NULL) {
        pfstring("Out of memory", (pfun_t)pserial);
        return nil;
    }

    curservo = curservo->nextservo;

    curservo->snum = snum;
    curservo->pin = pin;
    curservo->servo = Servo();
    curservo->servo.attach(pin, usmin, usmax);
    curservo->nextservo = NULL;
  }
  return tee;
}

/*
  (servo-write)
  Set angle of servo snum in degrees (0 to 180).
*/
object *fn_ServoWrite (object *args, object *env) {
  (void) env;
 
  int snum = checkinteger(first(args));
  int angle = checkinteger(second(args));
  curservo = servolist;

  if (curservo != NULL) {

    while(curservo->snum != snum) {
      curservo = curservo->nextservo;
      if (curservo == NULL) break;
    }

    if(curservo != NULL) {
      curservo->servo.write(angle);
      return number(angle);
    }
  }

  pfstring("Servo not found", (pfun_t)pserial);
  return nil;
}

/*
  (servo-write-microseconds)
  Set angle of servo snum using a pulse width value in microseconds.
*/
object *fn_ServoWriteMicroseconds (object *args, object *env) {
  (void) env;
 
  int snum = checkinteger(first(args));
  int us = checkinteger(second(args));
  curservo = servolist;

  if (curservo != NULL) {

    while(curservo->snum != snum) {
      curservo = curservo->nextservo;
      if (curservo == NULL) break;
    }

    if(curservo != NULL) {
      curservo->servo.writeMicroseconds(us);
      return number(us);
    }
  }

  pfstring("Servo not found", (pfun_t)pserial);
  return nil;
}

/*
  (servo-read)
  Read current angle of servo snum in degrees.
*/
object *fn_ServoRead (object *args, object *env) {
  (void) env;

  int snum = checkinteger(first(args));
  curservo = servolist;

  if (curservo != NULL) {

    while(curservo->snum != snum) {
      curservo = curservo->nextservo;
      if (curservo == NULL) break;
    }

    if(curservo != NULL) {
      return number(curservo->servo.read());
    }
  }

  pfstring("Servo not found", (pfun_t)pserial);
  return nil;
}

/*
  (servo-detach)
  Detach servo snum, thus freeing the assigned pin for other tasks.
*/
object *fn_ServoDetach (object *args, object *env) {
  (void) env;

  int snum = checkinteger(first(args));
  curservo = servolist;
  struct ulservo* lastservo = servolist;

  if (curservo != NULL) {

    while(curservo->snum != snum) {
      lastservo = curservo;
      curservo = curservo->nextservo;
      if (curservo == NULL) break;
    }

    if(curservo != NULL) {
      curservo->servo.detach();
      if (curservo == servolist) {    // delete first element of list
        servolist = curservo->nextservo;
      }
      else {
        lastservo->nextservo = curservo->nextservo;
      }
      free(curservo);
      return tee;
    }
  }

  pfstring("Servo not found", (pfun_t)pserial);
  return nil;
}
#endif


// Symbol names
#if defined NEOPIXEL_NUM
const char stringPixelsBegin[] PROGMEM = "pixels-begin";
const char stringPixelsClear[] PROGMEM = "pixels-clear";
const char stringPixelsFill[] PROGMEM = "pixels-fill";
const char stringPixelsSetPixelColor[] PROGMEM = "pixels-set-pixel-color";
const char stringPixelsColor[] PROGMEM = "pixels-color";
const char stringPixelsColorHSV[] PROGMEM = "pixels-color-hsv";
const char stringPixelsShow[] PROGMEM = "pixels-show";
const char stringPixelsRainbow[] PROGMEM = "pixels-rainbow";
#endif

#if defined(kbfw)
const char stringForceTFT[] PROGMEM = "force-tft";
const char stringTouchBegin[] PROGMEM = "touch-begin";
const char stringTouchGetPoint[] PROGMEM = "touch-get-point";
const char stringTouchWaitForTouch[] PROGMEM = "touch-wait-for-touch";
const char stringTouchTouched[] PROGMEM = "touch-touched";
const char stringTouchBufferEmpty[] PROGMEM = "touch-buffer-empty";
const char stringTouchBufferSize[] PROGMEM = "touch-buffer-size";
const char stringKeyboardGetKey[] PROGMEM = "keyboard-get-key";
#endif

#if defined(rfm69)
const char stringRFM69Begin[] PROGMEM = "rfm69-begin";
const char stringRFM69Send[] PROGMEM = "rfm69-send";
const char stringRFM69Receive[] PROGMEM = "rfm69-receive";
const char stringRFM69GetRSSI[] PROGMEM = "rfm69-get-rssi";
#endif

#if defined(servolib)
const char stringServoAttach[] PROGMEM = "servo-attach";
const char stringServoWrite[] PROGMEM = "servo-write";
const char stringServoWriteMicroseconds[] PROGMEM = "servo-write-microseconds";
const char stringServoRead[] PROGMEM = "servo-read";
const char stringServoDetach[] PROGMEM = "servo-detach";
#endif


// Documentation strings
#if defined NEOPIXEL_NUM
const char docPixelsBegin[] PROGMEM = "(pixels-begin)\n"
"Configures the NeoPixel pin for output.";
const char docPixelsClear[] PROGMEM = "(pixels-clear)\n"
"Sets all pixel colors to 'off'.";
const char docPixelsFill[] PROGMEM = "(pixels-fill [rgbw] [first] [fill])\n"
"Fills all or part of the NeoPixel strip with a fixed 32-bit packed RGB or RGBW value (default 0).\n"
"first, default 0, the first NeoPixel to fill.\n"
"fill, default all, the number of NeoPixels to fill.";
const char docPixelsSetPixelColor[] PROGMEM = "(pixels-set-pixel-color index rgbw)\n"
"(pixels-set-pixel-color index red green blue [white])\n"
"Sets a pixel's color using either a 32-bit packed RGB or RGBW value,\n"
"or separate red, green, blue, and optionally white components.";
const char docPixelsColor[] PROGMEM = "(pixels-color red green blue [white])\n"
"Converts separate red, green, blue, and optionally white values into\n"
"a single packed 32-bit RGB or RGBW color.";
const char docPixelsColorHSV[] PROGMEM = "(pixels-color-hsv hue sat val)\n"
"Converts separate hue (0 to 65535), saturation (0 to 255), and value (0 to 255) values into\n"
"a single packed 32-bit RGB or RGBW color.";
const char docPixelsShow[] PROGMEM = "(pixels-show)\n"
"Transmits the pixel data to the NeoPixels.";
const char docPixelsRainbow[] PROGMEM = "(pixels-rainbow [first-hue] [cycles] [saturation] [brightness] [gammify])\n"
"Fills the NeoPixel strip with one or more cycles of hues.\n"
"first-hue, default 0, is the hue of the first pixel (0 to 65535).\n"
"cycles, default 1, is the number of cycles.\n"
"saturation, default 255, is the saturation (0 to 255).\n"
"brightness, default 255, is the brightness (0 to 255).\n"
"gammify, default true, applies gamma correction to colours.";
#endif

#if defined(kbfw)
const char docForceTFT[] PROGMEM = "(force-tft)\n"
"Call to regain TFT control and responsiveness after SPI use with other modules.";
const char docTouchBegin[] PROGMEM = "(touch-begin)\n"
"Initialize touch screen controller.";
const char docTouchGetPoint[] PROGMEM = "(touch-get-point)\n"
"Return touched x,y position as a list.";
const char docTouchWaitForTouch[] PROGMEM = "(touch-wait-for-touch)\n"
"Wait for touch screen response and return touched x,y position as a list.\n"
"Convenience function, not included in driver. (Blocking!)";
const char docTouchTouched[] PROGMEM = "(touch-touched)\n"
"Check if something is touching the screen.";
const char docTouchBufferEmpty[] PROGMEM = "(touch-buffer-empty)\n"
"Check if touch screen buffer is empty.";
const char docTouchBufferSize[] PROGMEM = "(touch-buffer-size)\n"
"Returns the size of the touch screen's FIFO buffer.";
const char docKeyboardGetKey[] PROGMEM = "(keyboard-get-key)\n"
"Check for key and return keycode if BBQ10 key was recognized with requested state (up, down, pressed).";
#endif

#if defined(rfm69)
const char docRFM69Begin[] PROGMEM = "(rfm69-begin nodeid netid)\n"
"Reset RFM69 module and initialize communication with frequency band, node id and net id.";
const char docRFM69Send[] PROGMEM = "(rfm69-send receiver pckgstr [ack])\n"
"Send string data package to specified receiver ID optionally requesting hardware acknowledge.";
const char docRFM69Receive[] PROGMEM = "(rfm69-receive)\n"
"Retrieve string data package if something has been received.";
const char docRFM69GetRSSI[] PROGMEM = "(rfm69-get-rssi)\n"
"Obtain signal strength reported at last transmit.";
#endif

#if defined(servolib)
const char docServoAttach[] PROGMEM = "(servo-attach snum pin usmin usmax)\n"
"Attach servo snum to pin. Optionally define new pulse width min/max in microseconds.";
const char docServoWrite[] PROGMEM = "(servo-write snum angle)\n"
"Set angle of servo snum in degrees (0 to 180).";
const char docServoWriteMicroseconds[] PROGMEM = "(servo-write snum usecs)\n"
"Set angle of servo snum using a pulse width value in microseconds.";
const char docServoRead[] PROGMEM = "(servo-read snum)\n"
"Read current angle of servo snum in degrees.";
const char docServoDetach[] PROGMEM = "(servo-detach snum)\n"
"Detach servo snum, thus freeing the assigned pin for other tasks.";
#endif

// Symbol lookup table
const tbl_entry_t lookup_table2[] PROGMEM = {
#if defined NEOPIXEL_NUM
  { stringPixelsBegin, fn_PixelsBegin, 0200, docPixelsBegin },
  { stringPixelsClear, fn_PixelsClear, 0200, docPixelsClear },
  { stringPixelsFill, fn_PixelsFill, 0203, docPixelsFill },
  { stringPixelsSetPixelColor, fn_PixelsSetPixelColor, 0225, docPixelsSetPixelColor },
  { stringPixelsColor, fn_PixelsColor, 0234, docPixelsColor },
  { stringPixelsColorHSV, fn_PixelsColorHSV, 0233, docPixelsColorHSV },
  { stringPixelsShow, fn_PixelsShow, 0200, docPixelsShow },
  { stringPixelsRainbow, fn_PixelsRainbow, 0205, docPixelsRainbow },
#endif

#if defined(kbfw)
  { stringForceTFT, fn_ForceTFT, 0200, docForceTFT },
  { stringTouchBegin, fn_TouchBegin, 0200, docTouchBegin },
  { stringTouchGetPoint, fn_TouchGetPoint, 0200, docTouchGetPoint },
  { stringTouchWaitForTouch, fn_TouchWaitForTouch, 0200, docTouchWaitForTouch },
  { stringTouchTouched, fn_TouchTouched, 0200, docTouchTouched },
  { stringTouchBufferEmpty, fn_TouchBufferEmpty, 0200, docTouchBufferEmpty },
  { stringTouchBufferSize, fn_TouchBufferSize, 0200, docTouchBufferSize },
  { stringKeyboardGetKey, fn_KeyboardGetKey, 0201, docKeyboardGetKey },
#endif

#if defined(rfm69)
  #if defined radiohead
    { stringRFM69Begin, fn_RFM69Begin, 0200, docRFM69Begin },
    { stringRFM69Send, fn_RFM69Send, 0211, docRFM69Send },
  #else
    { stringRFM69Begin, fn_RFM69Begin, 0222, docRFM69Begin },
    { stringRFM69Send, fn_RFM69Send, 0223, docRFM69Send },
  #endif
  { stringRFM69Receive, fn_RFM69Receive, 0200, docRFM69Receive },
  { stringRFM69GetRSSI, fn_RFM69GetRSSI, 0200, docRFM69GetRSSI },
#endif

#if defined(servolib)
  { stringServoAttach, fn_ServoAttach, 0224, docServoAttach },
  { stringServoWrite, fn_ServoWrite, 0222, docServoWrite },
  { stringServoWriteMicroseconds, fn_ServoWriteMicroseconds, 0222, docServoWriteMicroseconds },
  { stringServoRead, fn_ServoRead, 0211, docServoRead },
  { stringServoDetach, fn_ServoDetach, 0211, docServoDetach },
#endif  
};

// Table cross-reference functions - do not edit below this line

tbl_entry_t *tables[] = {lookup_table, lookup_table2};
const unsigned int tablesizes[] = { arraysize(lookup_table), arraysize(lookup_table2) };

const tbl_entry_t *table (int n) {
  return tables[n];
}

unsigned int tablesize (int n) {
  return tablesizes[n];
}
