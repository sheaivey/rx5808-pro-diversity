#include <Arduino.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "receiver.h"
#include "channels.h"

inline void sendBit(uint8_t value);
inline void sendSlaveSelect(uint8_t value);

// Sends SPI command to receiver module to change frequency.
//
// Format is LSB first, with the following bits in order:
//     4 bits - address
//     1 bit  - read/write enable
//    20 bits - data
//
// Address for frequency select (Synth Register B) is 0x1
// Expected data is (LSB):
//     7 bits - A counter divider ratio
//      1 bit - seperator
//    12 bits - N counter divder ratio
//
// Forumla for calculating N and A is:
//    F_lo = 2 * (N * 32 + A) * (F_osc / R)
//    where:
//        F_osc = 8 Mhz
//        R = 8
//
// Refer to RTC6715 datasheet for further details.
void setChannelModule(uint8_t channel)
{
  sendSlaveSelect(LOW);

  // Clock in address (0x1)
  sendBit(HIGH);
  sendBit(LOW);
  sendBit(LOW);
  sendBit(LOW);

  // Enable write.
  sendBit(HIGH);

  // Send channel data.
  uint16_t channelData = pgm_read_word_near(channelTable + channel);
  for (uint8_t i = 0; i < 16; i++) {
    sendBit(channelData & 0x1);
    channelData >>= 1;
  }

  // Remaining bits are blank.
  for (uint8_t i = 0; i < 4; i++)
    sendBit(LOW);

  // Finished clocking data in
  sendSlaveSelect(HIGH);
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

inline void sendSlaveSelect(uint8_t value)
{
  digitalWrite(PIN_SPI_SLAVE_SELECT, value);
  delayMicroseconds(1);
}
