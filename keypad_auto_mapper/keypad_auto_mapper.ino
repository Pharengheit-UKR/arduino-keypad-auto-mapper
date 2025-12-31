#include <Arduino.h>
// =====================
// LCD CONFIG (optional)
// =====================
#define USE_I2C_LCD 0   // постав 1 якщо LCD1602 з I2C backpack (SDA=A4, SCL=A5)

#if USE_I2C_LCD
  #include <Wire.h>
  #include <LiquidCrystal_I2C.h>
  LiquidCrystal_I2C lcd(0x27, 16, 2); //  0x27 or 0x3F
  void lcdInit() { lcd.init(); lcd.backlight(); }
  void lcdClear() { lcd.clear(); }
  void lcdPrintAt(uint8_t c, uint8_t r, const String &s) { lcd.setCursor(c,r); lcd.print(s); }
#else
  void lcdInit() {}
  void lcdClear() {}
  void lcdPrintAt(uint8_t, uint8_t, const String &) {}
#endif

// =====================
// Keypad scan pins
// =====================
const uint8_t KP_PINS[8] = {2,3,4,5,6,7,8,9};

static void allHiZPullup() {
  for (uint8_t i = 0; i < 8; i++) {
    pinMode(KP_PINS[i], INPUT_PULLUP);
  }
}


static bool scanOnePressPair(uint8_t &pA, uint8_t &pB) {
  for (uint8_t di = 0; di < 8; di++) {
    allHiZPullup();

    pinMode(KP_PINS[di], OUTPUT);
    digitalWrite(KP_PINS[di], LOW);
    delayMicroseconds(300);

    for (uint8_t si = 0; si < 8; si++) {
      if (si == di) continue;

      if (digitalRead(KP_PINS[si]) == LOW) {
        uint8_t a = KP_PINS[di];
        uint8_t b = KP_PINS[si];
        if (a > b) { uint8_t t = a; a = b; b = t; }
        pA = a; pB = b;
        return true;
      }
    }
  }
  return false;
}

static bool waitForPressPair(uint8_t &pA, uint8_t &pB) {
  const uint16_t STABLE_MS = 30;
  uint32_t t0 = 0;
  uint8_t lastA=0, lastB=0;
  bool haveLast = false;

  while (true) {
    uint8_t a,b;
    bool ok = scanOnePressPair(a,b);

    if (!ok) {
      haveLast = false;
      t0 = 0;
      delay(3);
      continue;
    }

    if (!haveLast || a!=lastA || b!=lastB) {
      haveLast = true;
      lastA=a; lastB=b;
      t0 = millis();
    } else {
      if (t0 && (millis() - t0) >= STABLE_MS) {
        pA = lastA; pB = lastB;
        return true;
      }
    }
    delay(3);
  }
}

static void waitForRelease() {
  while (true) {
    uint8_t a,b;
    if (!scanOnePressPair(a,b)) return;
    delay(5);
  }
}

// =====================
// Target sequence
// =====================
const char KEY_SEQ[16] = {
  '1','2','3','A',
  '4','5','6','B',
  '7','8','9','C',
  '*','0','#','D'
};

struct Pair { uint8_t a; uint8_t b; };
Pair pairs[16];

// =====================
// Compute row/col
// =====================
static uint8_t mostFrequentPinIn4Pairs(uint8_t idx0, uint8_t idx1, uint8_t idx2, uint8_t idx3) {
  uint8_t pins[8] = {2,3,4,5,6,7,8,9};
  uint8_t count[8] = {0};

  uint8_t ids[4] = {idx0, idx1, idx2, idx3};
  for (uint8_t k=0;k<4;k++) {
    uint8_t a = pairs[ids[k]].a;
    uint8_t b = pairs[ids[k]].b;
    for (uint8_t i=0;i<8;i++) {
      if (pins[i] == a) count[i]++;
      if (pins[i] == b) count[i]++;
    }
  }

  uint8_t bestPin = pins[0];
  uint8_t bestCnt = count[0];
  for (uint8_t i=1;i<8;i++) {
    if (count[i] > bestCnt) {
      bestCnt = count[i];
      bestPin = pins[i];
    }
  }
  return bestPin;
}

static void printArray(const char *name, uint8_t arr[4]) {
  Serial.print(name);
  Serial.print(" = {");
  for (uint8_t i=0;i<4;i++) {
    Serial.print(arr[i]);
    if (i<3) Serial.print(", ");
  }
  Serial.println("};");
}

void setup() {
  Serial.begin(9600);
  delay(200);

  lcdInit();
  lcdClear();

  Serial.println(F("=== AUTO ROW/COL MAPPER (D2..D9) ==="));
  Serial.println(F("Press keys in order: 1 2 3 A / 4 5 6 B / 7 8 9 C / * 0 # D"));
  Serial.println(F("It will compute rowPins (top->bottom) and colPins (left->right)."));
  Serial.println();

  lcdPrintAt(0,0,"AUTO MAPPER");
  lcdPrintAt(0,1,"Open Serial");

  allHiZPullup();

  for (uint8_t i=0;i<16;i++) {
    char target = KEY_SEQ[i];

    Serial.print(F("Step "));
    Serial.print(i+1);
    Serial.print(F("/16. Press: "));
    Serial.println(target);

    lcdClear();
    lcdPrintAt(0,0,"Press key:");
    lcdPrintAt(11,0,String(target));
    lcdPrintAt(0,1,String(i+1) + "/16");

    uint8_t a,b;
    waitForPressPair(a,b);
    pairs[i] = {a,b};

    Serial.print(F("  Captured pair: "));
    Serial.print(a);
    Serial.print(F(" & "));
    Serial.println(b);

    lcdPrintAt(0,1,"Release...");
    waitForRelease();
    delay(50);
  }

  uint8_t rowPins[4];
  uint8_t colPins[4];

  rowPins[0] = mostFrequentPinIn4Pairs(0,1,2,3);       // 1 2 3 A
  rowPins[1] = mostFrequentPinIn4Pairs(4,5,6,7);       // 4 5 6 B
  rowPins[2] = mostFrequentPinIn4Pairs(8,9,10,11);     // 7 8 9 C
  rowPins[3] = mostFrequentPinIn4Pairs(12,13,14,15);   // * 0 # D

  colPins[0] = mostFrequentPinIn4Pairs(0,4,8,12);      // 1 4 7 *
  colPins[1] = mostFrequentPinIn4Pairs(1,5,9,13);      // 2 5 8 0
  colPins[2] = mostFrequentPinIn4Pairs(2,6,10,14);     // 3 6 9 #
  colPins[3] = mostFrequentPinIn4Pairs(3,7,11,15);     // A B C D

  Serial.println();
  Serial.println(F("=== RESULT ==="));
  printArray("byte rowPins[ROWS]", rowPins);
  printArray("byte colPins[COLS]", colPins);

  lcdClear();
  lcdPrintAt(0,0,"row:");
  lcdPrintAt(4,0,String(rowPins[0])+" "+rowPins[1]+" "+rowPins[2]+" "+rowPins[3]);
  lcdPrintAt(0,1,"col:");
  lcdPrintAt(4,1,String(colPins[0])+" "+colPins[1]+" "+colPins[2]+" "+colPins[3]);

  Serial.println(F("Done. You can copy arrays into Keypad library config."));
}

void loop() {
}
