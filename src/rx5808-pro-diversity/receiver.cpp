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
    ReceiverId activeReceiver = ReceiverId::A;
    uint8_t activeChannel = 0;

    uint8_t rssiA = 0;
    uint16_t rssiARaw = 0;
    uint8_t rssiALast[RECEIVER_LAST_DATA_SIZE] = { 0 };
    #ifdef USE_DIVERSITY
        uint8_t rssiB = 0;
        uint16_t rssiBRaw = 0;
        uint8_t rssiBLast[RECEIVER_LAST_DATA_SIZE] = { 0 };

        ReceiverId diversityTargetReceiver = activeReceiver;
        Timer diversityHysteresisTimer = Timer(DIVERSITY_HYSTERESIS_PERIOD);
    #endif

    static Timer rssiStableTimer = Timer(MIN_TUNE_TIME);
    static Timer rssiLogTimer = Timer(RECEIVER_LAST_DELAY);
    #ifdef USE_SERIAL_OUT
        static Timer serialLogTimer = Timer(25);
    #endif


    void setChannel(uint8_t channel)
    {
        ReceiverSpi::setSynthRegisterB(Channels::getSynthRegisterB(channel));

        rssiStableTimer.reset();
        activeChannel = channel;
    }

    void setActiveReceiver(ReceiverId receiver) {
        #ifdef USE_DIVERSITY
            #ifdef USE_DIVERSITY_FAST_SWITCHING
                uint8_t targetPin, disablePin;
                if (receiver == ReceiverId::A) {
                    targetPin = PIN_LED_A;
                    disablePin = PIN_LED_B;
                } else {
                    targetPin = PIN_LED_B;
                    disablePin = PIN_LED_A;
                }

                uint8_t port = digitalPinToPort(targetPin);
                uint8_t targetBit = digitalPinToBitMask(targetPin);
                uint8_t disablebit = digitalPinToBitMask(disablePin);
                volatile uint8_t *out = portOutputRegister(port);

                *out = (*out | targetBit) & ~disablebit;
            #else
                digitalWrite(PIN_LED_A, receiver == ReceiverId::A);
                digitalWrite(PIN_LED_B, receiver == ReceiverId::B);
            #endif
        #else
            digitalWrite(PIN_LED_A, HIGH);
        #endif

        activeReceiver = receiver;
    }

    bool isRssiStable() {
        return rssiStableTimer.hasTicked();
    }

    uint16_t updateRssi() {
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
                    rssiBRaw,
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

#ifdef USE_DIVERSITY
    void setDiversityMode(DiversityMode mode) {
        EepromSettings.diversityMode = mode;
        switchDiversity();
    }

    void switchDiversity() {
        ReceiverId nextReceiver = activeReceiver;

        if (EepromSettings.diversityMode == DiversityMode::AUTO) {
            int8_t rssiDiff = (int8_t) rssiA - (int8_t) rssiB;
            uint8_t rssiDiffAbs = abs(rssiDiff);
            ReceiverId currentBestReceiver = activeReceiver;

            if (rssiDiff > 0) {
                currentBestReceiver = ReceiverId::A;
            } else if (rssiDiff < 0) {
                currentBestReceiver = ReceiverId::B;
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
                case DiversityMode::FORCE_A:
                    nextReceiver = ReceiverId::A;
                    break;

                case DiversityMode::FORCE_B:
                    nextReceiver = ReceiverId::B;
                    break;
            }
        }

        setActiveReceiver(nextReceiver);
    }
#endif

    void setup() {
        #ifdef DISABLE_AUDIO
            ReceiverSpi::setPowerDownRegister(0b00010000110111110011);
        #endif
    }

    void update() {
        if (rssiStableTimer.hasTicked()) {
            updateRssi();

            #ifdef USE_SERIAL_OUT
                writeSerialData();
            #endif

            #ifdef USE_DIVERSITY
                switchDiversity();
            #endif
        }
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
