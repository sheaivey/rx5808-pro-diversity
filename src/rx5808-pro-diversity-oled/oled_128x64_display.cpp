#include <U8glib.h>

#include "settings.h"
#include "oled_128x64_display.h"
#define BLACK 0
#define WHITE 1

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI

display::display() {
    reset();
}

void display::reset() {
    u8g.setFont(u8g_font_fixed_v0);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
    u8g.setColorIndex(WHITE);
}

void display::flip() {
    u8g.setRot180();
}

void display::drawTitleBox(const char *title) {
    u8g.setColorIndex(WHITE);
    u8g.drawFrame(0, 0, u8g.getWidth(), u8g.getHeight());
    u8g.drawBox(0, 0, u8g.getWidth(), 11);
    u8g.setColorIndex(BLACK);

    // center text
    u8g.setPrintPos(((u8g.getWidth() - u8g.getStrWidth(title)) / 2),1);
    u8g.print(title);
}


void display::mainMenu(int menu_id){
    u8g.firstPage();
    do {
      drawMainMenu(menu_id);
    } while( u8g.nextPage() );
}

void display::drawMainMenu(int menu_id) {
    reset(); // start from fresh screen.
    drawTitleBox("MODE SELECTION");
    u8g.setColorIndex(WHITE);
    u8g.drawBox(0, 10*menu_id+12, u8g.getWidth(), 10);

    u8g.setColorIndex(menu_id == 0 ? BLACK : WHITE);
    u8g.setPrintPos(5,10*0+12);
    u8g.print("AUTO SEARCH");
    u8g.setColorIndex(menu_id == 1 ? BLACK : WHITE);
    u8g.setPrintPos(5,10*1+12);
    u8g.print("BAND SCANNER");
    u8g.setColorIndex(menu_id == 2 ? BLACK : WHITE);
    u8g.setPrintPos(5,10*2+12);
    u8g.print("MANUAL MODE");

#ifdef USE_DIVERSITY
    u8g.setColorIndex(menu_id == 3 ? BLACK : WHITE);
    u8g.setPrintPos(5,10*3+12);
    u8g.print("DIVERSITY");
#endif
    u8g.setColorIndex(menu_id == 4 ? BLACK : WHITE);
    u8g.setPrintPos(5,10*4+12);
    u8g.print("SAVE SETUP");
}
