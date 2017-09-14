to_HexC2Prog_Read_Me.txt

Working on using CodeSkin C2Prog as a vehicle for bootloading and burning flash into F2812DSP.

Running to_HexC2Prog.bat converts the TI .out file into a .hex file suitable for input to C2PROG.

As of 8/29/2014 the .hex file looks OK, I haven't used it for C2Prog yet.  I probably should get a
FLASH based application to try it with.  (Sci_Echoback is RAM based.)

Err: in H_012_F281xFLASH_005, for some reason our out and map files are named "Example_281xSci_FFDLB_int"
I'm going to find out why that is and change it in H_012_F281xFLASH_006 to be "H_012_F281xFLASH_006". Or
something that makes more sense -- FIXED: in H_012_F281xFLASH_006, changed file names to TB3CM_DSP_Code.out, etc.