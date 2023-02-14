#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define WORK_PIN 27
#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914f"
#define CHARACTERISTIC_SIGNAL_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a7"

BLEServer* pServer = NULL;
BLECharacteristic* pSignalCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class ServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };

  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};

void setup() {
  Serial.begin(115200);

  ledcAttachPin(WORK_PIN, 1);
  ledcSetup(1, 12000, 8);

  BLEDevice::init("toy_1");

  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new ServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pSignalCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_SIGNAL_UUID,
    BLECharacteristic::PROPERTY_WRITE
  );

  pSignalCharacteristic->addDescriptor(new BLE2902());

  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(false);
  pAdvertising->setMinPreferred(0x0);
  BLEDevice::startAdvertising();
}

void loop() {
  if (deviceConnected) {
    std::string signalData = pSignalCharacteristic->getValue();
    String normalizeData = signalData.c_str();
    if (normalizeData != "") {
      ledcWrite(1, normalizeData.toInt());
    }
    delay(30);
  }

  if (!deviceConnected && oldDeviceConnected) {
    delay(500);
    pServer->startAdvertising();
    oldDeviceConnected = deviceConnected;
  }

  if (deviceConnected && !oldDeviceConnected) {
    oldDeviceConnected = deviceConnected;
  }
}
