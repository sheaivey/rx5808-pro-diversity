#DIY Advanced - Custom Board
This requires ordering a board from [http://www.oshpark.com](http://www.oshpark.com/)
You will also need a programmer to get the Arduino bootloader onto the MEGA328 chip. After that you can use the FTDI pins to program the chip from the Arduino IDE. (Select Arduino Pro Mini 5v 328 as the board)


####BOM
- 4x 1K ohm resistor 603
- 1x 470 ohm resistor 603
- 1x 10K ohm resistor 603
- 3x 330 ohm resistor 603
- 2x 22pf capacitor 603
- 1x 0.1uf capacitor 603
- 1x 16.0mhz crystal
- 2x rx5808 receiver with [spi mod](rx5808-spi-mod.md)
- 3x LEDs 603
- 1x Navigation switch
- 2x A/V Jack AUDIO-JACK-TRRS-SMD
- 2x U.FL Connector SMD
- 1x SWITCH-SPDT-SMD
- 1x 4066D SO14 digital switch chip SMD
- 1x MEGA 328 TQFP

####Schematic
![alt text](img/rx5808-pro-diversity-schematic-eagle.jpg)

####Board
![alt text](img/rx5808-pro-diversity-schematic-board-white.jpg)
