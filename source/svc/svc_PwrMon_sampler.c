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
 * @file svc_PwrMon_sampler.c
 * @brief Contains functions for the Power Manager Sampling engine
 */
#include "bsp_Types.h"
#include "bsp_Platform.h"
#include "bsp_Mcu.h"
#include "bsp_Assert.h"
#include "bsp_TimerGp.h"
#include "svc_PwrMon_channel.h"
#include "svc_PwrMon_sampler.h"
#include "svc_EhId.h"
#include "dev_PwrMon.h"
#include "svc_MsgFwk.h"
#include "svc_PwrMonEh.h"

#ifndef SVC_LOG_LEVEL
#define SVC_LOG_LEVEL SVC_LOG_LEVEL_INFO
#endif
#include "svc_Log.h"

#if defined(SVC_EHID_PWRMON)
/*==============================================================================
 *                               Defines
 *============================================================================*/
/*============================================================================*/
#define SVC_PWRMON_SAMPLER_TIMER_ID BSP_TIMERGP_ID_0

// How many DataInd packets to buffer
#define SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT 2

/*==============================================================================
 *                                Types
 *============================================================================*/
// Structure is a preallocated message to be routed through event handler framework
typedef struct BSP_ATTR_PACKED svc_PwrMon_SamplerPkt_s {
    uint32_t               sysData;
    svc_PwrMonEh_DataInd_t dataInd;
} svc_PwrMon_SamplerPkt_t;

typedef struct svc_PwrMon_SamplerFrameInfo_s
{
    svc_PwrMonEh_ChBitmap_t vBitmap;
    svc_PwrMonEh_ChBitmap_t iBitmap;
    uint16_t                   smplSet;
} svc_PwrMon_SamplerPktInfo_t;

typedef struct svc_PwrMon_SamplerCtx_s
{
    svc_PwrMon_SamplerPkt_t      pktArray[SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT];
    svc_PwrMon_SamplerPktInfo_t  pktInfoArray[SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT];
    svc_PwrMonEh_SamplerStats_t  stats;
    svc_PwrMonEh_SmplFmt_t       smplFmt;
    svc_PwrMonEh_ChBitmap_t      chBitmap;
    uint8_t                      numCh;
    uint8_t                      pktIdx;
    uint8_t                      pktPrevIdx;
    bool_t                       firstSmpl;
    uint32_t                     pktLen;
} svc_PwrMon_SamplerCtx_t;


