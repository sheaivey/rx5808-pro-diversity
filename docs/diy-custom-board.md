#DIY Advanced - Custom Board
This requires ordering a board from [https://oshpark.com/shared_projects/A27Y4Afh](https://oshpark.com/shared_projects/A27Y4Afh)
You will also need a programmer to get the Arduino bootloader onto the MEGA328 chip. After that you can use the FTDI pins to program the chip from the Arduino IDE. (Select Arduino Pro Mini 5v 328 as the board)

####BOM
- 4x 1K ohm resistor 603
- 1x 470 ohm resistor 603
- 1x 10K ohm resistor 603
- 3x 330 ohm resistor 603
- 2x 22pf capacitor 603
- 1x 0.1uf capacitor 603
- 1x 16.0000mhz crystal
- 2x rx5808 receiver with [spi mod](rx5808-spi-mod.md)
- 3x LEDs 603
- 1x Navigation switch
- 2x SMA Connector
- 1x SWITCH-SPDT-SMD
- 1x 4066D SO14 digital switch chip SMD
- 1x MEGA 328 TQFP

####Schematic
![alt text](img/rx5808-pro-diversity-schematic-eagle.jpg)

####Board
![alt text](img/rx5808-pro-diversity-schematic-board-white.jpg)

####Example
Below is an example of the board installed into a 7" monitor. This board will also fit into a 5" monitor. There are two outputs one for the currently active rx5808 module. The other is the GUI that from the ATMEGA 328 chip.

This monitor has two inputs and a source button on the front for switching between the video signals. If your monitor does not allow for two inputs you can use a toggle switch to acheive the same result.
![alt text](img/rx5808-pro-diversity-schematic-monitor-example.jpg)
