Rem - - - - - - to_hex2Bin.Bat - - - - - -
Rem
Rem Convert TI .out file to hex format
Rem
Rem - - - - - - - - - - - - - - - - - - - 
cd ..
copy TB3CM_BootLoad.out hex2bin\* /Y
copy TB3CM_BootLoad.map hex2bin\* /Y
cd hex2bin
C:\ti\ccsv5\tools\compiler\c2000_6.1.0\bin\hex2000 to_Hex2Bin.cmd
Rem
pause
C:\tidcs\c28\spraaq2_SCI_Flash\Hex2Bin.exe TB3CM_BootLoad.Hex
pause
