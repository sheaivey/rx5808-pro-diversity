/*
  screens.h
  by Shea Ivey
*/

#ifndef screens_h
#define screens_h

class display
{
    private: // u8g helper functions for screens.
        void reset();
        void drawMainMenu(int menu_id);
        void drawTitleBox(const char *title);

    public:
        display();
        void mainMenu(int menu_id);
        void flip();
};

#endif
