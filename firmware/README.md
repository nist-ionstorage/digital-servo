# SuperLaserLand firmware

Ion Storage Group  
Time and Frequency Division  
National Institute of Standards and Technology  
Boulder, CO USA

Authors:  
David Leibrandt (david.leibrandt@nist.gov)

SuperLaserLand is open source firmware which runs on the FPGA of the NIST digital servo and implements three channels of feedback control.

This directory contains the source code which was written and compiled with the Xilinx ISE Design Suite 13.2.  It does not contain the HDL files for the Opal Kelly FrontPanel SDK, which are required to compile the source code.  This code was developed and tested with version 4.0.8 of the FrontPanel SDK.

To compile:

1. Download and install the Xilinx ISE Design Suite.  Note that the free version is not able to compile firmware for the FPGA used in the digital servo.

2. Obtain the Opal Kelly HDL files (*.v, *.vhd, *.ngc; located in the `C:\Program Files\Opal Kelly\FrontPanelUSB\FrontPanelHDL\XEM6010-LX150` folder after the FrontPanel SDK has been installed) and copy them into the `OpalKelly_HDL` folder.

3. Open the Xilinx Core Generator and generate the cores for each of the *.xco files found in the `ipcore_dir` folder.

4. Open `SuperLaserLand.xise` in the Xilinx ISE Design Suite and generate the bitfile.