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

// I2C
#define PIN_SDA 13
#define PIN_SCL 15
#define I2C_DEV_ADDR 0x60

// LD8035E
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

// 数字の0〜9に対応する配列
const bool digitPatterns[14][9] = {
// a  b  c  d  e  f  g  -  dp
  {1, 1, 1, 1, 1, 1, 0, 0, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1, 0, 0}, // 2
  {1, 1, 1, 1, 0, 0, 1, 0, 0}, // 3
  {0, 1, 1, 0, 0, 1, 1, 0, 0}, // 4
  {1, 0, 1, 1, 0, 1, 1, 0, 0}, // 5
  {1, 0, 1, 1, 1, 1, 1, 0, 0}, // 6
  {1, 1, 1, 0, 0, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1, 0, 0}, // 8
  {1, 1, 1, 1, 0, 1, 1, 0, 0}, // 9
  {0, 0, 0, 0, 0, 0, 0, 1, 1}, // :
  {0, 0, 0, 0, 0, 0, 0, 0, 1}, // .
  {0, 0, 0, 0, 0, 0, 0, 1, 0}, // -
  {0, 0, 0, 0, 0, 0, 0, 0, 0}  // 空白
};

// アニメーションパターンの定義
const bool animations[2][8][9] = {
  {
// a  b  c  d  e  f  g  -  dp
    {1, 0, 0, 0, 0, 0, 0, 0, 0}, // 0
    {0, 1, 0, 0, 0, 0, 0, 0, 0}, // 1
    {0, 0, 1, 0, 0, 0, 0, 0, 0}, // 2
    {0, 0, 0, 1, 0, 0, 0, 0, 0}, // 3
    {0, 0, 0, 0, 1, 0, 0, 0, 0}, // 4
    {0, 0, 0, 0, 0, 1, 0, 0, 0}, // 5
    {0, 0, 0, 0, 0, 0, 0, 0, 0}, // 空白（0以外の要素に注意）
    {0, 0, 0, 0, 0, 0, 0, 0, 0}  // 空白（0以外の要素に注意）
  },
  {
// a  b  c  d  e  f  g  -  dp
    {1, 0, 0, 0, 0, 0, 0, 0, 0}, // 0
    {0, 1, 0, 0, 0, 0, 0, 0, 0}, // 1
    {0, 0, 0, 0, 0, 0, 1, 0, 0}, // 2
    {0, 0, 0, 0, 1, 0, 0, 0, 0}, // 3
    {0, 0, 0, 1, 0, 0, 0, 0, 0}, // 4
    {0, 0, 1, 0, 0, 0, 0, 0, 0}, // 5
    {0, 0, 0, 0, 0, 0, 1, 0, 0}, // 6
    {0, 0, 0, 0, 0, 1, 0, 0, 0}  // 7
  }
};

void setDisplay(const bool pins[9]) {
  digitalWrite(PIN_a, pins[0] ? HIGH : LOW);
  digitalWrite(PIN_b, pins[1] ? HIGH : LOW);
  digitalWrite(PIN_c, pins[2] ? HIGH : LOW);
  digitalWrite(PIN_d, pins[3] ? HIGH : LOW);
  digitalWrite(PIN_e, pins[4] ? HIGH : LOW);
  digitalWrite(PIN_f, pins[5] ? HIGH : LOW);
  digitalWrite(PIN_g, pins[6] ? HIGH : LOW);
  digitalWrite(PIN_m, pins[7] ? HIGH : LOW);
  digitalWrite(PIN_dp, pins[8] ? HIGH : LOW);
}

void setNumber(int num) {
  if (num >= 0 && num <= 13) {
    setDisplay(digitPatterns[num]);
  } else {
    S3USBSerial.println("Invalid number. Please enter a number between 0 and 13.");
  }
}

void displayAnimation(int pattern, int delayTime) {
  int frames = (pattern == 0) ? 6 : 8; // アニメーションのフレーム数
  for (int i = 0; i < frames; i++) {
    setDisplay(animations[pattern][i]);
    delay(delayTime);
  }
}

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

  // 受信したデータを表示
  S3USBSerial.print("Command: ");
  S3USBSerial.println(com);
  S3USBSerial.print("Data: ");
  S3USBSerial.println(buffer);
  
  // comの内容に応じて関数を呼び出す
  if (strcmp(com, "N:") == 0) {
    displayNumber(buffer);
  } else if (strcmp(com, "A:") == 0) {
    displayAnimation(buffer[0] - '0', atoi(buffer + 1));
  } else if (strcmp(com, "M:") == 0) {
    displayManual(buffer);
  }
}

void displayNumber(char buffer[33]){
  S3USBSerial.print("number:");
  S3USBSerial.println(buffer);
  
  // 例として、最初の文字を数字として認識し表示
  int num;
  if (buffer[0] >= '0' && buffer[0] <= '9') {
    num = buffer[0] - '0'; // '0'から'9'の文字を数値に変換
  } else if (buffer[0] == ':') {
    num = 10;
  } else if (buffer[0] == '.') {
    num = 11;
  } else if (buffer[0] == '-') {
    num = 12;
  } else {
    num = 13; // その他の文字は空白として認識
  }
  setNumber(num);
}

void displayAnimation(char buffer[33]){
  S3USBSerial.print("animation:");
  S3USBSerial.println(buffer);
  
  // パターン番号とディレイ時間を取得
  int pattern = buffer[0] - '0'; // 最初の文字を数値に変換
  int delayTime = atoi(buffer + 1); // 残りの文字を数値に変換

  // アニメーションを表示
  displayAnimation(pattern, delayTime);
}

void displayManual(char buffer[33]){
  S3USBSerial.print("manual:");
  S3USBSerial.println(buffer);
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

  
}

void loop() {
  /*
  leds[0] = CRGB(10, 0, 0);
  FastLED.show();
  delay(500);
  leds[0] = CRGB::Black;
  FastLED.show();
  delay(500);
  leds[0] = CRGB(10, 10, 10);
  FastLED.show();
  delay(500);
  */
  for (i = 0; i < 10; i++)
  {
    displayNumber("9");
    delay(100);
    displayNumber("8");
    delay(100);
    displayNumber("7");
    delay(100);
    displayNumber("6");
    delay(100);
    displayNumber("5");
    delay(100);
    displayNumber("4");
    delay(100);
    displayNumber("3");
    delay(100);
    displayNumber("2");
    delay(100);
    displayNumber("1");
    delay(100);
    displayNumber("0");
    delay(100);
    displayNumber(":");
    delay(100);
    displayNumber(".");
    delay(100);
    displayNumber("-");
    delay(100);
    displayNumber(" ");
  }

  for (int j = 0; j < 20; j++)
  {
    displayAnimation(1,50);
  }
  for (int j = 0; j < 20; j++)
  {
    displayAnimation(0,50);
  }
  displayNumber(" ");
}


