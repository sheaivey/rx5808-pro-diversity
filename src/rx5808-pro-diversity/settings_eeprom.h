#ifndef EEPROM_SETTINGS_H
#define EEPROM_SETTINGS_H

#include <stdint.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "settings_internal.h"
#include "receiver.h"

struct EepromSettings {
    uint32_t magic;
    uint8_t channel;
    uint8_t defaultState;

    uint16_t rssiAMin;
    uint16_t rssiAMax;

    uint8_t beepEnabled;
    uint8_t orderByChannel;

    #ifdef USE_DIVERSITY
    uint8_t diversityMode;
    uint16_t rssiBMin;
    uint16_t rssiBMax;
    #endif

    #ifdef USE_VOLTAGE_MONITORING
    uint8_t vbatScale;
    uint8_t vbatWarning;
    uint8_t vbatCritical;
    #endif

    void load();
    void save();
    void initDefaults();
};

PROGMEM const struct {
    uint32_t magic = EEPROM_MAGIC;
    uint8_t channel = 0;
    uint8_t defaultState = 0;

    uint16_t rssiAMin = RSSI_MIN_VAL;
    uint16_t rssiAMax = RSSI_MAX_VAL;

    uint8_t beepEnabled = true;
    uint8_t orderByChannel = true;

    #ifdef USE_DIVERSITY
    uint8_t diversityMode = RECEIVER_AUTO;
    uint16_t rssiBMin = RSSI_MIN_VAL;
    uint16_t rssiBMax = RSSI_MAX_VAL;
    #endif

    #ifdef USE_VOLTAGE_MONITORING
    uint8_t vbatScale = VBAT_SCALE;
    uint8_t vbatWarning = WARNING_VOLTAGE;
    uint8_t vbatCritical = CRITICAL_VOLTAGE;
    #endif
} EepromDefaults;

extern struct EepromSettings EepromSettings;

#endif
