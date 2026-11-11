// HW3-1 Light Stick 加油棒
// 共陽極 RGB LED + IN/VCC/GND 按鈕模組
// 按鈕：切換模式　可變電阻：調亮度

const int PIN_R = 9;
const int PIN_G = 10;
const int PIN_B = 11;
const int PIN_BTN = 2;
const int PIN_POT = A0;

const bool COMMON_ANODE = true;   // 共陽極 = true
const int MODE_COUNT = 4;

int mode = 0;
int brightness = 255;             // 0~255，由旋鈕決定

// 按鈕防彈跳用（模組按下時輸出 HIGH，平常是 LOW）
int lastBtnState = LOW;
unsigned long lastDebounce = 0;

// 輸出顏色（會自動套用亮度）
void setColor(int r, int g, int b) {
  r = r * brightness / 255;
  g = g * brightness / 255;
  b = b * brightness / 255;
  if (COMMON_ANODE) {
    r = 255 - r;
    g = 255 - g;
    b = 255 - b;
  }
  analogWrite(PIN_R, r);
  analogWrite(PIN_G, g);
  analogWrite(PIN_B, b);
}

// 色輪：pos 0~255 對應彩虹顏色
void wheel(byte pos, int &r, int &g, int &b) {
  if (pos < 85) {
    r = 255 - pos * 3; g = pos * 3; b = 0;
  } else if (pos < 170) {
    pos -= 85;
    r = 0; g = 255 - pos * 3; b = pos * 3;
  } else {
    pos -= 170;
    r = pos * 3; g = 0; b = 255 - pos * 3;
  }
}

void setup() {
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_BTN, INPUT);        // 模組自帶電路，不用內建上拉
}

void loop() {
  // 1. 讀旋鈕調亮度（最低留 10，避免完全熄滅）
  brightness = map(analogRead(PIN_POT), 0, 1023, 10, 255);

  // 2. 讀按鈕，按下時切換模式
  int reading = digitalRead(PIN_BTN);
  if (reading != lastBtnState && millis() - lastDebounce > 50) {
    lastDebounce = millis();
    if (reading == HIGH) {          // 按下瞬間
      mode = (mode + 1) % MODE_COUNT;
    }
  }
  lastBtnState = reading;

  // 3. 依模式顯示燈光
  switch (mode) {
    case 0: {  // 白光常亮
      setColor(255, 255, 255);
      break;
    }
    case 1: {  // 彩虹漸變
      int r, g, b;
      byte pos = (millis() / 20) % 256;
      wheel(pos, r, g, b);
      setColor(r, g, b);
      break;
    }
    case 2: {  // 紫色呼吸燈
      int t = (millis() / 5) % 510;
      int v = (t < 255) ? t : 510 - t;
      setColor(v, 0, v);
      break;
    }
    case 3: {  // 紅藍交替閃爍
      if ((millis() / 150) % 2 == 0) {
        setColor(255, 0, 0);
      } else {
        setColor(0, 0, 255);
      }
      break;
    }
  }
}