#include <string.h>
#include <EEPROM.h>

#include "settings.h"
#include "settings_internal.h"
#include "settings_eeprom.h"

struct EepromSettings EepromSettings;

void EepromSettings::load() {
    EEPROM.get(0, *this);

    if (this->magic != EEPROM_MAGIC)
        this->initDefaults();
}

void EepromSettings::save() {
    EEPROM.put(0, *this);
}

void EepromSettings::initDefaults() {
    memcpy_P(this, &EepromDefaults, sizeof(EepromDefaults));
    this->save();
}
