/**
 * Copyright 2017 Brian Costabile
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
 * @file bsp_I2c.h
 * @brief Contains types and prototypes to access the I2C
 */
#ifndef BSP_I2CMASTER_H
#define BSP_I2CMASTER_H

#include "inc/hw_memmap.h"
#include "inc/hw_i2c.h"

/*==============================================================================
 *                                Defines
 *============================================================================*/
#define BSP_I2C_PLATFORM_NUM 4

/*==============================================================================
 *                                 Types
 *============================================================================*/
/*============================================================================*/
#define BSP_I2C_ID0 ((bsp_I2c_Id_t)0)
#define BSP_I2C_ID1 ((bsp_I2c_Id_t)1)
#define BSP_I2C_ID2 ((bsp_I2c_Id_t)2)
#define BSP_I2C_ID3 ((bsp_I2c_Id_t)3)
typedef uint8_t bsp_I2c_Id_t;

/*============================================================================*/
/**
 * @brief Enumerated value for clock Speed
 * Standard  - 100Kbps
 * Fast      - 400Kbps
 * Fast-Plus - 1Mbps
 * High      - 3.3Mbps
 */
#define BSP_I2C_SPEED_STANDARD  ((bsp_I2c_Id_t)0)
#define BSP_I2C_SPEED_FAST      ((bsp_I2c_Id_t)1)
#define BSP_I2C_SPEED_FAST_PLUS ((bsp_I2c_Id_t)2)
#define BSP_I2C_SPEED_HIGH      ((bsp_I2c_Id_t)3)
typedef uint8_t bsp_I2c_Speed_t;


/*============================================================================*/
#define BSP_I2C_CONTROL_DISABLE  ((bsp_I2c_Control_t)0)
#define BSP_I2C_CONTROL_ENABLE   ((bsp_I2c_Control_t)1)
typedef uint8_t bsp_I2c_Control_t;

/*============================================================================*/
#define BSP_I2C_TRANS_TYPE_WRITE      ((bsp_I2c_TransType_t)0)
#define BSP_I2C_TRANS_TYPE_READ       ((bsp_I2c_TransType_t)1)
#define BSP_I2C_TRANS_TYPE_WRITE_READ ((bsp_I2c_TransType_t)2)
typedef uint8_t bsp_I2c_TransType_t;

/*============================================================================*/
typedef uint8_t bsp_I2c_Addr_t;

typedef uint8_t bsp_I2c_Status_t;

/*==============================================================================
 *                                  Types
 *============================================================================*/
/*============================================================================*/
typedef void (*bsp_I2c_MasterTransCallback_t)( bsp_I2c_Status_t status, void* usrData );
typedef void (*bsp_I2c_SlaveTransCallback_t)( bsp_I2c_Status_t status, void* usrData );

typedef struct bsp_I2c_MasterTrans_n
{
    struct bsp_I2c_MasterTrans_n* nextPtr;
    bsp_I2c_TransType_t           type;
    bsp_I2c_Speed_t               speed;
    bsp_I2c_Addr_t                addr;
    size_t                        len;
    uint8_t*                      buffer;
    bsp_I2c_MasterTransCallback_t callback;
    void*                         usrData;
} bsp_I2c_MasterTrans_t;

/*==============================================================================
 *                            Public Functions
 *============================================================================*/
/*============================================================================*/
void
bsp_I2c_init( void );

/*============================================================================*/
void
bsp_I2c_masterControl( bsp_I2c_Id_t      id,
                       bsp_I2c_Control_t control );

/*============================================================================*/
void
bsp_I2c_masterTransQueue( bsp_I2c_Id_t           id,
                          bsp_I2c_MasterTrans_t* transPtr );

/*============================================================================*/
void
bsp_I2c_slaveConfig( bsp_I2c_Id_t    id,
                     bsp_I2c_Speed_t speed,
                     bsp_I2c_Addr_t  addr );

/*============================================================================*/
size_t
bsp_I2c_slaveRcv( bsp_I2c_Id_t id,
                  size_t       len,
                  void*        data );

/*============================================================================*/
void
bsp_I2c_slaveControl( bsp_I2c_Id_t      id,
                      bsp_I2c_Control_t control );

/*============================================================================*/
void
bsp_I2c_interruptHandler0( void );

/*============================================================================*/
void
bsp_I2c_interruptHandler1( void );

/*============================================================================*/
void
bsp_I2c_interruptHandler2( void );

/*============================================================================*/
void
bsp_I2c_interruptHandler3( void );

#endif
