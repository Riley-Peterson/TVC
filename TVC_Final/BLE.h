#ifndef BLE_H
#define BLE_H

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

bool deviceConnected = false;
bool armed = false;

BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      pServer->getAdvertising()->start();
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      String rxValue = pCharacteristic->getValue().c_str();
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
        }
        Serial.println();
        // Do stuff based on the command received from the app
        if (rxValue.indexOf("CAL") != -1) { 
          Serial.println("Calibrating...");
          pTxCharacteristic->setValue("Calibrating...\n");
          pTxCharacteristic->notify();
          calibrateIMU();
          delay(100);
          pTxCharacteristic->setValue("Calibrated!\n");
          pTxCharacteristic->notify();
        }
        else if (rxValue.indexOf("ARM") != -1) {
          Serial.println("Arming...");
          pTxCharacteristic->setValue("Arming...\n");
          pTxCharacteristic->notify();
          armed = true;
          delay(100);
          pTxCharacteristic->setValue("Armed!\n");
          pTxCharacteristic->notify();
        }
        Serial.println();
        Serial.println("*********");
      }
    }
};

inline void initializeBluetooth() {
  BLEDevice::init("Sigmoid Flight Controller");
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
  pTxCharacteristic->addDescriptor(new BLE2902());
  pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
  pRxCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();
  pServer->getAdvertising()->start();
}

#endif // BLE_H