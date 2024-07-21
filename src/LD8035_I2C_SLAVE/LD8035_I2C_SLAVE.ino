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
#define PIN_a   10
#define PIN_b    9
#define PIN_c    5
#define PIN_d    4
#define PIN_e    3
#define PIN_f    2
#define PIN_g    1
#define PIN_m    8
#define PIN_dp   6
#define PIN_G1   7
uint32_t i = 0;


void onRequest() {
  Wire.print(i++);
  Wire.print(" Packets.");
  S3USBSerial.println("onRequest");
}

void onReceive(int len) {
  S3USBSerial.println("");  // 改行
  S3USBSerial.println("onReceive called");  // デバッグメッセージ
  S3USBSerial.printf("onReceive[%d]: ", len);

  char com[3] = {0};  // 2文字を格納するバッファ（+1は終端用）
  char buffer[33] = {0};  // 残りのデータを格納するバッファ（+1は終端用）
  int comIndex = 0;
  int bufferIndex = 0;
  int totalBytesRead = 0;

  while (Wire.available()) {
    char c = Wire.read();
    totalBytesRead++;
    
    if (totalBytesRead == 1) {
      // 最初の1文字を捨てる
      continue;
    } else if (totalBytesRead >= 2 && totalBytesRead <= 3) {
      // 2文字目から3文字目をcomに格納
      if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
        com[comIndex++] = c;
      }
    } else if (bufferIndex < sizeof(buffer) - 1) {
      // 残りのデータをbufferに格納
      if (c >= 32 && c <= 126) {  // 表示可能なASCII文字のみをバッファに格納
        buffer[bufferIndex++] = c;
      }
    } else {
      // バッファサイズを超えたデータを無視（必要に応じてデバッグ出力可能）
      S3USBSerial.print("*");  // 超過データを視覚的に確認するためのデバッグ出力
    }
  }

  //com[comIndex] = '\0';  // comバッファの終端を設定
  //buffer[bufferIndex] = '\0';  // bufferバッファの終端を設定

  // 受信したデータを表示
  S3USBSerial.print("Command: ");
  S3USBSerial.println(com);
  S3USBSerial.print("Data: ");
  S3USBSerial.println(buffer);
  
  // comの内容に応じて関数を呼び出す
  if (strcmp(com, "N:") == 0) {
    displayNumber(buffer);
  } else if (strcmp(com, "A:") == 0) {
    displayAnimation(buffer);
  } else if (strcmp(com, "M:") == 0) {
    displayManual(buffer);
  }
}

void displayNumber(char buffer[33]){
  S3USBSerial.print("number:");
  S3USBSerial.println("buffer");
}

void displayAnimation(char buffer[33]){
  S3USBSerial.print("animation:");
  S3USBSerial.println("buffer");
}

void displayManual(char buffer[33]){
  S3USBSerial.print("number:");
  S3USBSerial.println("buffer");
}

void setup() {
  S3USBSerial.begin(115200);
  FastLED.addLeds<WS2812B, PIN_LED, GRB>(leds, NUM_LEDS);
  leds[0] = CRGB(40, 40, 40);   // 白色（赤, 緑, 青）※3色それぞれの明るさを0〜255で指定

  // デジタル出力ピンの設定
  pinMode(PIN_a, OUTPUT);
  pinMode(PIN_b, OUTPUT);
  pinMode(PIN_c, OUTPUT);
  pinMode(PIN_d, OUTPUT);
  pinMode(PIN_e, OUTPUT);
  pinMode(PIN_f, OUTPUT);
  pinMode(PIN_g, OUTPUT);
  pinMode(PIN_m, OUTPUT);
  pinMode(PIN_dp, OUTPUT);
  pinMode(PIN_G1, OUTPUT);

  digitalWrite(PIN_G1, HIGH);
  
  
  digitalWrite(PIN_a, HIGH);
  digitalWrite(PIN_b, HIGH);
  digitalWrite(PIN_c, HIGH);
  digitalWrite(PIN_d, HIGH);
  digitalWrite(PIN_e, HIGH);
  digitalWrite(PIN_f, HIGH);
  digitalWrite(PIN_g, HIGH);
  digitalWrite(PIN_m, HIGH);
  digitalWrite(PIN_dp, HIGH);


}

void loop() {
  // put your main code here, to run repeatedly:
  
  //  leds[0] = CRGB::Red;
  leds[0] = CRGB(10, 0, 0);
  FastLED.show();
  delay(500);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  leds[0] = CRGB(10, 10, 10);
  FastLED.show();
  delay(500);
}
