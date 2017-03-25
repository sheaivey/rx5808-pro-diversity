#include <Arduino.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "settings_eeprom.h"
#include "receiver.h"
#include "receiver_spi.h"
#include "channels.h"

#include "timer.h"

static void updateRssiLimits();
static void writeSerialData();


namespace Receiver {
    uint8_t activeReceiver = RECEIVER_A;
    uint8_t activeChannel = 0;

    uint8_t rssiA = 0;
    uint16_t rssiARaw = 0;
    uint8_t rssiALast[RECEIVER_LAST_DATA_SIZE] = { 0 };
    #ifdef USE_DIVERSITY
        uint8_t rssiB = 0;
        uint16_t rssiBRaw = 0;
        uint8_t rssiBLast[RECEIVER_LAST_DATA_SIZE] = { 0 };

        uint8_t diversityTargetReceiver = activeReceiver;
        Timer diversityHysteresisTimer = Timer(DIVERSITY_HYSTERESIS_PERIOD);
    #endif

    uint32_t lastChannelSwitchTime = 0;
    static Timer rssiLogTimer = Timer(RECEIVER_LAST_DELAY);
    #ifdef USE_SERIAL_OUT
        static Timer serialLogTimer = Timer(25);
    #endif


    void setChannel(uint8_t channel)
    {
        ReceiverSpi::setSynthRegisterB(Channels::getSynthRegisterB(channel));

        lastChannelSwitchTime = millis();
        activeChannel = channel;
    }

    void setActiveReceiver(uint8_t receiver) {
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
        waitForStableRssi();

        analogRead(PIN_RSSI_A); // Fake read to let ADC settle.
        rssiARaw = analogRead(PIN_RSSI_A);
        #ifdef USE_DIVERSITY
            analogRead(PIN_RSSI_B);
            rssiBRaw = analogRead(PIN_RSSI_B);
        #endif

        rssiA = constrain(
            map(
                rssiARaw,
                EepromSettings.rssiAMin,
                EepromSettings.rssiAMax,
                0,
                100
            ),
            0,
            100
        );
        #ifdef USE_DIVERSITY
            rssiB = constrain(
                map(
                    rssiARaw,
                    EepromSettings.rssiBMin,
                    EepromSettings.rssiBMax,
                    0,
                    100
                ),
                0,
                100
            );
        #endif

        if (rssiLogTimer.hasTicked()) {
            for (uint8_t i = 0; i < RECEIVER_LAST_DATA_SIZE - 1; i++) {
                rssiALast[i] = rssiALast[i + 1];
                #ifdef USE_DIVERSITY
                    rssiBLast[i] = rssiBLast[i + 1];
                #endif
            }

            rssiALast[RECEIVER_LAST_DATA_SIZE - 1] = rssiA;
            #ifdef USE_DIVERSITY
                rssiBLast[RECEIVER_LAST_DATA_SIZE - 1] = rssiB;
            #endif

            rssiLogTimer.reset();
        }
    }

    void setDiversityMode(uint8_t mode) {
        EepromSettings.diversityMode = mode;
        switchDiversity();
    }

    void switchDiversity() {
        uint8_t nextReceiver = activeReceiver;

        if (EepromSettings.diversityMode == DIVERSITY_AUTO) {
            int8_t rssiDiff = (int8_t) rssiA - (int8_t) rssiB;
            uint8_t rssiDiffAbs = abs(rssiDiff);
            uint8_t currentBestReceiver = activeReceiver;

            if (rssiDiff > 0) {
                currentBestReceiver = RECEIVER_A;
            } else if (rssiDiff < 0) {
                currentBestReceiver = RECEIVER_B;
            } else {
                currentBestReceiver = activeReceiver;
            }

            if (rssiDiffAbs >= DIVERSITY_HYSTERESIS) {
                if (currentBestReceiver == diversityTargetReceiver) {
                    if (diversityHysteresisTimer.hasTicked()) {
                        nextReceiver = diversityTargetReceiver;
                    }
                } else {
                    diversityTargetReceiver = currentBestReceiver;
                    diversityHysteresisTimer.reset();
                }
            } else {
                diversityHysteresisTimer.reset();
            }
        } else {
            switch (EepromSettings.diversityMode) {
                case DIVERSITY_FORCE_A:
                    nextReceiver = RECEIVER_A;
                    break;

                case DIVERSITY_FORCE_B:
                    nextReceiver = RECEIVER_B;
                    break;
            }
        }

        setActiveReceiver(nextReceiver);
    }

    void setup() {
        #ifdef DISABLE_AUDIO
            ReceiverSpi::setPowerDownRegister(0b00010000110111110011);
        #endif
    }

    void update() {
        updateRssi();

        #ifdef USE_SERIAL_OUT
            writeSerialData();
        #endif

        #ifdef USE_DIVERSITY
            switchDiversity();
        #endif
    }
}


#ifdef USE_SERIAL_OUT

#include "pstr_helper.h"

static void writeSerialData() {
    if (Receiver::serialLogTimer.hasTicked()) {
        Serial.print(Receiver::activeChannel, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiA, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiARaw, DEC);
        Serial.print(PSTR2("\t"));
        Serial.print(Receiver::rssiB, DEC);
        Serial.print(PSTR2("\t"));
        Serial.println(Receiver::rssiBRaw, DEC);

        Receiver::serialLogTimer.reset();
    }
}
#endif
