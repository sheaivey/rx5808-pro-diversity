// Include display
#ifdef OLED_128x64_DISPLAY
    #include "oled_128x64_display.h" // For I2C 128x64 OLED screens
    #define SELECTED_DSIPLAY "OLED_128x64_DISPLAY"
#endif

#ifdef TV_OUT_DISPLAY
    //#include "TV_Out_display.h"// For TVOut screens
    #define SELECTED_DSIPLAY "TV_OUT_DISPLAY"
#endif
