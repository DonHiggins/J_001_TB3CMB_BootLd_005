Rem - - - - - - to_HexC2Prog.Bat - - - - - -
Rem
Rem Convert TI .out file to hex format
Rem
Rem - - - - - - - - - - - - - - - - - - - 
Rem
Rem
Rem
Rem - - - - - - - - - - - - - - - - - - -
Rem
Rem (1) copy .out and .map files into this folder
Rem     assumes "this folder" named "HexC2Prog"
Rem     assumes .map and .out files are named "TB3CM_BootLoad"
Rem     assumes .out and .map files reside in next higher level folder.
Rem     typically CCS leaves .map and .out in {proj}\Debug\
Rem     and we assume "this" ("HexC2Prog") folder is located in {proj}\Debug\
Rem 
Rem - - - - - - - - - - - - - - - - - - - 
cd ..
copy TB3CM_BootLoad.out HexC2Prog\* /Y
copy TB3CM_BootLoad.map HexC2Prog\* /Y
cd HexC2Prog
Rem
Rem
Rem
Rem - - - - - - - - - - - - - - - - - - -
Rem
Rem (2) Run hex2000 to create the properly formatted .hex file
Rem     for C2Prog. See: C2Prog_readme_v_1_7.txt
Rem
Rem     Where is hex2000.exe ?
Rem     On my CCS5 system in the Lab it is
Rem        C:\ti\ccsv5\tools\compiler\c2000_6.1.0\bin\hex2000
Rem     For my CCS7 system in my office I found in in  
Rem        C:\ti_CCS7\ccsv7\tools\compiler\ti-cgt-c2000_16.9.3.LTS\bin\hex2000
Rem
Rem - - - - - - - - - - - - - - - - - - - 
Rem
C:\ti_CCS7\ccsv7\tools\compiler\ti-cgt-c2000_16.9.3.LTS\bin\hex2000 -romwidth 16 -memwidth 16 -i -O TB3CM_BootLoad.hex TB3CM_BootLoad.out
Rem
Rem
Rem
pause


