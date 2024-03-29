//*****************************************************************************
//
// nfc_p2p_demo_debug.h - contains debug over UART headers
//
// Copyright (c) 2014 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.0.12573 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************

#ifndef __NFC_P2P_DEBUG__
#define __NFC_P2P_DEBUG__

//*****************************************************************************
//
// Deffinitions for functions
//
//*****************************************************************************
void DebugHeader(sNDEFMessageData sNDEFMessage);
void DebugTextRecord(sNDEFTextRecord sNDEFText);
void DebugURIRecord(sNDEFURIRecord sNDEFURI);
void DebugSmartPosterRecord(sNDEFSmartPosterRecord sNDEFSmartPoster);
void DebugSignitureRecord(void);

#endif //__NFC_P2P_DEBUG__
