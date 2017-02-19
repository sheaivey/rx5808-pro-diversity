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

        const uint8_t xScaler = w / dataSize;
        const uint8_t xScalarMissing = w % dataSize;

        for (uint8_t i = 0; i < dataSize - 1; i++) {
            const uint8_t dataPoint = CLAMP_DATAPOINT(data[i]);
            const uint8_t dataPointNext = CLAMP_DATAPOINT(data[i + 1]);

            // Need to invert the heights so it shows the right way on the
            // screen.
            const uint8_t dataPointHeight = h - SCALE_DATAPOINT(dataPoint);
            const uint8_t dataPointNextHeight =
                h - SCALE_DATAPOINT(dataPointNext);

            Ui::display.drawLine(
                x + (i) * xScaler,
                y + dataPointHeight,
                x + (i + 1) * xScaler
                    + (i == dataSize - 2 ? xScalarMissing : 0),
                y + dataPointNextHeight,
                WHITE
            );
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
        for (int i = 0; i <= w; i += step) {
            Ui::display.drawFastHLine(x + i, y, step / 2, WHITE);
        }
    }

    void drawDashedVLine(
        const int x,
        const int y,
        const int h,
        const int step
    ) {
        for (int i = 0; i <= h; i += step) {
            Ui::display.drawFastVLine(x, y + i, step / 2, INVERSE);
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
