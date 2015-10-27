#DIY Advanced - Custom Board
This requires ordering a board from [https://oshpark.com/shared_projects/7J17i6at](https://oshpark.com/shared_projects/7J17i6at)
No bootloader is nessissary with this board. Program the board using the SPI breakout pins. (Select Arduino Pro Mini 5v 328 as the board)

####BOM
- 4x 1K ohm resistor 0805
- 1x 470 ohm resistor 0805
- 3x 330 ohm resistor 0805
- 2x 100k ohm resistor 0805
- 2x 22pf capacitor 0805
- 1x 16.0000mhz crystal
- 2x rx5808 receiver with [spi mod](rx5808-spi-mod.md)
- 3x LEDs 0805
- 1x [Navigation switch](https://www.sparkfun.com/products/8184)
- 2x SMA Connector
- 1x 4066D SO14 digital switch chip SMD
- 1x MEGA 328 TQFP
- 1x Active Buzzer (Optional)
- 1x OLED 128x64 display I2C (5v tollerant) (Optional - If you don't want to do TV_Out)

####Schematic
![alt text](img/rx5808-pro-diversity-schematic-eagle.jpg)

####Board
Silk:
![alt text](img/rx5808-pro-diversity-schematic-board-white.jpg)
Top:
![alt text](img/rx5808-pro-diversity-monitor-top.png)
Bottom:
![alt text](img/rx5808-pro-diversity-monitor-bottom.png)



####Example
Below is an example of the board installed into a 7" monitor. This board will also fit into a 5" monitor. There are two outputs one for the currently active rx5808 module. The other is the GUI that from the ATMEGA 328 chip.

This monitor has two inputs and a source button on the front for switching between the video signals. If your monitor does not allow for two inputs you can use a toggle switch to acheive the same result.
![alt text](img/rx5808-pro-diversity-schematic-monitor-example.jpg)
