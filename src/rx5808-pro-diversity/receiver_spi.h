#ifndef RECEIVER_SPI_H
#define RECEIVER_SPI_H


#include <stdint.h>


namespace ReceiverSpi {
  void setSynthRegisterB(uint16_t value);
  void setPowerDownRegister(uint32_t value);
};


#endif
