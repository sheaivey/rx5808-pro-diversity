#ifndef UI_H
#define UI_H


#include <Adafruit_SSD1306.h>

#include "settings.h"
#include "settings_internal.h"


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define SCREEN_WIDTH_MID SCREEN_WIDTH / 2
#define SCREEN_HEIGHT_MID SCREEN_HEIGHT / 2

#define CHAR_WIDTH 5
#define CHAR_HEIGHT 7

#define TITLE_HEIGHT 9


namespace Ui {
    extern OLED_CLASS display;
    extern bool shouldDrawUpdate;
    extern bool shouldDisplay;

    void setup();
    void update();

    void drawTitle(const char* title);
    void clear();
    void clearRect(int x, int y, int w, int h);

    void needUpdate();
    void needDisplay();

    class UiComponent {
        public:
            virtual void drawInitial();
            virtual void drawUpdate();
    };
}

#endif
