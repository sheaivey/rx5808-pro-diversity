#Introduction
This project originated at [here](https://code.google.com/p/rx5808-pro/) and has been modified to include diversity menu.

The main goal of this branch was to add diversity to the feature set. Please see the following video for a quick intro into the project.

[![IMAGE ALT TEXT HERE](http://img.youtube.com/vi/YOUTUBE_VIDEO_ID_HERE/0.jpg)](http://www.youtube.com/watch?v=YOUTUBE_VIDEO_ID_HERE)

1. [Features](#Features)
2. [Hardware](#Hardware)
3. [Software](#Software)
4. [Usage](#Usage)
5. [Contributing](#Contributing)
6. [License](#License)


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

##Hardware
This project is centered around the rx5808 5.8ghz receiver module which can be found at a number of online stores.

I have modified the origional schematics to incorperate the diversity setup.

I have also added LEDs to represent which antenna is currently being selected.

1. [DIY Arduino Nano Hardware Documentation]()
2. [Oshpark and BOM]()
3. [rx5808 SPI modification]()
4. [Enclosure Documentation]()

##Software
The major software changes are centered around the diversity menu. This required removing the Dipswitch mode from the origional project. Within the diversity menu you will find RSSI signal strength for each receiver and the abuility to select which mode the receivers are operating in (Auto, Receiver A, Receiver B).

The diversity switching logic is simple.
```
If Receiver A is greater than Receiver B by two precent
  Select Receiver A
else
  Select Receiver B.
```

The two percent helps pervent rapid video switching when both RSSI are close in value.

##Usage

####Initial Setup
When powering on for the first time it is best to calibrate your RSSI. To do this follow these steps below. (You can repeat this process as many times as needed for best results.)

1. Power on your receiver and transmitter and place them about 10 to 20 feet apart from one another. (If you are using directional antennas make sure they are pointed at your transmitter for best results.)
2. next navigate to the "Save Setup" menu.
3. Wait for 1 second then hold Mode button for 3 seconds.
4. when you release the mode button you will enter RSSI calibration screen. This will scan all channels 10 times getting the min and max RSSI strength.
5. Once all 10 scans are complete you will be returned to the last home screen. You should now be able to have accurate RSSI readings and also auto scan to active channels.

##Contributing

##License
