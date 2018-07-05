#include <stdint.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include <TVout.h>
#include <fontALL.h>

#include "settings.h"
#include "settings_internal.h"
#include "ui.h"

namespace Ui {
    #ifdef OLED_128x64_ADAFRUIT_SCREENS
      OLED_CLASS display;
    #endif 
    #ifdef TVOUT_SCREENS
      TVout display;
    #endif
    
    bool shouldDrawUpdate = false;
    bool shouldDisplay = false;
    bool shouldFullRedraw = false;

    void setup() {
        #ifdef OLED_128x64_ADAFRUIT_SCREENS
          display.begin(OLED_VCCSTATE, OLED_ADDRESS);
  
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setTextWrap(false);
  
          display.clearDisplay();
  
          display.begin();
        #endif
        #ifdef TVOUT_SCREENS
          display.begin(PAL,128,64);
          display.select_font(font8x8);
          display.clear_screen();
        #endif
    }

    void update() {
        if (shouldDisplay) {
              #ifdef OLED_128x64_ADAFRUIT_SCREENS
               display.display();
              #endif 
              #ifdef TVOUT_SCREENS
              #endif
            shouldDisplay = false;
        }
    }


    void drawGraph(
        const uint8_t data[],
        const uint8_t dataSize,
        const uint8_t dataScale,
        const uint8_t x,
        const uint8_t y,
        const uint8_t w,
        const uint8_t h
    ) {
        #define SCALE_DATAPOINT(p) (p * h / dataScale)
        #define CLAMP_DATAPOINT(p) \
            (p > dataScale) ? dataScale : ((p < 0) ? 0 : p);

        Ui::clearRect(x, y, w - 1, h + 1);

        const uint8_t xScaler = w / (dataSize - 1);
        const uint8_t xScalarMissing = w - (xScaler * (dataSize - 1));

        uint8_t xNext = x;

        for (uint8_t i = 0; i < dataSize - 1; i++) {
            const uint8_t dataPoint = CLAMP_DATAPOINT(data[i]);
            const uint8_t dataPointNext = CLAMP_DATAPOINT(data[i + 1]);

            // Need to invert the heights so it shows the right way on the
            // screen.
            const uint8_t dataPointHeight = h - SCALE_DATAPOINT(dataPoint);
            const uint8_t dataPointNextHeight =
                h - SCALE_DATAPOINT(dataPointNext);

            const uint8_t xEnd = xNext + xScaler
                    + (i == 0 || i == dataSize - 2 ? (xScalarMissing + 1) / 2 : 0);
              #ifdef OLED_128x64_ADAFRUIT_SCREENS
                display.drawLine(
                    xNext,
                    y + dataPointHeight,
                    xEnd,
                    y + dataPointNextHeight,
                    WHITE
                );
              #endif
              #ifdef TVOUT_SCREENS
                display.draw_line(
                    xNext,
                    y + dataPointHeight,
                    xEnd,
                    y + dataPointNextHeight,
                    WHITE
                );
              #endif
            xNext = xEnd;
        }

        #undef SCALE_DATAPOINT
        #undef CLAMP_DATAPOINT
    }


    void drawDashedHLine(
        const int x,
        const int y,
        const int w,
        const int step
    ) {
        for (int i = 0; i < w; i += step) {
          #ifdef OLED_128x64_ADAFRUIT_SCREENS
            display.drawFastHLine(x + i, y, step / 2, WHITE);
          #endif
          #ifdef TVOUT_SCREENS
            display.draw_row(y, x + i, x + i + (step / 2), WHITE);
          #endif
        }
    }

    void drawDashedVLine(
        const int x,
        const int y,
        const int h,
        const int step
    ) {
        for (int i = 0; i < h; i += step) {
          #ifdef OLED_128x64_ADAFRUIT_SCREENS
            display.drawFastVLine(x, y + i, step / 2, INVERSE);
          #endif
          #ifdef TVOUT_SCREENS
            display.draw_column(x, y + i, y + i + (step / 2), WHITE);
          #endif
        }
    }

    void drawFastHLine(
        const int x,
        const int y,
        const int w,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawFastHLine(x, y, w, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_row(y, x, x+w, color);
      #endif
    }

    void drawFastVLine(
        const int x,
        const int y,
        const int h,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawFastVLine(x, y, h, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_column(x, y, y+h, color);
      #endif
    }

    void drawRoundRect(
        const int x,
        const int y,
        const int w,
        const int h,
        const int r,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawRoundRect(x, y, w, h, r, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_rect(x, y, w, h, color);
      #endif
    }

    void fillRect(
        const int x,
        const int y,
        const int w,
        const int h,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.fillRect(x, y, w, h, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_rect(x, y, w, h, color, color);
      #endif
    }

    void fillTriangle(
        const int x0,
        const int y0,
        const int x1,
        const int y1,
        const int x2,
        const int y2,
        const int color
    ) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.fillTriangle(
          x0, y0, x1, y1, x2, y2, color
        );
      #endif
      #ifdef TVOUT_SCREENS
        for (int x=x0; x<=x2; x++) {
          display.draw_line(x,y0,x1,y1,color);
        }
      #endif
    }

    void drawBitmap(const int x, const int y, const unsigned char *image, const int w, const int h, const int color) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.drawBitmap(x, y, image, x, h, color);
      #endif
      #ifdef TVOUT_SCREENS
        display.bitmap(x, y, image, 0, w, h);
        if (color == BLACK) {
          display.draw_rect(x, y, w, h,INVERT,INVERT);
        }
      #endif
    }
    
    void setTextColor(const int color) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.setTextColor(color);
      #endif
      #ifdef TVOUT_SCREENS
        // No equivalent function
      #endif
    }
    
    void setTextSize(const int size) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.setTextSize(size); 
      #endif
      #ifdef TVOUT_SCREENS
      //TODO add larger font
        if (size == 1) {
          display.select_font(font4x6);
        }
        else if (size == 2) {
          display.select_font(font6x8);
        }
        else if (size == 3) {
          display.select_font(font8x8);
        }
        else if (size == 4) {
          display.select_font(font8x8);
        }
        else if (size == 5) {
          display.select_font(font8x8);
        }
        else if (size == 6) {
          display.select_font(font8x8);
        }
        else {
          display.select_font(font4x6);
        }        
      #endif
    }
    
    void setCursor(const int x, const int y) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.setCursor(x,y);
      #endif
      #ifdef TVOUT_SCREENS
        display.set_cursor(x,y);
      #endif
    }

    // TODO move print here
    void print(const char text) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
//        display.print(text);
      #endif
      #ifdef TVOUT_SCREENS
//        display.print(text);
      #endif
    }
          
    void clear() {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.clearDisplay();
      #endif
      #ifdef TVOUT_SCREENS
        display.clear_screen();
      #endif
    }

    void clearRect(const int x, const int y, const int w, const int h) {
      #ifdef OLED_128x64_ADAFRUIT_SCREENS
        display.fillRect(x, y, w, h, BLACK);
      #endif
      #ifdef TVOUT_SCREENS
        display.draw_rect(x, y, w, h,BLACK,BLACK);
      #endif
    }
          
    void needUpdate() {
        shouldDrawUpdate = true;
    }

    void needDisplay() {
        shouldDisplay = true;
    }

    void needFullRedraw() {
        shouldFullRedraw = true;
    }
}
