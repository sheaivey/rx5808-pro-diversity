#include <Arduino.h>
#include <avr/pgmspace.h>

#include "settings/settings.h"
#include "settings/eeprom_settings.h"
#include "receiver.h"
#include "channels.h"

uint8_t activeReceiver = -1;

uint8_t rssiA = 0;
uint16_t rssiARaw = 0;
#ifdef USE_DIVERSITY
    uint8_t rssiB = 0;
    uint16_t rssiBRaw = 0;
#endif

uint32_t lastChannelSwitchTime = 0;

inline void sendBit(uint8_t value);
inline void sendSlaveSelect(uint8_t value);
void updateRssiLimits();

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
// Forumla for calculating N and A is:
//    F_lo = 2 * (N * 32 + A) * (F_osc / R)
//    where:
//        F_osc = 8 Mhz
//        R = 8
//
// Refer to RTC6715 datasheet for further details.
//
void setChannel(uint8_t channel)
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

    lastChannelSwitchTime = millis();
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

void setActiveReceiver(uint8_t receiver) {
    if (receiver == activeReceiver)
        return;

    #ifdef USE_DIVERSITY
        digitalWrite(PIN_LED_A, receiver == RECEIVER_A);
        digitalWrite(PIN_LED_B, receiver == RECEIVER_B);
    #else
        digitalWrite(PIN_LED_A, HIGH);
    #endif

    activeReceiver = receiver;
}

//
// Blocks until MIN_TUNE_TIME has been reached since last channel switch.
//
void waitForStableRssi() {
    uint16_t timeSinceChannelSwitch = millis() - lastChannelSwitchTime;
    if (timeSinceChannelSwitch < MIN_TUNE_TIME) {
        delay(MIN_TUNE_TIME - timeSinceChannelSwitch);
    }
}

uint16_t updateRssi() {
    rssiARaw = 0;
    #ifdef USE_DIVERSITY
        rssiBRaw = 0;
    #endif

    for (uint8_t i = 0; i < RSSI_READS; i++) {
        rssiARaw += analogRead(PIN_RSSI_A);
        #ifdef USE_DIVERSITY
            rssiBRaw += analogRead(PIN_RSSI_B);
        #endif
    }

    rssiARaw = rssiARaw / RSSI_READS;
    #ifdef USE_DIVERSITY
        rssiBRaw = rssiBRaw / RSSI_READS;
    #endif

    updateRssiLimits();

    rssiA = map(
        rssiARaw,
        EepromSettings.rssiAMin,
        EepromSettings.rssiAMax,
        1,
        100);
    #ifdef USE_DIVERSITY
        rssiB = map(
            rssiBRaw,
            EepromSettings.rssiBMin,
            EepromSettings.rssiBMax,
            1,
            100);
    #endif
}

void updateRssiLimits() {
    // TEMP: Until logic for setting RSSI max is rewritten.

    if (rssiARaw > EepromSettings.rssiAMax)
        EepromSettings.rssiAMax = rssiARaw;

    if (rssiARaw < EepromSettings.rssiAMin)
        EepromSettings.rssiAMin = rssiARaw;

    #ifdef USE_DIVERSITY
        if (rssiBRaw > EepromSettings.rssiBMax)
            EepromSettings.rssiBMax = rssiBRaw;

        if (rssiBRaw < EepromSettings.rssiBMin)
            EepromSettings.rssiBMin = rssiBRaw;
    #endif
}

void setDiversityMode(uint8_t mode) {
    EepromSettings.diversityMode = mode;
    switchDiversity();
}

void switchDiversity() {
    static uint8_t diversityCheckTick = 0;
    uint8_t bestReceiver = activeReceiver;

    if (EepromSettings.diversityMode == DIVERSITY_AUTO) {
        uint8_t rssiDiff =
            (int) abs(((rssiA - rssiB) / (float) rssiB) * 100.0f);

        if (rssiDiff >= DIVERSITY_CUTOVER) {
            if(rssiA > rssiB && diversityCheckTick > 0)
                diversityCheckTick--;

            if(rssiA < rssiB && diversityCheckTick < DIVERSITY_MAX_CHECKS)
                diversityCheckTick++;

            // Have we reached the maximum number of checks to switch
            // receivers?
            if (diversityCheckTick == 0 ||
                diversityCheckTick >= DIVERSITY_MAX_CHECKS
            ) {
                bestReceiver =
                    (diversityCheckTick == 0) ?
                    RECEIVER_A :
                    RECEIVER_B;
            }
        }
    } else {
        switch (EepromSettings.diversityMode) {
            case DIVERSITY_FORCE_A:
                bestReceiver = RECEIVER_A;
                break;

            case DIVERSITY_FORCE_B:
                bestReceiver = RECEIVER_B;
                break;
        }
    }

    setActiveReceiver(bestReceiver);
}

void updateReceiver() {
    updateRssi();

    #ifdef USE_DIVERSITY
        switchDiversity();
    #endif
}
