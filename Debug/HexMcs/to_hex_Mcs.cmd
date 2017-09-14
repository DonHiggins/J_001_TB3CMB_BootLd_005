TB3CM_BootLoad.out
--outfile=TB3CM_BootLoad.Mcs
--map=MapOut.map
--intel
--romwidth=16
--image

SECTIONS
{
  Flash28_API 
  codestart
  .text
  ramfuncs
  .econst
  .cinit
  .switch
  csm_rsvd
  csmpasswds
}

ROMS
{
   ROM1: org=0x003F4000, len=0x4000, romwidth 16, fill=0xFFFF
}