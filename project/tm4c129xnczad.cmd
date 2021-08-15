/******************************************************************************
 * Define the memory regions for this processor
 *****************************************************************************/

/* The starting address of the application.  Normally the interrupt vectors  */
/* must be located at the beginning of the application.                      */
#define FLASH_BASE 0x00000000
#define RAM_BASE   0x20000000

MEMORY
{
    /* Application stored in and executes from internal flash */
    EXCEPTION_VECTORS (RX) : origin = FLASH_BASE, length = 0x00000040
    INTERRUPT_VECTORS (RX) : origin = 0x00000040, length = 0x0000022C
    APP_FLASH (RX)         : origin = 0x0000026C, length = 0x000FFD94
    /* Application uses internal RAM for data */
    SRAM (RWX)             : origin = RAM_BASE,   length = 0x00040000
    SRAM_PHANTOM (RWX)     : origin = 0x40000000, length = 0x00008000
}