/*============================================================================*/
// Define a max size packet with known pattern to test the Packet Data pipe
#define SVC_PWRMON_SAMPLER_TEST_PKT
#ifdef SVC_PWRMON_SAMPLER_TEST_PKT
static const svc_PwrMon_SamplerPkt_t svc_PwrMon_tstPkt = {
    0,
    {
        {
            .id = SVC_PWRMONEH_DATA_IND,
            .eh = SVC_EHID_BROADCAST,
            .alloc = false,
            .cnt = 0,
            .len = sizeof(svc_PwrMon_SamplerPkt_t)
        },
        .numCh = 10,
        .numSmplSet = 50,
        .smplFmt = 0,
        .seq = 1234,
        .smplStartIdx = 1200,
        .chBitmap = 0x000003FF,
        {
            {.fmt0 = {   0, 1 }}, {.fmt0 = {   1, 2 }}, {.fmt0 = {   2, 3 }}, {.fmt0 = {   3, 4 }}, {.fmt0 = {   4, 5 }}, {.fmt0 = {   5, 6 }}, {.fmt0 = {   6, 7 }}, {.fmt0 = {   7, 8 }}, {.fmt0 = {   8, 9 }}, {.fmt0 = {   9, 10 }},
            {.fmt0 = {  10, 1 }}, {.fmt0 = {  11, 2 }}, {.fmt0 = {  12, 3 }}, {.fmt0 = {  13, 4 }}, {.fmt0 = {  14, 5 }}, {.fmt0 = {  15, 6 }}, {.fmt0 = {  16, 7 }}, {.fmt0 = {  17, 8 }}, {.fmt0 = {  18, 9 }}, {.fmt0 = {  19, 10 }},
            {.fmt0 = {  20, 1 }}, {.fmt0 = {  21, 2 }}, {.fmt0 = {  22, 3 }}, {.fmt0 = {  23, 4 }}, {.fmt0 = {  24, 5 }}, {.fmt0 = {  25, 6 }}, {.fmt0 = {  26, 7 }}, {.fmt0 = {  27, 8 }}, {.fmt0 = {  28, 9 }}, {.fmt0 = {  29, 10 }},
            {.fmt0 = {  30, 1 }}, {.fmt0 = {  31, 2 }}, {.fmt0 = {  32, 3 }}, {.fmt0 = {  33, 4 }}, {.fmt0 = {  34, 5 }}, {.fmt0 = {  35, 6 }}, {.fmt0 = {  36, 7 }}, {.fmt0 = {  37, 8 }}, {.fmt0 = {  38, 9 }}, {.fmt0 = {  39, 10 }},
            {.fmt0 = {  40, 1 }}, {.fmt0 = {  41, 2 }}, {.fmt0 = {  42, 3 }}, {.fmt0 = {  43, 4 }}, {.fmt0 = {  44, 5 }}, {.fmt0 = {  45, 6 }}, {.fmt0 = {  46, 7 }}, {.fmt0 = {  47, 8 }}, {.fmt0 = {  48, 9 }}, {.fmt0 = {  49, 10 }},
            {.fmt0 = {  50, 1 }}, {.fmt0 = {  51, 2 }}, {.fmt0 = {  52, 3 }}, {.fmt0 = {  53, 4 }}, {.fmt0 = {  54, 5 }}, {.fmt0 = {  55, 6 }}, {.fmt0 = {  56, 7 }}, {.fmt0 = {  57, 8 }}, {.fmt0 = {  58, 9 }}, {.fmt0 = {  59, 10 }},
            {.fmt0 = {  60, 1 }}, {.fmt0 = {  61, 2 }}, {.fmt0 = {  62, 3 }}, {.fmt0 = {  63, 4 }}, {.fmt0 = {  64, 5 }}, {.fmt0 = {  65, 6 }}, {.fmt0 = {  66, 7 }}, {.fmt0 = {  67, 8 }}, {.fmt0 = {  68, 9 }}, {.fmt0 = {  69, 10 }},
            {.fmt0 = {  70, 1 }}, {.fmt0 = {  71, 2 }}, {.fmt0 = {  72, 3 }}, {.fmt0 = {  73, 4 }}, {.fmt0 = {  74, 5 }}, {.fmt0 = {  75, 6 }}, {.fmt0 = {  76, 7 }}, {.fmt0 = {  77, 8 }}, {.fmt0 = {  78, 9 }}, {.fmt0 = {  79, 10 }},
            {.fmt0 = {  80, 1 }}, {.fmt0 = {  81, 2 }}, {.fmt0 = {  82, 3 }}, {.fmt0 = {  83, 4 }}, {.fmt0 = {  84, 5 }}, {.fmt0 = {  85, 6 }}, {.fmt0 = {  86, 7 }}, {.fmt0 = {  87, 8 }}, {.fmt0 = {  88, 9 }}, {.fmt0 = {  89, 10 }},
            {.fmt0 = {  90, 1 }}, {.fmt0 = {  91, 2 }}, {.fmt0 = {  92, 3 }}, {.fmt0 = {  93, 4 }}, {.fmt0 = {  94, 5 }}, {.fmt0 = {  95, 6 }}, {.fmt0 = {  96, 7 }}, {.fmt0 = {  97, 8 }}, {.fmt0 = {  98, 9 }}, {.fmt0 = {  99, 10 }},
            {.fmt0 = { 100, 1 }}, {.fmt0 = { 101, 2 }}, {.fmt0 = { 102, 3 }}, {.fmt0 = { 103, 4 }}, {.fmt0 = { 104, 5 }}, {.fmt0 = { 105, 6 }}, {.fmt0 = { 106, 7 }}, {.fmt0 = { 107, 8 }}, {.fmt0 = { 108, 9 }}, {.fmt0 = { 109, 10 }},
            {.fmt0 = { 110, 1 }}, {.fmt0 = { 111, 2 }}, {.fmt0 = { 112, 3 }}, {.fmt0 = { 113, 4 }}, {.fmt0 = { 114, 5 }}, {.fmt0 = { 115, 6 }}, {.fmt0 = { 116, 7 }}, {.fmt0 = { 117, 8 }}, {.fmt0 = { 118, 9 }}, {.fmt0 = { 119, 10 }},
            {.fmt0 = { 120, 1 }}, {.fmt0 = { 121, 2 }}, {.fmt0 = { 122, 3 }}, {.fmt0 = { 123, 4 }}, {.fmt0 = { 124, 5 }}, {.fmt0 = { 125, 6 }}, {.fmt0 = { 126, 7 }}, {.fmt0 = { 127, 8 }}, {.fmt0 = { 128, 9 }}, {.fmt0 = { 129, 10 }},
            {.fmt0 = { 130, 1 }}, {.fmt0 = { 131, 2 }}, {.fmt0 = { 132, 3 }}, {.fmt0 = { 133, 4 }}, {.fmt0 = { 134, 5 }}, {.fmt0 = { 135, 6 }}, {.fmt0 = { 136, 7 }}, {.fmt0 = { 137, 8 }}, {.fmt0 = { 138, 9 }}, {.fmt0 = { 139, 10 }},
            {.fmt0 = { 140, 1 }}, {.fmt0 = { 141, 2 }}, {.fmt0 = { 142, 3 }}, {.fmt0 = { 143, 4 }}, {.fmt0 = { 144, 5 }}, {.fmt0 = { 145, 6 }}, {.fmt0 = { 146, 7 }}, {.fmt0 = { 147, 8 }}, {.fmt0 = { 148, 9 }}, {.fmt0 = { 149, 10 }},
            {.fmt0 = { 150, 1 }}, {.fmt0 = { 151, 2 }}, {.fmt0 = { 152, 3 }}, {.fmt0 = { 153, 4 }}, {.fmt0 = { 154, 5 }}, {.fmt0 = { 155, 6 }}, {.fmt0 = { 156, 7 }}, {.fmt0 = { 157, 8 }}, {.fmt0 = { 158, 9 }}, {.fmt0 = { 159, 10 }},
            {.fmt0 = { 160, 1 }}, {.fmt0 = { 161, 2 }}, {.fmt0 = { 162, 3 }}, {.fmt0 = { 163, 4 }}, {.fmt0 = { 164, 5 }}, {.fmt0 = { 165, 6 }}, {.fmt0 = { 166, 7 }}, {.fmt0 = { 167, 8 }}, {.fmt0 = { 168, 9 }}, {.fmt0 = { 169, 10 }},
            {.fmt0 = { 170, 1 }}, {.fmt0 = { 171, 2 }}, {.fmt0 = { 172, 3 }}, {.fmt0 = { 173, 4 }}, {.fmt0 = { 174, 5 }}, {.fmt0 = { 175, 6 }}, {.fmt0 = { 176, 7 }}, {.fmt0 = { 177, 8 }}, {.fmt0 = { 178, 9 }}, {.fmt0 = { 179, 10 }},
            {.fmt0 = { 180, 1 }}, {.fmt0 = { 181, 2 }}, {.fmt0 = { 182, 3 }}, {.fmt0 = { 183, 4 }}, {.fmt0 = { 184, 5 }}, {.fmt0 = { 185, 6 }}, {.fmt0 = { 186, 7 }}, {.fmt0 = { 187, 8 }}, {.fmt0 = { 188, 9 }}, {.fmt0 = { 189, 10 }},
            {.fmt0 = { 190, 1 }}, {.fmt0 = { 191, 2 }}, {.fmt0 = { 192, 3 }}, {.fmt0 = { 193, 4 }}, {.fmt0 = { 194, 5 }}, {.fmt0 = { 195, 6 }}, {.fmt0 = { 196, 7 }}, {.fmt0 = { 197, 8 }}, {.fmt0 = { 198, 9 }}, {.fmt0 = { 199, 10 }},
            {.fmt0 = { 200, 1 }}, {.fmt0 = { 201, 2 }}, {.fmt0 = { 202, 3 }}, {.fmt0 = { 203, 4 }}, {.fmt0 = { 204, 5 }}, {.fmt0 = { 205, 6 }}, {.fmt0 = { 206, 7 }}, {.fmt0 = { 207, 8 }}, {.fmt0 = { 208, 9 }}, {.fmt0 = { 209, 10 }},
            {.fmt0 = { 210, 1 }}, {.fmt0 = { 211, 2 }}, {.fmt0 = { 212, 3 }}, {.fmt0 = { 213, 4 }}, {.fmt0 = { 214, 5 }}, {.fmt0 = { 215, 6 }}, {.fmt0 = { 216, 7 }}, {.fmt0 = { 217, 8 }}, {.fmt0 = { 218, 9 }}, {.fmt0 = { 219, 10 }},
            {.fmt0 = { 220, 1 }}, {.fmt0 = { 221, 2 }}, {.fmt0 = { 222, 3 }}, {.fmt0 = { 223, 4 }}, {.fmt0 = { 224, 5 }}, {.fmt0 = { 225, 6 }}, {.fmt0 = { 226, 7 }}, {.fmt0 = { 227, 8 }}, {.fmt0 = { 228, 9 }}, {.fmt0 = { 229, 10 }},
            {.fmt0 = { 230, 1 }}, {.fmt0 = { 231, 2 }}, {.fmt0 = { 232, 3 }}, {.fmt0 = { 233, 4 }}, {.fmt0 = { 234, 5 }}, {.fmt0 = { 235, 6 }}, {.fmt0 = { 236, 7 }}, {.fmt0 = { 237, 8 }}, {.fmt0 = { 238, 9 }}, {.fmt0 = { 239, 10 }},
            {.fmt0 = { 240, 1 }}, {.fmt0 = { 241, 2 }}, {.fmt0 = { 242, 3 }}, {.fmt0 = { 243, 4 }}, {.fmt0 = { 242, 5 }}, {.fmt0 = { 245, 6 }}, {.fmt0 = { 246, 7 }}, {.fmt0 = { 247, 8 }}, {.fmt0 = { 248, 9 }}, {.fmt0 = { 249, 10 }},
            {.fmt0 = { 250, 1 }}, {.fmt0 = { 251, 2 }}, {.fmt0 = { 252, 3 }}, {.fmt0 = { 253, 4 }}, {.fmt0 = { 254, 5 }}, {.fmt0 = { 255, 6 }}, {.fmt0 = { 256, 7 }}, {.fmt0 = { 257, 8 }}, {.fmt0 = { 258, 9 }}, {.fmt0 = { 259, 10 }},
            {.fmt0 = { 260, 1 }}, {.fmt0 = { 261, 2 }}, {.fmt0 = { 262, 3 }}, {.fmt0 = { 263, 4 }}, {.fmt0 = { 264, 5 }}, {.fmt0 = { 265, 6 }}, {.fmt0 = { 266, 7 }}, {.fmt0 = { 267, 8 }}, {.fmt0 = { 268, 9 }}, {.fmt0 = { 269, 10 }},
            {.fmt0 = { 270, 1 }}, {.fmt0 = { 271, 2 }}, {.fmt0 = { 272, 3 }}, {.fmt0 = { 273, 4 }}, {.fmt0 = { 274, 5 }}, {.fmt0 = { 275, 6 }}, {.fmt0 = { 276, 7 }}, {.fmt0 = { 277, 8 }}, {.fmt0 = { 278, 9 }}, {.fmt0 = { 279, 10 }},
            {.fmt0 = { 280, 1 }}, {.fmt0 = { 281, 2 }}, {.fmt0 = { 282, 3 }}, {.fmt0 = { 283, 4 }}, {.fmt0 = { 284, 5 }}, {.fmt0 = { 285, 6 }}, {.fmt0 = { 286, 7 }}, {.fmt0 = { 287, 8 }}, {.fmt0 = { 288, 9 }}, {.fmt0 = { 289, 10 }},
            {.fmt0 = { 290, 1 }}, {.fmt0 = { 291, 2 }}, {.fmt0 = { 292, 3 }}, {.fmt0 = { 293, 4 }}, {.fmt0 = { 294, 5 }}, {.fmt0 = { 295, 6 }}, {.fmt0 = { 296, 7 }}, {.fmt0 = { 297, 8 }}, {.fmt0 = { 298, 9 }}, {.fmt0 = { 299, 10 }},
            {.fmt0 = { 300, 1 }}, {.fmt0 = { 301, 2 }}, {.fmt0 = { 302, 3 }}, {.fmt0 = { 303, 4 }}, {.fmt0 = { 304, 5 }}, {.fmt0 = { 305, 6 }}, {.fmt0 = { 306, 7 }}, {.fmt0 = { 307, 8 }}, {.fmt0 = { 308, 9 }}, {.fmt0 = { 309, 10 }},
            {.fmt0 = { 310, 1 }}, {.fmt0 = { 311, 2 }}, {.fmt0 = { 312, 3 }}, {.fmt0 = { 313, 4 }}, {.fmt0 = { 314, 5 }}, {.fmt0 = { 315, 6 }}, {.fmt0 = { 316, 7 }}, {.fmt0 = { 317, 8 }}, {.fmt0 = { 318, 9 }}, {.fmt0 = { 319, 10 }},
            {.fmt0 = { 320, 1 }}, {.fmt0 = { 321, 2 }}, {.fmt0 = { 322, 3 }}, {.fmt0 = { 323, 4 }}, {.fmt0 = { 324, 5 }}, {.fmt0 = { 325, 6 }}, {.fmt0 = { 326, 7 }}, {.fmt0 = { 327, 8 }}, {.fmt0 = { 328, 9 }}, {.fmt0 = { 329, 10 }},
            {.fmt0 = { 330, 1 }}, {.fmt0 = { 331, 2 }}, {.fmt0 = { 332, 3 }}, {.fmt0 = { 333, 4 }}, {.fmt0 = { 334, 5 }}, {.fmt0 = { 335, 6 }}, {.fmt0 = { 336, 7 }}, {.fmt0 = { 337, 8 }}, {.fmt0 = { 338, 9 }}, {.fmt0 = { 339, 10 }},
            {.fmt0 = { 340, 1 }}, {.fmt0 = { 341, 2 }}, {.fmt0 = { 342, 3 }}, {.fmt0 = { 343, 4 }}, {.fmt0 = { 344, 5 }}, {.fmt0 = { 345, 6 }}, {.fmt0 = { 346, 7 }}, {.fmt0 = { 347, 8 }}, {.fmt0 = { 348, 9 }}, {.fmt0 = { 349, 10 }},
            {.fmt0 = { 350, 1 }}, {.fmt0 = { 351, 2 }}, {.fmt0 = { 352, 3 }}, {.fmt0 = { 353, 4 }}, {.fmt0 = { 354, 5 }}, {.fmt0 = { 355, 6 }}, {.fmt0 = { 356, 7 }}, {.fmt0 = { 357, 8 }}, {.fmt0 = { 358, 9 }}, {.fmt0 = { 359, 10 }},
            {.fmt0 = { 360, 1 }}, {.fmt0 = { 361, 2 }}, {.fmt0 = { 362, 3 }}, {.fmt0 = { 363, 4 }}, {.fmt0 = { 364, 5 }}, {.fmt0 = { 365, 6 }}, {.fmt0 = { 366, 7 }}, {.fmt0 = { 367, 8 }}, {.fmt0 = { 368, 9 }}, {.fmt0 = { 369, 10 }},
            {.fmt0 = { 370, 1 }}, {.fmt0 = { 371, 2 }}, {.fmt0 = { 372, 3 }}, {.fmt0 = { 373, 4 }}, {.fmt0 = { 374, 5 }}, {.fmt0 = { 375, 6 }}, {.fmt0 = { 376, 7 }}, {.fmt0 = { 377, 8 }}, {.fmt0 = { 378, 9 }}, {.fmt0 = { 379, 10 }},
            {.fmt0 = { 380, 1 }}, {.fmt0 = { 381, 2 }}, {.fmt0 = { 382, 3 }}, {.fmt0 = { 383, 4 }}, {.fmt0 = { 384, 5 }}, {.fmt0 = { 385, 6 }}, {.fmt0 = { 386, 7 }}, {.fmt0 = { 387, 8 }}, {.fmt0 = { 388, 9 }}, {.fmt0 = { 389, 10 }},
            {.fmt0 = { 390, 1 }}, {.fmt0 = { 391, 2 }}, {.fmt0 = { 392, 3 }}, {.fmt0 = { 393, 4 }}, {.fmt0 = { 394, 5 }}, {.fmt0 = { 395, 6 }}, {.fmt0 = { 396, 7 }}, {.fmt0 = { 397, 8 }}, {.fmt0 = { 398, 9 }}, {.fmt0 = { 399, 10 }},
            {.fmt0 = { 400, 1 }}, {.fmt0 = { 401, 2 }}, {.fmt0 = { 402, 3 }}, {.fmt0 = { 403, 4 }}, {.fmt0 = { 404, 5 }}, {.fmt0 = { 405, 6 }}, {.fmt0 = { 406, 7 }}, {.fmt0 = { 407, 8 }}, {.fmt0 = { 408, 9 }}, {.fmt0 = { 409, 10 }},
            {.fmt0 = { 410, 1 }}, {.fmt0 = { 411, 2 }}, {.fmt0 = { 412, 3 }}, {.fmt0 = { 413, 4 }}, {.fmt0 = { 414, 5 }}, {.fmt0 = { 415, 6 }}, {.fmt0 = { 416, 7 }}, {.fmt0 = { 417, 8 }}, {.fmt0 = { 418, 9 }}, {.fmt0 = { 419, 10 }},
            {.fmt0 = { 420, 1 }}, {.fmt0 = { 421, 2 }}, {.fmt0 = { 422, 3 }}, {.fmt0 = { 423, 4 }}, {.fmt0 = { 424, 5 }}, {.fmt0 = { 425, 6 }}, {.fmt0 = { 426, 7 }}, {.fmt0 = { 427, 8 }}, {.fmt0 = { 428, 9 }}, {.fmt0 = { 429, 10 }},
            {.fmt0 = { 430, 1 }}, {.fmt0 = { 431, 2 }}, {.fmt0 = { 432, 3 }}, {.fmt0 = { 433, 4 }}, {.fmt0 = { 434, 5 }}, {.fmt0 = { 435, 6 }}, {.fmt0 = { 436, 7 }}, {.fmt0 = { 437, 8 }}, {.fmt0 = { 438, 9 }}, {.fmt0 = { 439, 10 }},
            {.fmt0 = { 440, 1 }}, {.fmt0 = { 441, 2 }}, {.fmt0 = { 442, 3 }}, {.fmt0 = { 443, 4 }}, {.fmt0 = { 444, 5 }}, {.fmt0 = { 445, 6 }}, {.fmt0 = { 446, 7 }}, {.fmt0 = { 447, 8 }}, {.fmt0 = { 448, 9 }}, {.fmt0 = { 449, 10 }},
            {.fmt0 = { 450, 1 }}, {.fmt0 = { 451, 2 }}, {.fmt0 = { 452, 3 }}, {.fmt0 = { 453, 4 }}, {.fmt0 = { 454, 5 }}, {.fmt0 = { 455, 6 }}, {.fmt0 = { 456, 7 }}, {.fmt0 = { 457, 8 }}, {.fmt0 = { 458, 9 }}, {.fmt0 = { 459, 10 }},
            {.fmt0 = { 460, 1 }}, {.fmt0 = { 461, 2 }}, {.fmt0 = { 462, 3 }}, {.fmt0 = { 463, 4 }}, {.fmt0 = { 464, 5 }}, {.fmt0 = { 465, 6 }}, {.fmt0 = { 466, 7 }}, {.fmt0 = { 467, 8 }}, {.fmt0 = { 468, 9 }}, {.fmt0 = { 469, 10 }},
            {.fmt0 = { 470, 1 }}, {.fmt0 = { 471, 2 }}, {.fmt0 = { 472, 3 }}, {.fmt0 = { 473, 4 }}, {.fmt0 = { 474, 5 }}, {.fmt0 = { 475, 6 }}, {.fmt0 = { 476, 7 }}, {.fmt0 = { 477, 8 }}, {.fmt0 = { 478, 9 }}, {.fmt0 = { 479, 10 }},
            {.fmt0 = { 480, 1 }}, {.fmt0 = { 481, 2 }}, {.fmt0 = { 482, 3 }}, {.fmt0 = { 483, 4 }}, {.fmt0 = { 484, 5 }}, {.fmt0 = { 485, 6 }}, {.fmt0 = { 486, 7 }}, {.fmt0 = { 487, 8 }}, {.fmt0 = { 488, 9 }}, {.fmt0 = { 489, 10 }},
            {.fmt0 = { 490, 1 }}, {.fmt0 = { 491, 2 }}, {.fmt0 = { 492, 2 }}, {.fmt0 = { 493, 4 }}, {.fmt0 = { 494, 5 }}, {.fmt0 = { 495, 6 }}, {.fmt0 = { 496, 7 }}, {.fmt0 = { 497, 8 }}, {.fmt0 = { 498, 9 }}, {.fmt0 = { 499, 10 }}
        }
    }
};
#endif

