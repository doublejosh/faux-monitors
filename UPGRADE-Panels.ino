/**
 * UPGRADE - Faux Computer Monitors.
 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define SCREEN_SIZE 33
#define TOTAL_PIX 152
#define BOTTOM_ROW_LENGTH 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIX, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across pixel power leads

uint8_t ledShift = TOTAL_PIX - (SCREEN_SIZE + BOTTOM_ROW_LENGTH);
uint8_t wait = 120;

// Command line.
int screen[SCREEN_SIZE];
uint8_t strengthRange = 10, // devided into 255 brightness.
        strengthThreshold = 4, // blank lines, see strengthRange.
        waitChance = 80; // out of 100, varied scrolling.

// Button Row.
uint8_t changeChance = 5, // out of 100, blinkiness.
        colorSplit = 5,
        colorListLength = 7;
uint32_t colorList[7] = {
           strip.Color(0, 255, 0),
           strip.Color(0, 0, 255),
           strip.Color(255, 0, 0),
           strip.Color(255,140,0),
           strip.Color(255, 0, 255),
           strip.Color(255,20, 147),
           strip.Color(0, 0, 0)
         };

void setup() {
//  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
//  #if defined (__AVR_ATtiny85__)
//    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
//  #endif
//  // End of trinket special code
  
  randomSeed(analogRead(0));
  strip.begin();
  strip.show();
}

void loop() {
  commandLine(ledShift);
  buttonRow(ledShift + SCREEN_SIZE);
  strip.show();
  delay(wait);
}

void commandLine(uint8_t shift) {
  uint8_t newStrength, pixelStrength;

  // Decide whether to move/adjust, keep scrolling varied.
  if (waitChance > random(1, 100)) {
    // Loop through screen.
    for (uint16_t i=0; i<SCREEN_SIZE; i++) {
      // Shift value up.
      if (i != SCREEN_SIZE-1) {
        screen[i] = screen[i+1];
      }
      // Determine pixel characteristics.
      pixelStrength = (255 / strengthRange) * screen[i];
      // Set screen row pixel.
      strip.setPixelColor(i + shift, strip.Color(0, pixelStrength, 0));
    }
    // Set new screen pixel.
    newStrength = random(strengthRange),
    screen[SCREEN_SIZE-1] = (newStrength > strengthThreshold) ? newStrength : 0;
  }
}

void buttonRow(uint8_t shift) {
  uint32_t color;

  // Decide whether to animate.
  if (changeChance > random(1, 100)) {
    // Set button pixel.
    color = colorList[random(0, colorListLength)];
    strip.setPixelColor(random(0, BOTTOM_ROW_LENGTH) + shift, color);
  }
}

/** 
 *  Input a value 0 to 255 to get a color value.
 *  The colours are a transition r - g - b - back to r.
 */
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
