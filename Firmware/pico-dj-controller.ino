/*
  Raspberry Pi Pico PWM Controller (ESP32 BLE removed)

  bigPin   -> GPIO 5
  smallPin -> GPIO 18

  Control via Serial:
  A <0-255>  -> big channel
  B <0-255>  -> small channel
  E <0-3>    -> effect mode
*/

const int bigPin = 5;
const int smallPin = 18;

volatile uint8_t chA = 0;
volatile uint8_t chB = 0;
volatile uint8_t effectMode = 0;

void setup() {
  Serial.begin(115200);

  pinMode(bigPin, OUTPUT);
  pinMode(smallPin, OUTPUT);

  analogWrite(bigPin, 0);
  analogWrite(smallPin, 0);

  Serial.println("CYBER_DJ Pico ready (Serial control)");
}

void handleSerial() {
  if (Serial.available()) {
    char cmd = Serial.read();
    int value = Serial.parseInt();

    switch (cmd) {
      case 'A':
        chA = constrain(value, 0, 255);
        break;
      case 'B':
        chB = constrain(value, 0, 255);
        break;
      case 'E':
        effectMode = constrain(value, 0, 3);
        break;
    }
  }
}

void loop() {
  handleSerial();

  unsigned long now = millis();

  if (effectMode == 1) { // breathe
    float t = now / 1000.0;
    uint8_t va = (sin(t * 2.0 * PI * 0.35) * 0.5 + 0.5) * 255;
    uint8_t vb = (sin(t * 2.0 * PI * 0.35 + PI) * 0.5 + 0.5) * 255;
    analogWrite(bigPin, va);
    analogWrite(smallPin, vb);

  } else if (effectMode == 2) { // crossfade
    float t = now / 1000.0;
    float phase = fmod(t * 0.25, 1.0);
    analogWrite(bigPin, (1.0 - phase) * 255);
    analogWrite(smallPin, phase * 255);

  } else if (effectMode == 3) { // strobe
    int strobeFreq = 10;
    bool on = ((now / (1000 / strobeFreq)) % 2) == 0;
    analogWrite(bigPin, on ? 255 : 0);
    analogWrite(smallPin, on ? 128 : 0);

  } else { // manual
    analogWrite(bigPin, chA);
    analogWrite(smallPin, chB);
  }

  delay(10);
}
