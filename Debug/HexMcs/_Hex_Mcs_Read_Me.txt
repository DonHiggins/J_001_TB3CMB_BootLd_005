_Hex_Mcs_Read_Me.txt

- - - - - 8/22/2017 - - - - - - - -

1) Have to pay attention to the filename used for .map and .out files created by the Code Composer Studio project.  The name is set somewhere inside the properties interface for CCS.  Here we use "TB3CM_BootLoad.out" and .map.  We have to make sure our to_hex_Mcs.bat and .cmd files call for those files.

2) In to_hex_Mcs.cmd there is a "SECTIONS" directive.  I just added ".switch" and "Flash28_API" sections so that it would pull those into it's output file.  You can look into the TB3CM_BootLoad.map file to see what sections are loaded into flash.

3) I have reserved Flash segments FLASHA & B, 0x4000 words for the Bootloader.  In the .cmd file I defined the combined segment as "FLASHAB."  The main program will be located in FLASHC, D, E, etc.

4) Added an .asm module to define "FF" values for CSM passwords and "00" values for block of FLASH at 0x3F7F80 per recommendations for Code Security Module.


- - - - - 8/29/2014 - - - - - - - -

Running to_hex_Mcs.bat creates a readable (Intel MCS format -- .mcs) ROM image of all
the loadable code and data sections from the example project -- the Sci Echoback application.

The mcs-format file, .mcs, was useful for viewing the object image, when I was
comparing it with the .boot (binary) image in the HexBoot file directory.  I had to 
use a hex editor to view the .boot file, and I was checking to make sure it contained
all the data plus proper headers to represent the code segments from the COFF .out file.

The command file, to_hex_Mcs.cmd,  uses a ROMS directive which identifies the 
target memory range starting at 0x003F8000 for a length of 0x600 words (a number 
I picked as longer than necessary to accommodate the sample project.  It uses 
0x7766 as a fill value so you can easily see in the .mcs file what is loadable
program/memory and what is fill.

Err: in H_012_F281xFLASH_005, for some reason our out and map files are named "Example_281xSci_FFDLB_int"
I'm going to find out why that is and change it in H_012_F281xFLASH_006 to be "H_012_F281xFLASH_006". Or
something that makes more sense. -- FIXED in H_012_F281xFLASH_006, files named TB3CM_DSP_Code.out, etc,
TB3CM_BootLoad.out, TB3CM_BootLoad.map

Err: in H_012_F281xFLASH_005, it looks like the defined PROM address range doesn't include the current code address range -- have to fix this if we ever want to use it.

8/21/2017 -- changed .cmd file to start at 0x3F4000, for a length of 0x4000, eg FLASHA & FLASHB segments

8/22/2017 -- changed .cmd file to include sections "Flasg28-API", and ".switch"
             and set fill=0x7777