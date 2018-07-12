/**
 * Faux Computer Monitors panels for UPGRADE vehicle.
 * 
 * REMEMBER: To avoid NeoPixel burnout...
 * add 1000 uF capacitor across power leads
 * and 470Ω resister on data leads.
 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN_1 6
#define SCREEN_SIZE 33
#define TOTAL_PIX_1 100
#define BOTTOM_ROW_LENGTH 8

#define PIN_2 7
#define TOTAL_PIX_2 111

Adafruit_NeoPixel strip1 = Adafruit_NeoPixel(TOTAL_PIX_1, PIN_1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel strip2 = Adafruit_NeoPixel(TOTAL_PIX_2, PIN_2, NEO_GRB + NEO_KHZ800);

uint8_t ledShift = 0;
uint8_t wait = 90;

// Command line.
int     screen[SCREEN_SIZE];
uint8_t strengthRange = 10, // devided into 255 brightness.
        strengthThreshold = 4, // blank lines, see strengthRange.
        waitChance = 70; // out of 100, varied scrolling.

// Button Row.
uint8_t  changeChanceRow = 15, // out of 100, blinkiness.
         colorListLength = 6;
uint32_t colorList[6] = {
           strip1.Color(255, 255, 0), // yellow
           strip1.Color(0, 255, 0), // green
           strip1.Color(0, 0, 255), // blue
           strip1.Color(255, 0, 0), // red
           //strip1.Color(255,20, 147), //pink
           strip1.Color(0, 0, 0) // off
         };

// Wave Line.
uint8_t waitChanceWave = 50; // out of 100, varied timing.

// Series groups.
uint8_t  changeChanceSeries = 30, // out of 100, blinkiness.
         changeSeriesChange = 35; // out of 100, varied speed.


void setup() {
//  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
//  #if defined (__AVR_ATtiny85__)
//    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
//  #endif
//  // End of trinket special code
  
  randomSeed(analogRead(0));
  strip1.begin();
  strip2.begin();
  strip1.show();
  strip2.show();
}

/**
 * Main loop.
 */
void loop() {
  // Panel #1.
  commandLine(0, strip1);
  buttonRow(SCREEN_SIZE, BOTTOM_ROW_LENGTH);
  seriesRows(SCREEN_SIZE + BOTTOM_ROW_LENGTH, 3);
  seriesRows(SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 1, 1);
  seriesRows(SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3, 5);
  seriesRows(SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3 + 5, 9);
  seriesRows(SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3 + 5 + 9, 7);
  seriesRows(SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3 + 5 + 9 + 8, 3);

  // Panel #2.
  waveLine(54, strip2); // Assumed end gap of 3.

  // Panel #3.
  //heartBeat(strip3);

  strip1.show();
  strip2.show();
  delay(wait);
}

/**
 * Rolling commands or logging simulation.
 */
void commandLine(uint8_t shift, Adafruit_NeoPixel &strip) {
  uint8_t newStrength,
          pixelStrength;

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

/**
 * Waveform simulation.
 */
void waveLine(uint8_t screenSize, Adafruit_NeoPixel &strip) {
  uint8_t pixelStrength,
          pixelIndex;
  uint32_t color; 

  // Decide whether to move/adjust, keep scrolling varied.
  if (waitChanceWave > random(1, 100)) {
    // Loop through screen.
    for (uint16_t i=0; i<screenSize; i++) {
      // Determine pixel characteristics.      
      pixelStrength = random(0, 255),
      color = (pixelStrength < 170) ? colorWheel(pixelStrength) : 0;
      // Set screen pixel.
      strip.setPixelColor(i, color);
      strip.setPixelColor(((screenSize * 2) + 2) - i, color);
    }
  }
}

/**
 * Separate pixels animation.
 */
void buttonRow(uint8_t start, uint8_t pixelLength) {
  uint32_t color;

  // Decide whether to animate.
  if (changeChanceRow > random(1, 100)) {
    // Set button pixel.
    color = colorList[random(0, colorListLength)];
    strip1.setPixelColor(random(0, pixelLength) + start, color);
  }
}

/**
 * Grouping of pixel(s) animation.
 */
void seriesRows(uint8_t start, uint8_t pixelLength) {
  uint32_t color;
  boolean  already = false;

  if (changeSeriesChange < random(1, 100)) {
    return;
  }

  // Move existing pixels.
  for (uint8_t i=0; i<pixelLength; i++) {
    color = strip1.getPixelColor(start + i);
    // Back to off.
    strip1.setPixelColor(start + i, strip1.Color(0, 0, 0));
    
    // Lit pixel.
    if (color != strip1.Color(0, 0, 0)) {
      already = true;
      // Last pixel.
      if (i == (pixelLength - 1)) {
        //strip1.setPixelColor(start, strip1.getPixelColor(start + i));
      } else {
        // Move pixel along.
        strip1.setPixelColor(start + i + 1, color);
        break;
      }
    }
  }

  // Decide whether to initiate new sequence.
  if (!already && changeChanceSeries > random(1, 100)) {
    // Don't include off.
    strip1.setPixelColor(start, colorList[random(0, colorListLength - 1)]);
  }
}

/**
 * Input a value 0 to 255 to get a color value.
 */
uint32_t colorWheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip1.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip1.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip1.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
