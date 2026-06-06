/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _AX_IVPS_COMM_H_
#define _AX_IVPS_COMM_H_
#include "ax_global_type.h"
#include "ax_pool_type.h"

/* When the common error code of AX_ERR_CODE_E cannot meet the requirements,
 the module error code can be extended in the following places,
 and the value should start with ox80 */

#define IVPS_SUCC 0x00

#define AX_SUB_ID_IVPS         0x01
#define AX_SUB_ID_RGN          0x02
#define AX_SUB_ID_GDC          0x03
#define AX_SUB_ID_VPP          0x04
#define AX_SUB_ID_VGP          0x05

#define AX_ERROR_IVPS(e) AX_DEF_ERR(AX_ID_IVPS, AX_SUB_ID_IVPS, (e))
#define AX_ERROR_RGN(e)  AX_DEF_ERR(AX_ID_IVPS, AX_SUB_ID_RGN, (e))
#define AX_ERROR_GDC(e)  AX_DEF_ERR(AX_ID_IVPS, AX_SUB_ID_GDC, (e))


/*******************************IVPS ERROR CODE************************************/
/* IVPS module is not exist, maybe IVPS module is not initial */
#define AX_ERR_IVPS_INVALID_MODID                 AX_ERROR_IVPS(AX_ERR_INVALID_MODID)
/* device node is invalid, plz check /dev/[xxx] */
#define AX_ERR_IVPS_INVALID_DEVID                 AX_ERROR_IVPS(AX_ERR_INVALID_DEVID)
/* this channel is not exist */
#define AX_ERR_IVPS_INVALID_CHNID                 AX_ERROR_IVPS(AX_ERR_INVALID_CHNID)
/* at lease one parameter is illagal ,eg. an illegal enumeration value */
#define AX_ERR_IVPS_ILLEGAL_PARAM                 AX_ERROR_IVPS(AX_ERR_ILLEGAL_PARAM)
/* used a NULL point */
#define AX_ERR_IVPS_NULL_PTR                      AX_ERROR_IVPS(AX_ERR_NULL_PTR)
/* bad address, eg. used for copy_from_user & copy_to_user */
#define AX_ERR_IVPS_BAD_ADDR                      AX_ERROR_IVPS(AX_ERR_BAD_ADDR)
/* system is not ready,had not initialed or loaded*/
#define AX_ERR_IVPS_SYS_NOTREADY                  AX_ERROR_IVPS(AX_ERR_SYS_NOTREADY)
/* ivps channel is busy */
#define AX_ERR_IVPS_BUSY                          AX_ERROR_IVPS(AX_ERR_BUSY)
/* initilize error */
#define AX_ERR_IVPS_NOT_INIT                      AX_ERROR_IVPS(AX_ERR_NOT_INIT)
/* try to enable or initialize system, device or channel, before configing attribute */
#define AX_ERR_IVPS_NOT_CONFIG                    AX_ERROR_IVPS(AX_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AX_ERR_IVPS_NOT_SUPPORT                   AX_ERROR_IVPS(AX_ERR_NOT_SUPPORT)
/* operation is not permitted, eg. try to change stati attribute */
#define AX_ERR_IVPS_NOT_PERM                      AX_ERROR_IVPS(AX_ERR_NOT_PERM)
/* channel exists */
#define AX_ERR_IVPS_EXIST                         AX_ERROR_IVPS(AX_ERR_EXIST)
/* the channle is not existed */
#define AX_ERR_IVPS_UNEXIST                       AX_ERROR_IVPS(AX_ERR_UNEXIST)
/* failure caused by malloc memory */
#define AX_ERR_IVPS_NOMEM                         AX_ERROR_IVPS(AX_ERR_NOMEM)
/* failure caused by getting block from pool */
#define AX_ERR_IVPS_NOBUF                         AX_ERROR_IVPS(AX_ERR_NOBUF)
/* buffer contains no data */
#define AX_ERR_IVPS_BUF_EMPTY                     AX_ERROR_IVPS(AX_ERR_BUF_EMPTY)
/* buffer contains fresh data */
#define AX_ERR_IVPS_BUF_FULL                      AX_ERROR_IVPS(AX_ERR_BUF_FULL)
/* failed to read as fifo is empty */
#define AX_ERR_IVPS_QUEUE_EMPTY                   AX_ERROR_IVPS(AX_ERR_QUEUE_EMPTY)
/* failed to write as fifo is full */
#define AX_ERR_IVPS_QUEUE_FULL                    AX_ERROR_IVPS(AX_ERR_QUEUE_FULL)
/* wait timeout failed */
#define AX_ERR_IVPS_TIMED_OUT                     AX_ERROR_IVPS(AX_ERR_TIMED_OUT)
/* process termination */
#define AX_ERR_IVPS_FLOW_END                      AX_ERROR_IVPS(AX_ERR_FLOW_END)
/* for ivps unknown error */
#define AX_ERR_IVPS_UNKNOWN                       AX_ERROR_IVPS(AX_ERR_UNKNOWN)


/*******************************IVPS RGN ERROR CODE*********************************/
/* this region handle id invalid */
#define AX_ERR_IVPS_RGN_INVALID_GRPID             AX_ERROR_RGN(AX_ERR_INVALID_GRPID)
/* this region has been attached and in busy state */
#define AX_ERR_IVPS_RGN_BUSY                      AX_ERROR_RGN(AX_ERR_BUSY)
/* this region has been destroy */
#define AX_ERR_IVPS_RGN_UNEXIST                   AX_ERROR_RGN(AX_ERR_UNEXIST)
/* at lease one parameter is illagal */
#define AX_ERR_IVPS_RGN_ILLEGAL_PARAM             AX_ERROR_RGN(AX_ERR_ILLEGAL_PARAM)
/* failure caused by getting block from pool */
#define AX_ERR_IVPS_RGN_NOBUF                     AX_ERROR_RGN(AX_ERR_NOBUF)


#define AX_IVPS_MAX_LUMA_SUM_ROI_NUM 512

#endif
