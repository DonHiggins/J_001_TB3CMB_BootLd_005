Example_281xSci_Echoback.out
--outfile=Example_281xSci_Echoback.mcs
--map=MapOut.map
--intel
--image

SECTIONS
{
  codestart
  .text
  ramfuncs
  .cinit
  .econst
}

ROMS
{
   ROM1: org=0x003F8000, len=0x600, romwidth 16, fill=0x7766
}