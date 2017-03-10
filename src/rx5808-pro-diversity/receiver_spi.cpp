#include <Arduino.h>
#include <stdint.h>

#include "receiver_spi.h"
#include "settings.h"


static inline void sendBit(uint8_t value);
static inline void sendBits(uint32_t bits, uint8_t count = 20);
static inline void sendSlaveSelect(uint8_t value);
static inline void sendRegister(uint8_t address, uint32_t data);


#define SPI_ADDRESS_SYNTH_A 0x01
#define SPI_ADDRESS_POWER 0x0A


namespace ReceiverSpi {
    //
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
    // Forumla for calculating N and A is:/
    //    F_lo = 2 * (N * 32 + A) * (F_osc / R)
    //    where:
    //        F_osc = 8 Mhz
    //        R = 8
    //
    // Refer to RTC6715 datasheet for further details.
    //
    void setSynthRegisterB(uint16_t value) {
        sendRegister(SPI_ADDRESS_SYNTH_A, value);
    }

    void setPowerDownRegister(uint32_t value) {
        sendRegister(SPI_ADDRESS_POWER, value);
    }
}


static inline void sendRegister(uint8_t address, uint32_t data) {
    sendSlaveSelect(LOW);

    sendBits(address, 4);
    sendBit(HIGH); // Enable write.

    sendBits(data, 20);

    // Finished clocking data in
    sendSlaveSelect(HIGH);
    digitalWrite(PIN_SPI_CLOCK, LOW);
    digitalWrite(PIN_SPI_DATA, LOW);
}


static inline void sendBits(uint32_t bits, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        sendBit(bits & 0x1);
        bits >>= 1;
    }
}

static inline void sendBit(uint8_t value) {
    digitalWrite(PIN_SPI_CLOCK, LOW);
    delayMicroseconds(1);

    digitalWrite(PIN_SPI_DATA, value);
    delayMicroseconds(1);
    digitalWrite(PIN_SPI_CLOCK, HIGH);
    delayMicroseconds(1);

    digitalWrite(PIN_SPI_CLOCK, LOW);
    delayMicroseconds(1);
}

static inline void sendSlaveSelect(uint8_t value) {
    digitalWrite(PIN_SPI_SLAVE_SELECT, value);
    delayMicroseconds(1);
}