/*==============================================================================
 *                                Globals
 *============================================================================*/
svc_PwrMon_SamplerCtx_t BSP_ATTR_ALIGNMENT(4) svc_PwrMon_ctx;

#define SVC_PWRMON_SAMPLER_SET_COMPLETE(_ctx)                \
    ( ((_ctx)->pktInfoArray[(_ctx)->pktIdx].iBitmap == 0) && \
      ((_ctx)->pktInfoArray[(_ctx)->pktIdx].vBitmap == 0) )

/*==============================================================================
 *                             Local Functions
 *============================================================================*/

/*============================================================================*/
// Called at the beginning of the fixed time sampling callback, before making all i2c requests
bool_t
svc_PwrMon_checkForCompletion( svc_PwrMon_SamplerCtx_t* ctx )
{
    svc_PwrMon_SamplerPkt_t*  pktPtr;
    svc_PwrMon_SamplerPktInfo_t* pktInfoPtr;

    BSP_TRACE_PWRMON_SAMPLER_SET_DONE_ENTER();
    // Quick exit on first time after starting
    if( ctx->firstSmpl == true )
    {
        ctx->firstSmpl = false;
        BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT();
        return( false );
    }

    // Unconditionally increment the smplSet and smplNum
    pktInfoPtr = &ctx->pktInfoArray[ ctx->pktIdx ];
    pktInfoPtr->smplSet++;
    ctx->stats.smplNum++;

    if( SVC_PWRMON_SAMPLER_SET_COMPLETE( ctx ) == false )
    {
        // Previous sample set did not complete in time.
        // Todo: Keep track of errors per channel
        ctx->stats.smplErrNum++;
    }

    if( pktInfoPtr->smplSet == SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX )
    {
        BSP_TRACE_PWRMON_SAMPLER_PKT_DONE();

        // Make sure the previous
        ctx->stats.pktErrNum += (ctx->pktArray[ ctx->pktIdx ].dataInd.hdr.cnt != 0) ? 1 : 0;

        // Reset frame info and point to next packet
        ctx->pktPrevIdx     = ctx->pktIdx;
        ctx->pktIdx         = ((ctx->pktIdx + 1) % SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT);
        pktPtr              = &ctx->pktArray[ ctx->pktIdx ];
        pktInfoPtr          = &ctx->pktInfoArray[ ctx->pktIdx ];
        pktInfoPtr->smplSet = 0;

        // Setup next packet header
        ctx->stats.pktSndNum++;
        pktPtr->dataInd.seq          = ctx->stats.pktSndNum;
        pktPtr->dataInd.smplStartIdx = ctx->stats.smplNum;

        BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT();
        return( true );
    }

    BSP_TRACE_PWRMON_SAMPLER_SET_DONE_EXIT();
    return( false );
}

