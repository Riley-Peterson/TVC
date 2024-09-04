#ifndef FLASH_CHIP_H
#define FLASH_CHIP_H

#include <SPI.h>
#include <SPIMemory.h>

SPIFlash flash(chip_select);
uint32_t curr_page = 0;
//FlightData data;
//FlightPhase curr_phase;
String curr_phase;

inline void initializeFlash() {
  flash.begin();
}

inline void writeData(String dataToWrite) {
  if(curr_page > 131000) {
    Serial.println("Max page reached, cannot write any more!");
    onGround();
  }
  flash.writeStr(curr_page * 256, dataToWrite);
  curr_page++;
}

inline void dumpData() {
  Serial.println("Time, Temp, Pressure, Altitude, Pitch, Yaw, Roll, AccelX, AccelY, AccelZ, Servox, Servoy, phase");
  for(int page = 0; page < 131000;) {
    String tempString;
    String printString = "";
    
    // Loop to read all 13 fields for the current record
    for(int i = 0; i < 13; i++) {
      flash.readStr(page * 256, tempString);  // Read data from current page
      if(i < 12) {
        printString += tempString + ", ";     // Append data with a comma
      } else {
        printString += tempString;            // Last item without a comma
      }
      page++;  // Increment page for the next field
    }
    
    Serial.println(printString);  // Print the complete record
  }
}

/*
inline void writeData(FlightData dataToWrite) {
  
  if(curr_page > 131000) {
    Serial.println("Max page reached, cannot write any more!");
    onGround();
  }
  flash.writeAnything(curr_page * 256, dataToWrite);
  curr_page++;
}

inline void dumpData() {
  std::array<FlightData, 256 / sizeof(FlightData)> main_data;
  Serial.println(FLASH_MAIN_HEADER);
  bool at_end = false;
  for(uint32_t page = 0; page < 131000; page++) {
    if(at_end) {
      break;
    }
    if(curr_page > 131000) {
      break;
    }
    flash.readAnything(curr_page * 256, main_data);
    for(auto &e : main_data) {
      quat_t q = e.orientation;
      vec3_t a = e.acceleration;
      Serial.printf("%u, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %.4f, %d\n", e.time, e.temperature, e.pressure, e.altitude, q.w, q.v.x, q.v.y, q.v.z, a.x, a.y, a.z, e.servoActuationX, e.servoActuationY, e.phase);
      Serial.println();
      delay(1);
    }
  }
}
*/

#endif