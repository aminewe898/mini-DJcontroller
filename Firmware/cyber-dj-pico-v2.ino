/*
  CYBER-DJ Pico Controller V2
  Raspberry Pi Pico (RP2040)
  Offline PWM DJ lighting controller

  Commands:
  A <0-255>  Big channel
  B <0-255>  Small channel
  E <0-3>    Effect mode
  S <1-100>  Effect speed
*/

const int BIG_PIN   = 5;
const int SMALL_PIN = 18;

uint8_t chA = 0;
uint8_t chB = 0;
uint8_t effectMode = 0;
uint8_t speed = 30;

unsigned long lastUpdate = 0;
float phase = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(BIG_PIN, OUTPUT);
  pinMode(SMALL_PIN, OUTPUT);

  analogWrite(BIG_PIN, 0);
  analogWrite(SMALL_PIN, 0);

  Serial.println("CYBER-DJ Pico V2 ready");
}

void handleSerial() {
  if (!Serial.available()) return;

  char cmd = Serial.read();
  int value = Serial.parseInt();

  switch (cmd) {
    case 'A': chA = constrain(value, 0, 255); break;
    case 'B': chB = constrain(value, 0, 255); break;
    case 'E': effectMode = constrain(value, 0, 3); break;
    case 'S': speed = constrain(value, 1, 100); break;
  }
}

void loop() {
  handleSerial();

  unsigned long now = millis();
  if (now - lastUpdate < 10) return;
  lastUpdate = now;

  float dt = speed * 0.0001;
  phase += dt;
  if (phase > 1.0) phase -= 1.0;

  switch (effectMode) {
    case 1: breathe(); break;
    case 2: crossfade(); break;
    case 3: strobe(); break;
    default: manual(); break;
  }
}

void manual() {
  analogWrite(BIG_PIN, chA);
  analogWrite(SMALL_PIN, chB);
}

void breathe() {
  float v = sin(phase * TWO_PI) * 0.5 + 0.5;
  analogWrite(BIG_PIN, v * 255);
  analogWrite(SMALL_PIN, (1.0 - v) * 255);
}

void crossfade() {
  analogWrite(BIG_PIN, (1.0 - phase) * 255);
  analogWrite(SMALL_PIN, phase * 255);
}

void strobe() {
  bool on = ((int)(phase * 20) % 2) == 0;
  analogWrite(BIG_PIN, on ? 255 : 0);
  analogWrite(SMALL_PIN, on ? 128 : 0);
}