/*============================================================================*/
void svc_PwrMon_voltageCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    ctx->pktInfoArray[ ctx->pktIdx ].vBitmap ^= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_VOLTAGE_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_currentCallback( void* cbData )
{
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    ctx->pktInfoArray[ ctx->pktIdx ].iBitmap ^= (1 << (uint32_t)cbData);
    BSP_TRACE_PWRMON_SAMPLER_CURRENT_EXIT();
    return;
}

/*============================================================================*/
void svc_PwrMon_sendTestPkt( void )
{
    memcpy( &svc_PwrMon_ctx.pktArray[0], &svc_PwrMon_tstPkt, sizeof(svc_PwrMon_tstPkt));
    svc_MsgFwk_msgBroadcast( &(svc_PwrMon_ctx.pktArray[0].dataInd.hdr) );
}

/*============================================================================*/
// Main interrupt driving the sampling of the Power Monitor ICs
void svc_PwrMon_samplerCallback( bsp_TimerGp_TimerId_t    timerId,
                                 bsp_TimerGp_SubTimerId_t subTimerId,
                                 uint32_t                 mask )
{
    BSP_TRACE_PWRMON_SAMPLER_SET_ENTER();
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;

    bool_t sndPkt = svc_PwrMon_checkForCompletion( ctx );

    // Request all configured channels for next sample Set
    svc_PwrMon_SamplerPkt_t*     pktPtr     = &ctx->pktArray[ ctx->pktIdx ];
    svc_PwrMon_SamplerPktInfo_t* pktInfoPtr = &ctx->pktInfoArray[ ctx->pktIdx ];
    uint16_t chIdx = 0;
    uint16_t offsetIdx = (pktInfoPtr->smplSet * ctx->numCh);

    // Reset the sample set bitmaps
    pktInfoPtr->vBitmap = ctx->chBitmap;
    pktInfoPtr->iBitmap = ctx->chBitmap;
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        if( (ctx->chBitmap & (1 << chId)) != 0 )
        {
            if( ctx->smplFmt == SVC_PWRMONEH_SMPL_FMT0_IV)
            {
                dev_PwrMon_channelCurrentRead( chId,
                                               (uint8_t*)&pktPtr->dataInd.data[ (offsetIdx + chIdx) ].fmt0.i,
                                               svc_PwrMon_currentCallback,
                                               (void*)(uint32_t)chId );

                dev_PwrMon_channelBusVoltageRead( chId,
                                                  (uint8_t*)&pktPtr->dataInd.data[ (offsetIdx + chIdx) ].fmt0.v,
                                                  svc_PwrMon_voltageCallback,
                                                  (void*)(uint32_t)chId );
            }
            chIdx++;
        }
    }

    // Check if packet needs to be sent
    if( sndPkt == true )
    {
        BSP_TRACE_PWRMON_SAMPLER_PKT_SND_ENTER();
        svc_MsgFwk_msgBroadcast( &(ctx->pktArray[ ctx->pktPrevIdx ].dataInd.hdr) );
        BSP_TRACE_PWRMON_SAMPLER_PKT_SND_EXIT();
    }
    BSP_TRACE_PWRMON_SAMPLER_SET_EXIT();
    return;
}


