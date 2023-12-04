/*
  Keyboard FeatherWing uLisp Extension - Version 0.1 - 4th Dec 2023

  Based on:
  NeoPixel uLisp Extension - Version 1a - 22nd May 2023
  See http://www.ulisp.com/show?4GMV
*/

#include <Adafruit_NeoPixel.h>
#include <Adafruit_STMPE610.h>

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
#elif (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4) || defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)) && defined(kbfw)
  #define NEOPIXEL_NUM 1
  #define PIN_NEOPIXEL 11
#endif

#if (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4)) && defined(kbfw)
  #define PIN_STMPE_CS 6
  #define TS_MINX 150
  #define TS_MINY 130
  #define TS_MAXX 3800
  #define TS_MAXY 4000
#endif

Adafruit_NeoPixel pixels(NEOPIXEL_NUM, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

#if (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4)) && defined(kbfw)
  Adafruit_STMPE610 touch(PIN_STMPE_CS);
#endif

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

#if (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4)) && defined(kbfw)
/*
  (touch-begin)
  Initialize touch screen controller.
*/
object *fn_TouchBegin (object *args, object *env) {
  (void) args, (void) env;
  touch.begin();
  tft.begin();
  tft.setRotation(1);
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
  p.x = map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);
  p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());

  object *px = number(p.x);
  object *py = number(p.y);

  tft.begin();
  tft.setRotation(1);

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
  p.x = map(p.x, TS_MINY, TS_MAXY, tft.height(), 0);
  p.y = map(p.y, TS_MINX, TS_MAXX, 0, tft.width());

  object *px = number(p.x);
  object *py = number(p.y);

  tft.begin();
  tft.setRotation(1);

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
  tft.begin();
  tft.setRotation(1);
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
  tft.begin();
  tft.setRotation(1);
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
  tft.begin();
  tft.setRotation(1);
  return number(bsize);
}
#endif


// Symbol names
const char stringPixelsBegin[] PROGMEM = "pixels-begin";
const char stringPixelsClear[] PROGMEM = "pixels-clear";
const char stringPixelsFill[] PROGMEM = "pixels-fill";
const char stringPixelsSetPixelColor[] PROGMEM = "pixels-set-pixel-color";
const char stringPixelsColor[] PROGMEM = "pixels-color";
const char stringPixelsColorHSV[] PROGMEM = "pixels-color-hsv";
const char stringPixelsShow[] PROGMEM = "pixels-show";
const char stringPixelsRainbow[] PROGMEM = "pixels-rainbow";

#if (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4)) && defined(kbfw)
const char stringTouchBegin[] PROGMEM = "touch-begin";
const char stringTouchGetPoint[] PROGMEM = "touch-get-point";
const char stringTouchWaitForTouch[] PROGMEM = "touch-wait-for-touch";
const char stringTouchTouched[] PROGMEM = "touch-touched";
const char stringTouchBufferEmpty[] PROGMEM = "touch-buffer-empty";
const char stringTouchBufferSize[] PROGMEM = "touch-buffer-size";
#endif


// Documentation strings
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

#if (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4)) && defined(kbfw)
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
#endif

// Symbol lookup table
const tbl_entry_t lookup_table2[] PROGMEM = {
  { stringPixelsBegin, fn_PixelsBegin, 0200, docPixelsBegin },
  { stringPixelsClear, fn_PixelsClear, 0200, docPixelsClear },
  { stringPixelsFill, fn_PixelsFill, 0203, docPixelsFill },
  { stringPixelsSetPixelColor, fn_PixelsSetPixelColor, 0225, docPixelsSetPixelColor },
  { stringPixelsColor, fn_PixelsColor, 0234, docPixelsColor },
  { stringPixelsColorHSV, fn_PixelsColorHSV, 0233, docPixelsColorHSV },
  { stringPixelsShow, fn_PixelsShow, 0200, docPixelsShow },
  { stringPixelsRainbow, fn_PixelsRainbow, 0205, docPixelsRainbow },

#if (defined(ADAFRUIT_FEATHER_M0) || defined(ARDUINO_FEATHER_M4)) && defined(kbfw)
  { stringTouchBegin, fn_TouchBegin, 0200, docTouchBegin },
  { stringTouchGetPoint, fn_TouchGetPoint, 0200, docTouchGetPoint },
  { stringTouchWaitForTouch, fn_TouchWaitForTouch, 0200, docTouchWaitForTouch },
  { stringTouchTouched, fn_TouchTouched, 0200, docTouchTouched },
  { stringTouchBufferEmpty, fn_TouchBufferEmpty, 0200, docTouchBufferEmpty },
  { stringTouchBufferSize, fn_TouchBufferSize, 0200, docTouchBufferSize },
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
