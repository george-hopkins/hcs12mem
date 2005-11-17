MEMORY
{
  page0 (rwx)  : ORIGIN = 0x0000, LENGTH = 0x0000
  text  (rx)   : ORIGIN = 0x3c00, LENGTH = 0x0400
  data         : ORIGIN = 0x4000, LENGTH = 0x0000
  vectors (rx) : ORIGIN = 0xffc0, LENGTH = 0x0040
  eeprom       : ORIGIN = 0x0800, LENGTH = 0x0800
}

PROVIDE (_stack = 0x4000);
PROVIDE (_io = 0x0000);
PROVIDE (_eeprom = 0x0800);
