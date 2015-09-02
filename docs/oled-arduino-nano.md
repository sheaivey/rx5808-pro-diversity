#OLED Screen Shots
![alt text](img/screens-oled.jpg)

#Source Code
[Enclosure Documentation](../src/rx5808-pro-diversity-oled/rx5808-pro-diversity-oled.ino)
You will need the following Adafruit libraries:
```
Adafruit_SSD1306.h
Adafruit_GFX.h
```

Make sure the you have selected the correct screen size in Adafruit_SSD1306.h
```
#define SSD1306_128_64
//   #define SSD1306_128_32
//   #define SSD1306_96_16
```

You may need to change the following line to be the correct I2C address for your OLED display.
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
