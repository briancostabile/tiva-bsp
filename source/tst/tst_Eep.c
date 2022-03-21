/**
 * Copyright 2021 Brian Costabile
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/*============================================================================*/
/**
 * @file tst_Eep.c
 * @brief Test functions to read/write arbitrary EEPROM regions
 */

#include "bsp_Types.h"
#include "bsp_Eeprom.h"
#include "tst.h"
#include "tst_Eep.h"
#include "osapi.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

/*==============================================================================
 *                                 Globals
 *============================================================================*/
/*============================================================================*/
// Define the number of bytes to dump per line. Too many bytes may need to increase
// the serial device buffers
#define DUMP_BYTES_PER_LINE 16

// Define a sleep time in ms between line dumps. This allows the serial device to
// empty buffers. Current time is tuned for 9600bps UART
#define TST_EEP_DUMP_LINE_DELAY_MS 130

typedef void (*tst_Eep_DumpHexHandler_t)(void *addr);

// String globals
TST_STR_CMD(TST_EEP_READ32_STR_CMD, "read32");
TST_STR_HLP(TST_EEP_READ32_STR_HLP, "Read Memory region <start address> %x <length in longs> %d");
TST_STR_CMD(TST_EEP_WRITE32_STR_CMD, "write32");
TST_STR_HLP(TST_EEP_WRITE32_STR_HLP, "Write long to Memory <address> %x <value> %8x");
TST_STR_CMD(TST_EEP_FILL32_STR_CMD, "fill32");
TST_STR_HLP(
    TST_EEP_FILL32_STR_HLP,
    "Fill Memory <start address> %x <32-bit value> %8x <length in longs> %d");

TST_STR_OUT(TST_EEP_ERROR_STR_CMD, "Error: Expecting %d parameters" NL);
TST_STR_OUT(TST_EEP_ADDR_ERROR_STR_CMD, "Error: EEPROM Address 0x%08X must be 32-bit boundary" NL);
TST_STR_OUT(TST_EEP_READ_STR_CMD, "Reading %d-bits from Address:0x%08X Length:%d" NL);
TST_STR_OUT(TST_EEP_FILL_STR_CMD, "Filling Address:0x%08X Value:0x%0X Length:%d" NL);
TST_STR_OUT(TST_EEP_WRITE_STR_CMD, "Writing %d-bits to Address:0x%08X Value:0x%0X" NL);

/*==============================================================================
 *                            Private Functions
 *============================================================================*/
/*============================================================================*/
static tst_Status_t
tst_Eep_addrValParams(int argc, char **argv, uint32_t *addr_ptr, uint32_t *val_ptr)
{
    if (argc < 2) {
        printf(TST_EEP_ERROR_STR_CMD, 2);
        return (TST_STATUS_ERROR);
    }
    *addr_ptr = (uint32_t)strtol(argv[0], NULL, 16);
    *val_ptr  = (uint32_t)strtol(argv[1], NULL, 16);

    if ((*addr_ptr & 0x3) != 0) {
        printf(TST_EEP_ADDR_ERROR_STR_CMD, (unsigned int)(*addr_ptr));
        return (TST_STATUS_ERROR);
    }

    return (TST_STATUS_OK);
}

/*============================================================================*/
static tst_Status_t
tst_Eep_addrLenParams(int argc, char **argv, uint32_t *addr_ptr, uint32_t *len_ptr)
{
    if (argc < 2) {
        printf(TST_EEP_ERROR_STR_CMD, 2);
        return (TST_STATUS_ERROR);
    }
    *addr_ptr = (uint32_t)strtol(argv[0], NULL, 16);
    *len_ptr  = (uint32_t)strtol(argv[1], NULL, 10);

    if ((*addr_ptr & 0x3) != 0) {
        printf(TST_EEP_ADDR_ERROR_STR_CMD, (unsigned int)(*addr_ptr));
        return (TST_STATUS_ERROR);
    }

    return (TST_STATUS_OK);
}

