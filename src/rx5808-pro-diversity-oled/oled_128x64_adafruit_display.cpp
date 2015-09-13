#include "settings.h"

#ifdef OLED_128x64_ADAFRUIT_SCREENS
#include "screens.h" // function headers
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <SPI.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if !defined SSD1306_128_64
#error("Screen incorrect, please fix Adafruit_SSD1306.h!");
#endif


screens::screens() {
}

void screens::begin() {
    // Set the address of your OLED Display.
    // 128x64 ONLY!!
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D or 0x3C (for the 128x64)
    // init done
    display.clearDisplay();   // clears the screen and buffer
    display.display();
}

void screens::reset() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
}

void screens::flip() {
    display.setRotation(3);
}

void screens::drawTitleBox(const char *title) {
    display.drawRect(0, 0, display.width(), display.height(),WHITE);
    display.fillRect(0, 0, display.width(), 11,WHITE);

    display.setTextSize(1);
    display.setTextColor(BLACK);
    // center text
    display.setCursor(((display.width() - (strlen(title)*6)) / 2),2);
    display.print(title);
    display.setTextColor(WHITE);
}


void screens::mainMenu(char menu_id){
    drawMainMenu(menu_id);
    display.display();
}

void screens::drawMainMenu(char menu_id) {
    reset(); // start from fresh screen.
    drawTitleBox("MODE SELECTION");

    display.fillRect(0, 10*menu_id+12, display.width(), 10,WHITE);

    display.setTextColor(menu_id == 0 ? BLACK : WHITE);
    display.setCursor(5,10*0+13);
    display.print("AUTO SEARCH");
    display.setTextColor(menu_id == 1 ? BLACK : WHITE);
    display.setCursor(5,10*1+13);
    display.print("BAND SCANNER");
    display.setTextColor(menu_id == 2 ? BLACK : WHITE);
    display.setCursor(5,10*2+13);
    display.print("MANUAL MODE");

#ifdef USE_DIVERSITY
    display.setTextColor(menu_id == 3 ? BLACK : WHITE);
    display.setCursor(5,10*3+13);
    display.print("DIVERSITY");
#endif
    display.setTextColor(menu_id == 4 ? BLACK : WHITE);
    display.setCursor(5,10*4+13);
    display.print("SAVE SETUP");

}


void screens::seekMode(char state) {
    drawSeekMode(state);
    display.display();
}

void screens::drawSeekMode(char state) {
    reset(); // start from fresh screen.
    if (state == STATE_MANUAL)
    {
        drawTitleBox("MANUAL MODE");
    }
    else if(state == STATE_SEEK)
    {
        drawTitleBox("AUTO SEEK MODE");
    }
    display.setTextColor(WHITE);
    display.drawLine(0, 21, display.width(), 21, WHITE);
    display.drawLine(0, 32, display.width(), 32, WHITE);
    display.setCursor(5,12);
    display.drawLine(97,11,97,20,WHITE);
    display.print("BAND:");
    for(int i=0;i<8;i++) {
        display.setCursor(15*i+8,23);
        display.print((char) (i+'1'));
    }
    display.drawLine(0, 36, display.width(), 36, WHITE);
    display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    display.setCursor(2,display.height()-9);
    display.print("5645");
    display.setCursor(55,display.height()-9);
    display.print("5800");
    display.setCursor(display.width()-25,display.height()-9);
    display.print("5945");
}


void screens::bandScanMode(char state) {
    drawBandScanMode(state);
    display.display();
}

void screens::drawBandScanMode(char state) {
    reset(); // start from fresh screen.
    if(state==STATE_SCAN)
    {
        drawTitleBox("BAND SCANNER");
        display.setCursor(5,12);
        display.print("BEST:");
    }
    else
    {
        drawTitleBox("RSSI SETUP");
        display.setCursor(5,12);
        display.print("Min:     Max:");
    }
    display.drawLine(0, 21, display.width(), 21, WHITE);

    display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    display.setCursor(2,display.height()-9);
    display.print("5645");
    display.setCursor(55,display.height()-9);
    display.print("5800");
    display.setCursor(display.width()-25,display.height()-9);
    display.print("5945");
}
void screens::updateBandScanMode(char state, char channel, char rssi, int channelName, int channelFrequency, int rssi_setup_min_a, int rssi_setup_max_a) {
    updateBandScanMode(state, channel, rssi, channelName, channelFrequency, rssi_setup_min_a, rssi_setup_max_a);
    display.display();
}
void screens::drawUpdateBandScanMode(char state, char channel, char rssi, int channelName, int channelFrequency, int rssi_setup_min_a, int rssi_setup_max_a) {
    rssi=map(rssi, 1, 100, 1, 30);
    int hight = (display.height()-12-rssi);
    display.fillRect((channel*3)+4,display.height()-12-30,3,30-rssi,BLACK);
    display.fillRect((channel*3)+4,hight,3,rssi,WHITE);

    display.setTextColor(WHITE,BLACK);
    display.setCursor(36,12);
    display.print(channelName, HEX);
    display.setCursor(52,12);
    display.print(channelFrequency);
}


#endif
