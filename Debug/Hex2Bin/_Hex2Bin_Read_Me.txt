_Hex2Bin_Read_Me.txt

Running the to_Hex2Bin.Bat file produces a .bin file suitable for sending to the DSP ROM Bootloader via RS232. (Don't forget to type an "A" character first to set the autobaud.)

First to_Hex2Bin.Bat copies the TI COFF (linked output) .out file into this directory.  Then it runs the command line utility, HEX2000.exe to produce an Intel MCS format (ascii) file containing all the program code and initialized data sections, along with header info necessary for the ROM bootloader.

Then we use Hex2Bin.exe to do a simple ascii to binary conversion, yielding the .bin file to send to the bootloader.

I'm not real fond of this version of Hex2Bin.exe, although for now it does the job.  Literature suggests that other Hex2Bins are publicly available on the internet (ie at sourceforge.com).

Err: in H_012_F281xFLASH_005, for some reason our out and map files are named "Example_281xSci_FFDLB_int"
I'm going to find out why that is and change it in H_012_F281xFLASH_006 to be "H_012_F281xFLASH_006". Or
something that makes more sense. -- FIXED in _006, changed file names to TB3CM_DSP_Code.out, etc.