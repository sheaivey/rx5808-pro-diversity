#Introduction
This project originated [here](https://code.google.com/p/rx5808-pro/) and has been modified to include diversity receiver menu.

The main goal of this fork was to add diversity to an already amazing feature set. Please see the following video for a quick intro of the project.

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/NwnQCUikqvI/0.jpg)](http://www.youtube.com/watch?v=NwnQCUikqvI)

## Table of Contents
1. [Features](#features)
2. [Usage](#usage)
3. [Hardware](#hardware)
4. [Software](#software)
5. [Contributing](#contributing)
6. [License](#license)


##Features
- **User control** - 3 Button navigation, up, down, select.
- **Manual Mode** - Set channel manual
- **Search Mode** - Search next channel based on RSSI
- **Scanner** - Print spectrum of all 32 channels
- **Save mode** - Save startup mode (manu, seek, scan)
- **Save channel** - Startup with last saved channel
- **Beeper** - Acustic feedback on important actions
- **Save RSSI range** - RSSI voltage may vary per part
- *(NEW)* **Quick Save** - From any home press and hold select for 1 second.
- *(NEW)* **Diversity** - Receiver select and monitor.
- *(NEW)* **Led Status** - Power, button pressed, active antenna.
- *(REMOVED)* ~~**DIP mode** - Set channel by extern DIP switch~~

####In the works...
- Auto switch video from GUI to Receiver on button press.
- OLED Version (No TVout menu system).
- New board design taking heat concerns into account.
- SPI breakout on the advanced board.
- Breakout rx5808 functionality into its own library.

##Usage
####Screens

1. **Mode Select** - Use the up/down buttons to navigate. Hit mode to select. Don't press anything to return to the last mode selected. ![alt text](docs/img/screen-menu.jpg "Mode Select")

2. **Auto Scan** - Scans all bands until a signal with good signal strength is found. ![alt text](docs/img/screen-auto-scan.jpg "Auto Scan")

4. **Band Scaner** - Scans all bands and presents them with a sitnal strength bar graph. ![alt text](docs/img/screen-band-scan.jpg "Band Scaner")

3. **Manual Mode** - Will hold on a manualy selected channel. ![alt text](docs/img/screen-manual.jpg "Manual Mode")

5. **Diversity** - Select which receiver to use or auto. Also Displays signal strength of each antenna. ![alt text](docs/img/screen-diversity.jpg "Diversity")

6. **Save** - Saves last used channel and mode for next power cycle. This is also where you enter RSSI calibration mode. ![alt text](docs/img/screen-save.jpg "Save")

####Initial Setup
When powering on for the first time it is best to calibrate your RSSI. To do this follow these steps below. (You can repeat this process as many times as needed for best results.)

1. Power on your receiver and transmitter and place them about 10 to 20 feet apart from one another. (If you are using directional antennas make sure they are pointed at your transmitter for best results.)
2. next navigate to the "Save Setup" menu.
3. Wait for 1 second then hold Mode button for 3 seconds.
4. when you release the mode button you will enter RSSI calibration screen. This will scan all channels 10 times getting the min and max RSSI strength.
5. Once all 10 scans are complete you will be returned to the last home screen. You should now be able to have accurate RSSI readings and also auto scan to active channels.

##Hardware
This project is centered around the rx5808 5.8ghz receiver module which can be found at a number of online stores.

I have modified the original schematics to incorporate the diversity setup.

I have also added LEDs to represent which antenna is currently being selected.

1. [DIY Simple - Arduino Nano](/docs/diy-arduino-nano.md)
2. [DIY Advanced - Custom Board](/docs/diy-custom-board.md)
3. [rx5808 SPI modification](/docs/rx5808-spi-mod.md)
4. [Enclosure Documentation](/docs/enclosure.md)

##Software
The major software changes are centered around the diversity menu. This required removing the dip switch mode from the origional project. Within the diversity menu you will find RSSI signal strength for each receiver and the ability to select which mode the receivers are operating in (Auto, Receiver A, Receiver B).

The diversity switching logic is simple.
```
If Receiver A is greater than Receiver B by two precent
  Select Receiver A
else
  Select Receiver B.
```

The two percent helps prevent rapid video switching when both RSSI are close in value.

##Contributing
Any contributions are welcome!

Please follow [CONTRIBUTING.md](CONTRIBUTING.md) for standard practices regarding this repo.

##License & Recognition
####Recognition
- SPI driver based on fs_skyrf_58g-main.c Written by Simon Chambers
- TVOUT by Myles Metzel
- Scanner by Johan Hermen
- Inital 2 Button version by Peter (pete1990)
- Refactored and GUI reworked by Marko Hoepken
- Universal version my Marko Hoepken
- Diversity Receiver Board and GUI improvements by Shea Ivey

####License
The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
