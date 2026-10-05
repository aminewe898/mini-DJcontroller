#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

const char* DEVICE_NAME = "CYBER_DJ";
const char* SERVICE_UUID = "6e400001-b5a3-f393-e0a9-e50e24dcca9e"; // random-ish
const char* CHAR_CH_A =   "6e400002-b5a3-f393-e0a9-e50e24dcca9e";
const char* CHAR_CH_B =   "6e400003-b5a3-f393-e0a9-e50e24dcca9e";
const char* CHAR_EFFECT = "6e400004-b5a3-f393-e0a9-e50e24dcca9e";

const int bigPin = 5;
const int smallPin = 18;
const int bigChannel = 0;
const int smallChannel = 1;
const int freq = 5000;
const int resolution = 8; // 0-255

volatile uint8_t chA = 0;
volatile uint8_t chB = 0;
volatile uint8_t effectMode = 0;

BLECharacteristic *pCharA;
BLECharacteristic *pCharB;
BLECharacteristic *pCharEffect;

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pChar) {
    std::string val = pChar->getValue();
    if(val.length() >= 1) {
      uint8_t v = (uint8_t)val[0];
      if(pChar == pCharA) {
        chA = v;
        ledcWrite(bigChannel, chA);
      } else if(pChar == pCharB) {
        chB = v;
        ledcWrite(smallChannel, chB);
      } else if(pChar == pCharEffect) {
        effectMode = v;
      }
    }
  }
};

unsigned long lastMillis = 0;
float phase = 0.0;

void setup() {
  Serial.begin(115200);
  // LEDC setup
  ledcSetup(bigChannel, freq, resolution);
  ledcSetup(smallChannel, freq, resolution);
  ledcAttachPin(bigPin, bigChannel);
  ledcAttachPin(smallPin, smallChannel);
  ledcWrite(bigChannel, 0);
  ledcWrite(smallChannel, 0);

  // BLE setup
  BLEDevice::init(DEVICE_NAME);
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharA = pService->createCharacteristic(CHAR_CH_A, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pCharB = pService->createCharacteristic(CHAR_CH_B, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);
  pCharEffect = pService->createCharacteristic(CHAR_EFFECT, BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_WRITE_NR);

  MyCallbacks *cb = new MyCallbacks();
  pCharA->setCallbacks(cb);
  pCharB->setCallbacks(cb);
  pCharEffect->setCallbacks(cb);

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->start();

  Serial.println("BLE CYBER_DJ ready");
}

void loop() {
  unsigned long now = millis();
  // If effectMode is 0, we just keep chA/chB as set by BLE writes.
  if(effectMode == 1) { // breathe
    float t = now / 1000.0;
    uint8_t va = (uint8_t)((sin(t * 2.0 * PI * 0.35) * 0.5 + 0.5) * 255.0);
    uint8_t vb = (uint8_t)((sin(t * 2.0 * PI * 0.35 + PI) * 0.5 + 0.5) * 255.0);
    ledcWrite(bigChannel, va);
    ledcWrite(smallChannel, vb);
  } else if(effectMode == 2) { // crossfade
    float t = now / 1000.0;
    float phase = fmod(t * 0.25, 1.0); // 4s cycle
    uint8_t va = (uint8_t)((1.0 - phase) * 255.0);
    uint8_t vb = (uint8_t)(phase * 255.0);
    ledcWrite(bigChannel, va);
    ledcWrite(smallChannel, vb);
  } else if(effectMode == 3) { // strobe
    int strobeFreq = 10;
    bool on = ((int)(now / (1000 / strobeFreq)) % 2) == 0;
    ledcWrite(bigChannel, on ? 255 : 0);
    ledcWrite(smallChannel, on ? 128 : 0);
  } else {
    // mode 0: write last-ch values (set through BLE)
    ledcWrite(bigChannel, chA);
    ledcWrite(smallChannel, chB);
  }
  delay(10);
}
