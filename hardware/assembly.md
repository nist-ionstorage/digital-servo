Assembly instructions for the digital-servo box. 

## enclosure continutity
For safety reasons all sides of enclosure must be shorted to GND on power supply cable. 

## grounding of enclosure electrical connectors
All connector shields are shorted to enclosure except JP1 and JP2 which use isolated SMA.

## LED wiring
JP18, JP18, JP19 are labeled with GRN, GND, RED which corresponds to
the wire colors of the specified LEDs.

## AUX connector wiring 
AUX0 and AUX1 are used for expansion options. 
- AUX0 is routed to JP9. 
- AUX1 is routed to JP10.

## power connectors
Refer to the photo [here](https://github.com/nist-ionstorage/digital-servo/blob/master/doc/Figs/DigitalServoOutsideWithLabels.pdf). 

- For "Low voltage power supply input" and "Low voltage power supply
output" match wiring on connector from power supply brick 
(AEH45UM31 or [PCM50UT04](http://datasheet.octopart.com/PCM50UT04-XP-Power-datasheet-3173.pdf)).

- +5, -12, +12, GNDs from "Low voltage power supply input" are
permanently shorted to +5, -12, +12, GNDs on "Low voltage power supply
output".

- +68V on "High goltage power supply input" is permanently shorted to
"High voltage power supply output".

## power switch
Power Switch connects/disconnects -12, +12, +5, +68 to JP3, JP4 and the fans.

## wire color choice
If practical duplicate the wire color choice as in 
[this](https://github.com/nist-ionstorage/digital-servo/blob/master/doc/Figs/DigitalServoInsideWithLabels.pdf) photo. 

## wire gauge
Current for all power supplies is <= 5.0 A. Based on [this]
(http://assets.bluesea.com/files/resources/newsletter/images/DC_wire_selection_chartlg.jpg)
chart <= 16 AWG is sufficient. Use multi-strand wires throughout the
design (no mono-filament).
