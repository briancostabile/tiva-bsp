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
 * @file svc_Nvm.c
 * @brief Contains the API to save data in NVM. Keeping it very simple. Only
 *        one writer, using address 0
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Pragma.h"
#include "bsp_Crc.h"
#include "bsp_Eeprom.h"
#include "svc_Nvm.h"


// Ensure Data read/written is 4-byte aligned
BSP_ATTR_ALIGNMENT(4) svc_Nvm_Data_t svc_Nvm_data;

// Ensure size is a multiple of 4
#define SVC_NVM_DATA_SIZE32 ((sizeof(svc_Nvm_Data_t) + 3) / 4)
#define SVC_NVM_DATA_SIZE (SVC_NVM_DATA_SIZE32 * 4)

// start 256 bytes into EEPROM
#define SVC_NVM_DATA_ADDR 0x00000100

/*==============================================================================
 *                              Local Functions
 *============================================================================*/
/*============================================================================*/
void
svc_Nvm_init( void )
{
    // Read out structure from NVM
    bsp_Eeprom_read( (uint32_t *)&svc_Nvm_data,
                     SVC_NVM_DATA_ADDR,
                     SVC_NVM_DATA_SIZE32 );

    bool initError = false;

    // Check CRC
    uint32_t crc32 = bsp_Crc_32p04C11DB7( &((uint32_t*)&svc_Nvm_data)[1],
                                          (SVC_NVM_DATA_SIZE32 - 1) );

    initError |= (((svc_Nvm_DataHdr_t*)&svc_Nvm_data)->crc != crc32);
    initError |= (svc_Nvm_validData(&svc_Nvm_data) == false);

    // If invalid then initialize with product specific init data
    if( initError )
    {
        svc_Nvm_setHdr( &svc_Nvm_data );
        svc_Nvm_initData( &svc_Nvm_data );
        svc_Nvm_save();
    }

    return;
}

/*============================================================================*/
void
svc_Nvm_save( void )
{
    svc_Nvm_setHdr( &svc_Nvm_data );
    ((svc_Nvm_DataHdr_t*)&svc_Nvm_data)->crc = bsp_Crc_32p04C11DB7( &((uint32_t*)&svc_Nvm_data)[1],
                                                (SVC_NVM_DATA_SIZE32 - 1) );

    bsp_Eeprom_write( SVC_NVM_DATA_ADDR,
                      (uint32_t *)&svc_Nvm_data,
                      SVC_NVM_DATA_SIZE );
    return;
}
