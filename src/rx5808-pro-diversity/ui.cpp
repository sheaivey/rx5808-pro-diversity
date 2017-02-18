#include <stdint.h>
#include <Adafruit_SSD1306.h>
#include <avr/pgmspace.h>

#include "settings.h"
#include "settings_internal.h"
#include "ui.h"


namespace Ui {
    OLED_CLASS display;
    bool shouldDrawUpdate = false;
    bool shouldDisplay = false;


    void setup() {
        display.begin(OLED_VCCSTATE, OLED_ADDRESS);

        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setTextWrap(false);

        display.clearDisplay();

        display.begin();
    }

    void update() {
        if (shouldDisplay) {
            display.display();
            shouldDisplay = false;
        }
    }


    void drawTitle(const char* title) {
        display.drawFastHLine(0, CHAR_HEIGHT + 2, SCREEN_WIDTH, WHITE);

        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.print(title);
    }

    void drawDashedHLine(
        const int x,
        const int y,
        const int w,
        const int step
    ) {
        for (int i = 0; i <= w; i += step) {
            Ui::display.drawFastHLine(x + i, y, step / 2, WHITE);
        }
    }

    void clear() {
        display.clearDisplay();
    }

    void clearRect(const int x, const int y, const int w, const int h) {
        display.fillRect(x, y, w, h, BLACK);
    }


    void needUpdate() {
        shouldDrawUpdate = true;
    }

    void needDisplay() {
        shouldDisplay = true;
    }
}
