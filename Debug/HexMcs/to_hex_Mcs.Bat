Rem - - - - - - to_hex_Mcs.Bat - - - - - -
Rem
Rem Convert TI .out file to hex format
Rem
Rem - - - - - - - - - - - - - - - - - - - 
cd ..
copy TB3CM_BootLoad.out hexMcs\* /Y
copy TB3CM_BootLoad.map hexMcs\* /Y
cd hexMcs
C:\ti\ccsv5\tools\compiler\c2000_6.1.0\bin\hex2000 to_hex_Mcs.cmd
Rem
pause
