#OLED Screen Shots
![alt text](img/screens-oled.jpg)

Below is a quick video demonstration the performance of the I2C OLED display.
[![Video](http://img.youtube.com/vi/I4wZWitvHXw/0.jpg)](http://www.youtube.com/watch?v=I4wZWitvHXw)

#Source Code
[Code](../src/rx5808-pro-diversity/rx5808-pro-diversity.ino)
You will need the following Adafruit libraries:
```
Adafruit_SSD1306.h
Adafruit_GFX.h
```

In the settings.h file uncomment the following line OLED_128x64_ADAFRUIT_SCREENS
```
// Choose the display you will be using
// you will also have to uncomment the includes in the main project.
//#define TVOUT_SCREENS
#define OLED_128x64_ADAFRUIT_SCREENS
```
NOTE: only one screen can be use at a time.

In the main project uncomment the following lines
```
// uncomment depending on the display you are using.
// this is an issue with the arduino preprocessor
#ifdef TVOUT_SCREENS
//    #include <TVout.h>
//    #include <fontALL.h>
#endif
#ifdef OLED_128x64_ADAFRUIT_SCREENS
    #include <Adafruit_SSD1306.h>
    #include <Adafruit_GFX.h>
    #include <Wire.h>
    #include <SPI.h>
#endif
```

Make sure the you have selected the correct screen size in Adafruit_SSD1306.h
```
#define SSD1306_128_64
//   #define SSD1306_128_32
//   #define SSD1306_96_16
```

You may need to change the following line to be the correct I2C address for your OLED display. Found in
[here](../src/rx5808-pro-diversity/oled_128x64_adafruit_screens.cpp)
```
display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
```

#OLED Diversity - Arduino Nano

####BOM
- 3x 1k resistor
- 3x 330 resistor
- 2x rx5808 receiver with [spi mod](rx5808-spi-mod.md)
- 3x LEDs
- 3x Momentary switch
- 1x Toggle switch
- 1x Buzzer (optional)
- 1x 4066 digital switch chip
- 1x Arduino Nano 3.0 328
- 1x OLED 128x64 display I2C (5v tollerant)

####Schematic
![alt text](img/rx5808-pro-diversity-schematic-simple-oled.jpg)

#OLED Basic - Arduino Nano (Not Diversity)
You can comment out the following line to remove the diversity menu from the GUI. (Note: dip mode is still not implemented.)
```
#define USE_DIVERSITY
```

####BOM
- 3x 1k resistor
- 1x 330 resistor (optional)
- 1x rx5808 receiver with [spi mod](rx5808-spi-mod.md)
- 1x LEDs (optional)
- 3x Momentary switch
- 1x Toggle switch
- 1x Buzzer (optional)
- 1x Arduino Nano 3.0 328
- 1x OLED 128x64 display I2C (5v tollerant)

####Schematic
![alt text](img/rx5808-pro-original-schematic-oled.jpg)