/*============================================================================*/
static tst_Status_t tst_Eep_addrValLenParams(
    int       argc,
    char **   argv,
    uint32_t *addr_ptr,
    uint32_t *val_ptr,
    uint32_t *len_ptr)
{
    if (argc < 3) {
        printf(TST_EEP_ERROR_STR_CMD, 3);
        return (TST_STATUS_ERROR);
    }
    *addr_ptr = (uint32_t)strtol(argv[0], NULL, 16);
    *val_ptr  = (uint32_t)strtol(argv[1], NULL, 16);
    *len_ptr  = (uint32_t)strtol(argv[2], NULL, 10);

    if ((*addr_ptr & 0x3) != 0) {
        printf(TST_EEP_ADDR_ERROR_STR_CMD, (unsigned int)(*addr_ptr));
        return (TST_STATUS_ERROR);
    }

    return (TST_STATUS_OK);
}

/*============================================================================*/
static void tst_Eep_dumpAscii(uint8_t *addr)
{
    uint8_t line_idx;
    for (line_idx = 0; line_idx < DUMP_BYTES_PER_LINE; line_idx++) {
        char c = addr[line_idx];
        if (isalpha((int)c) || isdigit((int)c)) {
            printf("%c", c);
        }
        else {
            putc('.', stdout);
        }
    }
    return;
}

/*============================================================================*/
static void tst_Eep_dumpHex32(void *addr)
{
    uint8_t  line_idx;
    uint32_t eepAddr = (uint32_t)addr;
    for (line_idx = 0; line_idx < DUMP_BYTES_PER_LINE; line_idx += 4) {
        uint32_t tmp;
        bsp_Eeprom_read(&tmp, eepAddr, 1);
        eepAddr += 4;
        printf("%08X ", (unsigned int)tmp);
    }
    return;
}

/*============================================================================*/
static void tst_Eep_dumpMem(void *addr, uint32_t len, tst_Eep_DumpHexHandler_t dumpHexHandler)
{
    uint32_t i = 0;
    while (i < len) {
        dumpHexHandler(addr);
        printf("    ");
        tst_Eep_dumpAscii((uint8_t *)addr);
        printf(NL);
        i += DUMP_BYTES_PER_LINE;
        addr = (void *)((uint32_t)addr + DUMP_BYTES_PER_LINE);

        // sleep enough time to ship out 100 bytes at 9600bps
        osapi_Thread_sleep(TST_EEP_DUMP_LINE_DELAY_MS);
    }
    return;
}

/*============================================================================*/
static tst_Status_t tst_Eep_read32(int argc, char **argv)
{
    uint32_t addr;
    uint32_t len;

    if (tst_Eep_addrLenParams(argc, argv, &addr, &len) != TST_STATUS_OK) {
        return (TST_STATUS_ERROR);
    }

    printf(TST_EEP_READ_STR_CMD, 32, (unsigned int)addr, (int)len);
    tst_Eep_dumpMem((void *)addr, (len * 4), tst_Eep_dumpHex32);
    return (TST_STATUS_OK);
}

/*============================================================================*/
static tst_Status_t tst_Eep_write32(int argc, char **argv)
{
    uint32_t addr;
    uint32_t val;

    if (tst_Eep_addrValParams(argc, argv, &addr, &val) != TST_STATUS_OK) {
        return (TST_STATUS_ERROR);
    }

    printf(TST_EEP_WRITE_STR_CMD, 32, (unsigned int)addr, (int)val);
    bsp_Eeprom_write(addr, &val, 1);

    return (TST_STATUS_OK);
}

/*============================================================================*/
static tst_Status_t tst_Eep_fill32(int argc, char **argv)
{
    uint32_t addr;
    uint32_t val;
    uint32_t len;

    if (tst_Eep_addrValLenParams(argc, argv, &addr, &val, &len) != TST_STATUS_OK) {
        return (TST_STATUS_ERROR);
    }

    printf(TST_EEP_FILL_STR_CMD, (unsigned int)addr, (unsigned int)val, (int)len);

    for (uint32_t i = 0; i < len; i++) {
        bsp_Eeprom_write(addr, &val, 1);
        addr += 4;
    }

    return (TST_STATUS_OK);
}

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
// Helper macro to cleanup the table
#define TST_EEP_CMD(_uname, _lname) \
    TST_HANDLER_ELEMENT(TST_EEP_##_uname##_STR_CMD, TST_EEP_##_uname##_STR_HLP, tst_Eep_##_lname)

/*============================================================================*/
/* clang-format off */ 
const tst_TableElement_t tst_Eep_menu[] = {
    TST_EEP_CMD(READ32,  read32),
    TST_EEP_CMD(WRITE32, write32),
    TST_EEP_CMD(FILL32,  fill32),
    TST_END_ELEMENT};
/* clang-format on */
