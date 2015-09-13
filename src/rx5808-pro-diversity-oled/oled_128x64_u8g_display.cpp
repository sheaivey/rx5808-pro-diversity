#include "settings.h"

#ifdef OLED_128x64_U8G_SCREENS
#include "screens.h" // function headers
#include <U8glib.h>

#define BLACK 0
#define WHITE 1


U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0|U8G_I2C_OPT_NO_ACK|U8G_I2C_OPT_FAST);	// Fast I2C / TWI

screens::screens() {
    reset();
}

void screens::reset() {
    u8g.setFont(u8g_font_fixed_v0);
    u8g.setFontRefHeightExtendedText();
    u8g.setDefaultForegroundColor();
    u8g.setFontPosTop();
    u8g.setColorIndex(WHITE);
}

void screens::flip() {
    u8g.setRot180();
}

void screens::drawTitleBox(const char *title) {
    u8g.setColorIndex(WHITE);
    u8g.drawFrame(0, 0, u8g.getWidth(), u8g.getHeight());
    u8g.drawBox(0, 0, u8g.getWidth(), 11);
    u8g.setColorIndex(BLACK);

    // center text
    u8g.setPrintPos(((u8g.getWidth() - u8g.getStrWidth(title)) / 2),1);
    u8g.print(title);
}


void screens::mainMenu(int menu_id){
    u8g.firstPage();
    do {
      drawMainMenu(menu_id);
    } while( u8g.nextPage() );
}

void screens::drawMainMenu(int menu_id) {
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


void screens::seekMode() {
    u8g.firstPage();
    do {
      drawSeekMode();
    } while( u8g.nextPage() );
}

void screens::drawSeekMode() {
    reset(); // start from fresh screen.
    drawTitleBox("SEEK MODE");
    /*
    //display.drawRect(0, 0, display.width(), display.height(), WHITE);
    //display.fillRect(0, 0, display.width(), 11, WHITE);
    //display.drawRect(0, 10, display.width(), 11, WHITE);
    if (state == STATE_MANUAL)
    {
        time_screen_saver=millis();
        //display.setTextColor(BLACK);
        //display.setCursor(35,2);
        //display.print("MANUAL MODE");
    }
    else if(state == STATE_SEEK)
    {
        time_screen_saver=0; // dont show screen saver until we found a channel.
        //display.setCursor(25,2);
        //display.setTextColor(BLACK);
        //display.print("AUTO SEEK MODE");
    }
    //display.setTextColor(WHITE);

    //display.drawLine(0, 32, display.width(), 32, WHITE);
    //display.setCursor(5,12);
    //display.drawLine(97,11,97,20,WHITE);
    //display.print("BAND:");
    for(int i=0;i<8;i++) {
        //display.setCursor(15*i+8,23);
        //display.print((char) (i+'1'));
    }
    //display.drawLine(0, 36, display.width(), 36, WHITE);


    //display.drawLine(0, display.height()-11, display.width(), display.height()-11, WHITE);
    //display.setCursor(2,display.height()-9);
    //display.print("5645");
    //display.setCursor(55,display.height()-9);
    //display.print("5800");
    //display.setCursor(display.width()-25,display.height()-9);
    //display.print("5945");
    */

}
#endif
