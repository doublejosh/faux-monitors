/**
 * UPGRADE - Faux Computer Monitors.
 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define SCREEN_SIZE 34
#define TOTAL_PIX 200
#define BOTTOM_ROW_LENGTH 8

Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIX, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across pixel power leads

uint8_t ledShift = 110;
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
           strip.Color(0, 255, 0), // green
           strip.Color(0, 0, 255), // blue
           strip.Color(255, 0, 0), // red
           strip.Color(255, 255, 0), // yellow
           //strip.Color(255,20, 147), //pink
           strip.Color(0, 0, 0) // off
         };

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
  strip.begin();
  strip.show();
}

void loop() {
  commandLine(ledShift);
  buttonRow(ledShift + SCREEN_SIZE, BOTTOM_ROW_LENGTH);
  seriesRows(ledShift + SCREEN_SIZE + BOTTOM_ROW_LENGTH, 3);
  seriesRows(ledShift + SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 1, 1);
  seriesRows(ledShift + SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3, 5);
  seriesRows(ledShift + SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3 + 5, 9);
  seriesRows(ledShift + SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3 + 5 + 9, 7);
  seriesRows(ledShift + SCREEN_SIZE + BOTTOM_ROW_LENGTH + 3 + 3 + 5 + 9 + 8, 3);
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

void buttonRow(uint8_t start, uint8_t pixelLength) {
  uint32_t color;

  // Decide whether to animate.
  if (changeChanceRow > random(1, 100)) {
    // Set button pixel.
    color = colorList[random(0, colorListLength)];
    strip.setPixelColor(random(0, pixelLength) + start, color);
  }
}

void seriesRows(uint8_t start, uint8_t pixelLength) {
  uint32_t color;
  boolean  already = false;

  if (changeSeriesChange < random(1, 100)) {
    return;
  }

  // Move existing pixels.
  for (uint8_t i=0; i<pixelLength; i++) {
    color = strip.getPixelColor(start + i);
    // Back to off.
    strip.setPixelColor(start + i, strip.Color(0, 0, 0));
    
    // Lit pixel.
    if (color != strip.Color(0, 0, 0)) {
      already = true;
      // Last pixel.
      if (i == (pixelLength - 1)) {
        //strip.setPixelColor(start, strip.getPixelColor(start + i));
      } else {
        // Move pixel along.
        strip.setPixelColor(start + i + 1, color);
        break;
      }
    }
  }

  // Decide whether to initiate new sequence.
  if (!already && changeChanceSeries > random(1, 100)) {
    // Don't include off.
    strip.setPixelColor(start, colorList[random(0, colorListLength - 1)]);
  }
}
