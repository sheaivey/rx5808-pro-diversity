#Custom Boot LOGO
You can add a custom logo at the boot of the board using a BMP image and the LcdAssistant ([DOWNLOAD Here](http://en.radzio.dxp.pl/bitmap_converter/ "LCD Assistant Page"))

The Bmp Image require:
- Image size: 128x64 Pixel
- Color bit: 1 bit (Black and White)
- Bmp format

The step to create the Boot logo are this:

STEP 1: Create a Black and White Bmp image with size 128x64.

![Original IMAGE](docs/img/LOGO%20LCD.bmp "Original BMP")

STEP 2: Launch the LCDASSISTANT program, load the BMP and set the parameter: Byte Orientation - Horizontal; Size - 128x64; Size Endianness - Little;

![LCDASSISTANT Setup](docs/img/LCD Assist Setup.JPG "LCD Assistant Setup")

STEP 3: Save the Output as Text File and open it

Step 4: copies the text between the braces to the Arduino Boot_Logo.h tab

![Copy the HEX](docs/img/Copy TEXT LCD2.JPG "Copy the HEX to Arduino")

STEP 5: Uncomment the line "#define USE_CUSTOM_BOOT_LOGO" in "Settings.h"

STEP 6: Compile and upload

Congratulations... now you have your boot logo!