/*==============================================================================
 *                             Public Functions
 *============================================================================*/
/*============================================================================*/
void svc_PwrMon_samplerInit( void )
{
    BSP_ASSERT(callback != NULL);
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
    memset( &svc_PwrMon_ctx, 0, sizeof(svc_PwrMon_ctx) );
}

/*============================================================================*/
void svc_PwrMon_samplerStop( void )
{
    bsp_TimerGp_stop( SVC_PWRMON_SAMPLER_TIMER_ID );
}

/*============================================================================*/
svc_PwrMonEh_SamplerStats_t*
svc_PwrMon_samplerStatsPtr( void )
{
    return( &svc_PwrMon_ctx.stats );
}

/*============================================================================*/
void
svc_PwrMon_samplerStatsReset( void )
{
    memset( &svc_PwrMon_ctx.stats, 0, sizeof(svc_PwrMon_ctx.stats) );
    return;
}

/*============================================================================*/
void svc_PwrMon_samplerStart( svc_PwrMonEh_ChBitmap_t chBitmap,
                              svc_PwrMonEh_SmplFmt_t     smplFmt )
{
    svc_PwrMon_SamplerCtx_t* ctx = &svc_PwrMon_ctx;
    memset( &ctx->pktInfoArray[0], 0, sizeof(ctx->pktInfoArray) );
    svc_PwrMon_samplerStatsReset();

    // Reset Frame
    ctx->firstSmpl = true;
    ctx->chBitmap  = chBitmap;
    ctx->smplFmt   = smplFmt;

    // Count the channels
    ctx->numCh = 0;
    while( chBitmap )
    {
        ctx->numCh += (chBitmap & 0x01);
        chBitmap >>= 1;
    }

    // Compute the packet length in bytes
    ctx->pktLen = ( (sizeof(svc_PwrMonEh_DataInd_t) - sizeof(svc_MsgFwk_Hdr_t)) -
                    ((SVC_PWRMONEH_DATA_IND_CHANNELS_MAX - ctx->numCh) * sizeof(svc_PwrMonEh_SmplData_t)) );

    // Setup static portion of headers
    for( int i=0; i<SVC_PWRMON_SAMPLER_PACKET_BUFFER_CNT; i++ )
    {
        svc_PwrMon_SamplerPkt_t* pktPtr = &ctx->pktArray[ i ];

        // sysData is typically hidden behind the msgFwk allocation method
        // and is used when forwarding messages off device.
        pktPtr->sysData = 0;

        // Setup the Event Handler message portion. cn increments for every
        // received queue and decrements for every free. Will be 0 once
        // shipped off device
        pktPtr->dataInd.hdr.id    = SVC_PWRMONEH_DATA_IND;
        pktPtr->dataInd.hdr.eh    = SVC_EHID_BROADCAST;
        pktPtr->dataInd.hdr.alloc = false;
        pktPtr->dataInd.hdr.cnt   = 0;
        pktPtr->dataInd.hdr.len   = ctx->pktLen + sizeof(svc_MsgFwk_Hdr_t);

        // Fill in sample packet header information
        pktPtr->dataInd.numCh      = ctx->numCh;
        pktPtr->dataInd.numSmplSet = SVC_PWRMONEH_DATA_IND_SAMPLE_SETS_MAX;
        pktPtr->dataInd.smplFmt    = ctx->smplFmt;
        pktPtr->dataInd.chBitmap   = ctx->chBitmap;

        // These fields change per packet, initialized to 0
        pktPtr->dataInd.seq          = ctx->stats.pktSndNum;
        pktPtr->dataInd.smplStartIdx = ctx->stats.smplNum;
    }

    // Get each power monitor IC set to the proper register with a dummy read
    for( dev_PwrMon_ChannelId_t chId = 0; chId < BSP_PLATFORM_PWRMON_NUM_CHANNELS; chId++ )
    {
        dev_PwrMon_Data_t tmp;
        if( (ctx->chBitmap & (1 << chId)) != 0 )
        {
            dev_PwrMon_channelCurrentRead( chId, (uint8_t*)&tmp, NULL, NULL );
            dev_PwrMon_channelBusVoltageRead( chId, (uint8_t*)&tmp, NULL, NULL );
        }
    }

    bsp_TimerGp_startCountdown( SVC_PWRMON_SAMPLER_TIMER_ID,
                                BSP_TIMERGP_TYPE_PERIODIC,
                                BSP_TIMERGP_DMA_CTRL_NONE,
                                SVC_PWRMON_SAMPLER_TICK_US,
                                svc_PwrMon_samplerCallback );
}
#endif
