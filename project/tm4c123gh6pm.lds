/******************************************************************************
 *  Define the memory regions for this processor
 *****************************************************************************/

MEMORY
{
    FLASH (RX)        : ORIGIN = 0x00000000, LENGTH = 0x00040000
    SRAM (WX)         : ORIGIN = 0x20000000, LENGTH = 0x00008000
    SRAM_PHANTOM (WX) : ORIGIN = 0x40000000, LENGTH = 0x00008000
}
