#include <Arduino.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "receiver.h"
#include "channels.h"

inline void sendBit(uint8_t value);
inline void toggleSpi(uint8_t value);

void setChannelModule(uint8_t channel)
{
  uint8_t i;
  uint16_t channelData;

  channelData = pgm_read_word_near(channelTable + channel);

  // bit bash out 25 bits of data
  // Order: A0-3, !R/W, D0-D19
  // A0=0, A1=0, A2=0, A3=1, RW=0, D0-19=0
  toggleSpi(HIGH);
  delayMicroseconds(1);
  toggleSpi(LOW);

  sendBit(LOW);
  sendBit(LOW);
  sendBit(LOW);
  sendBit(HIGH);

  sendBit(LOW);

  // remaining zeros
  for (i = 20; i > 0; i--)
    sendBit(LOW);

  // Clock the data in
  toggleSpi(HIGH);
  delayMicroseconds(1);
  toggleSpi(LOW);

  // Second is the channel data from the lookup table
  // 20 bytes of register data are sent, but the MSB 4 bits are zeros
  // register address = 0x1, write, data0-15=channelData data15-19=0x0
  toggleSpi(HIGH);
  toggleSpi(LOW);

  // Register 0x1
  sendBit(HIGH);
  sendBit(LOW);
  sendBit(LOW);
  sendBit(LOW);

  // Write to register
  sendBit(HIGH);

  // D0-D15
  //   note: loop runs backwards as more efficent on AVR
  for (i = 16; i > 0; i--)
  {
    // Is bit high or low?
    if (channelData & 0x1)
    {
      sendBit(HIGH);
    }
    else
    {
      sendBit(LOW);
    }

    // Shift bits along to check the next one
    channelData >>= 1;
  }

  // Remaining D16-D19
  for (i = 4; i > 0; i--)
    sendBit(LOW);

  // Finished clocking data in
  toggleSpi(HIGH);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_SLAVE_SELECT, LOW);
  digitalWrite(PIN_SPI_CLOCK, LOW);
  digitalWrite(PIN_SPI_DATA, LOW);
}


inline void sendBit(uint8_t value)
{
  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_DATA, value);
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_CLOCK, HIGH);
  delayMicroseconds(1);

  digitalWrite(PIN_SPI_CLOCK, LOW);
  delayMicroseconds(1);
}

inline void toggleSpi(uint8_t value)
{
  delayMicroseconds(1);
  digitalWrite(PIN_SPI_SLAVE_SELECT, value);
  delayMicroseconds(1);
}
