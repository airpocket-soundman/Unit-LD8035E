#if ARDUINO_USB_CDC_ON_BOOT
  #define S3USBSerial Serial
#else
  #if ARDUINO_USB_MODE
    #define S3USBSerial USBSerial
  #else
    #error "Please, board settings -> USB CDC On Boot=Enabled"
  #endif
#endif

#include <stdlib.h>
#include "Wire.h"

#include <FastLED.h>

#define PIN_LED    21   // 本体フルカラーLEDの使用端子（G21）
#define NUM_LEDS   1    // 本体フルカラーLEDの数
CRGB leds[NUM_LEDS];

//I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x58

//LD8035E
#define PIN_D0   9
#define PIN_D1   7
#define PIN_D2   5
#define PIN_D3   3
#define PIN_D4  43
#define PIN_D5  46
#define PIN_D6   1


void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
  /*
  leds[0] = CRGB::Red;
  FastLED.show();
  delay(500);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  leds[0] = CRGB(40, 40, 40);
  FastLED.show();
  delay(500);
  */
}
