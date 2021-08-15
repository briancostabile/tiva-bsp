/******************************************************************************
 *
 * project_ccs.cmd - CCS linker configuration file for project.
 *
 * Copyright (c) 2013 Texas Instruments Incorporated.  All rights reserved.
 * Software License Agreement
 * 
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 * 
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the  
 *   distribution.
 * 
 *   Neither the name of Texas Instruments Incorporated nor the names of
 *   its contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * This is part of revision 1.0 of the Tiva Firmware Development Package.
 *
 *****************************************************************************/

--retain=g_pfnVectors

/* The following command line options are set as part of the CCS project.    */
/* If you are building using the command line, or for some reason want to    */
/* define them here, you can uncomment and modify these lines as needed.     */
/* If you are using CCS for building, it is probably better to make any such */
/* modifications in your CCS project and leave this file alone.              */
/*                                                                           */
--heap_size=0
--stack_size=4096
/* --library=rtsv7M3_T_le_eabi.lib                                           */


/* Section allocation in memory */
SECTIONS
{
    .vecsException : > EXCEPTION_VECTORS
    .vecsInterrupt : > INTERRUPT_VECTORS
    .text   :   > APP_FLASH
    .const  :   > APP_FLASH
    .cinit  :   > APP_FLASH
    .pinit  :   > APP_FLASH
    .init_array : > APP_FLASH
    .tinit  : LOAD = APP_FLASH,
              LOAD_START(_tinitStart),
              LOAD_END(_tinitEnd)
    .tst_str_cmd : > APP_FLASH
    .tst_str_hlp : > APP_FLASH
    .tst_str_out : > APP_FLASH

    .reset_info : type = NOLOAD > RAM_BASE
    .noinit     : type = NOLOAD > SRAM
    .data   :   > SRAM
    .bss    :   > SRAM
    .sysmem :   > SRAM
    .stack  :   > SRAM

    .vtable : type = NOLOAD > SRAM_PHANTOM
}

__STACK_TOP = __stack + 4096;
