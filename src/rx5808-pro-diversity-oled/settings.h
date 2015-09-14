#ifndef settings_h
#define settings_h

// Choose the display you will be using
#define OLED_128x64_ADAFRUIT_SCREENS
//#define TVOUT_SCREENS
// u8glib has performance issues.
//#define OLED_128x64_U8G_SCREENS

// this will be displayed on the screensaver.
#define CALL_SIGN "CALL SIGN"

// Feature Togglels
//#define DEBUG
#define USE_DIVERSITY


#define spiDataPin 10
#define slaveSelectPin 11
#define spiClockPin 12

// Receiver PINS
#define receiverA_led A0
#define rssiPinA A2

#define useReceiverA 1

#ifdef USE_DIVERSITY
    // Diversity
    #define receiverB_led A1
    #define rssiPinB A3
    #define useReceiverAuto 0
    #define useReceiverB 2
    // rssi strenth should be 2% greater than other receiver before switch.
    // this pervents flicker when rssi values are close and delays diversity checks counter.
    #define DIVERSITY_CUTOVER 2
    // number of checks a receiver needs to win over the other to switch receivers.
    // this pervents rapid switching.
    // 1 to 10 is a good range. 1 being fast switching, 10 being slow 100ms to switch.
    #define DIVERSITY_MAX_CHECKS 5
#endif

// this two are minimum required
#define buttonUp 2
#define buttonMode 3
// optional comfort buttons
#define buttonDown 4
#define buttonSave 5
// Buzzer
#define buzzer 6

// key debounce delay in ms
// NOTE: good values are in the range of 100-200ms
// shorter values will make it more reactive, but may lead to double trigger
#define KEY_DEBOUNCE 200

#define led 13
// number of analog rssi reads to average for the current check.
#define RSSI_READS 50
// RSSI default raw range
#define RSSI_MIN_VAL 90
#define RSSI_MAX_VAL 300
// 75% threshold, when channel is printed in spectrum
#define RSSI_SEEK_FOUND 75
// 80% under max value for RSSI
#define RSSI_SEEK_TRESHOLD 80
// scan loops for setup run
#define RSSI_SETUP_RUN 10

#define STATE_SEEK_FOUND 0
#define STATE_SEEK 1
#define STATE_SCAN 2
#define STATE_MANUAL 3
#ifdef USE_DIVERSITY
    #define STATE_DIVERSITY 4
#endif
#define STATE_SAVE 5
#define STATE_RSSI_SETUP 6
#define STATE_SCREEN_SAVER 7

#define START_STATE STATE_SEEK
#define MAX_STATE STATE_MANUAL

#define CHANNEL_BAND_SIZE 8
#define CHANNEL_MIN_INDEX 0
#define CHANNEL_MAX_INDEX 39

#define CHANNEL_MAX 39
#define CHANNEL_MIN 0

#define EEPROM_ADR_STATE 0
#define EEPROM_ADR_TUNE 1
#define EEPROM_ADR_RSSI_MIN_A_L 2
#define EEPROM_ADR_RSSI_MIN_A_H 3
#define EEPROM_ADR_RSSI_MAX_A_L 4
#define EEPROM_ADR_RSSI_MAX_A_H 5
#ifdef USE_DIVERSITY
    #define EEPROM_ADR_DIVERSITY 6
    #define EEPROM_ADR_RSSI_MIN_B_L 7
    #define EEPROM_ADR_RSSI_MIN_B_H 8
    #define EEPROM_ADR_RSSI_MAX_B_L 9
    #define EEPROM_ADR_RSSI_MAX_B_H 10
#endif

#endif // file_defined
