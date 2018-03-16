to_HexC2Prog_Read_Me.txt

Working on using CodeSkin C2Prog as a vehicle for bootloading and burning flash into F2812DSP.

Running to_HexC2Prog.bat converts the TI .out file into a .hex file suitable for input to C2PROG.

As of 8/29/2014 the .hex file looks OK, I haven't used it for C2Prog yet.  I probably should get a
FLASH based application to try it with.  (Sci_Echoback is RAM based.)

As of 9/26/2017 the .hex file works for CAN bootloader project, loads into flash sectors FLASHA and FLASHB. 