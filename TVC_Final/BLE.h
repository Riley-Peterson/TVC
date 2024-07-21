#ifndef BLE_H
#define BLE_H

#include <pin_defs.h>
#include <Arduino.h>
#include <SPIMemory.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

bool deviceConnected = false;

BLECharacteristic *pTxCharacteristic;
BLECharacteristic *pRxCharacteristic;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };
    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
      
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
          gyro.readSensor();
          accel.readSensor();
          vec3_t gyro_rads(gyro.getGyroX_rads(), gyro.getGyroY_rads(), gyro.getGyroZ_rads());
          vec3_t curr_accel(accel.getAccelX_mss(), accel.getAccelY_mss(), accel.getAccelZ_mss());
          vec3_t filt_accel = ALPHA * curr_accel;
          fusion.update(gyro_rads.x, gyro_rads.y, gyro_rads.z, filt_accel.x, filt_accel.y, filt_accel.z);
          // Capture the initial quaternion
          initialQuat = fusion.getQuat();
          // Compute its conjugate
          initialQuatConjugate = {initialQuat.w, -initialQuat.v.x, -initialQuat.v.y, -initialQuat.v.z};
        }
        else if (rxValue.indexOf("B") != -1) {
          Serial.println("Turning OFF!");
          pTxCharacteristic->setValue("Turning OFF!\n");
          pTxCharacteristic->notify();
        }
        Serial.println();
        Serial.println("*********");
      }
    }
};

inline void initialize_bluetooth() {
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