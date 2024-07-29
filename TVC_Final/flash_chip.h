#ifndef FLASH_CHIP_H
#define FLASH_CHIP_H

#include <SPI.h>
#include <SPIMemory.h>

SPIFlash flash_5(chip_select);

inline void initializeFlash() {
  
}

#endif