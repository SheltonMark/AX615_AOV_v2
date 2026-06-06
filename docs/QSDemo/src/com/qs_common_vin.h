/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __QS_COMMON_VIN_H__
#define __QS_COMMON_VIN_H__

#include "ax_vin_api.h"
#include "ax_base_type.h"
#include "ax_sensor_struct.h"
#include "ax_mipi_rx_api.h"
#include "ax_isp_3a_plus.h"

#define BOARD_ID_LEN 32

typedef struct _AX_SNS_CLK_ATTR_T_ {
    AX_U8               nSnsClkIdx;
    AX_SNS_CLK_RATE_E   eSnsClkRate;
} QS_AX_SNS_CLK_ATTR_T;

typedef enum {
    QS_SAMPLE_SNS_TYPE_NONE = -1,

    /* tpg dummy */
    SENSOR_DUMMY = 0,

    /* ov sensor */
    OMNIVISION_OS04A10 = 1,
    OMNIVISION_OS04D10 = 2,

    /* smartsens sensor */
    SMARTSENS_SC450AI = 10,
    SMARTSENS_SC200AI = 11,
    SMARTSENS_SC500AI = 12,
    SMARTSENS_SC235HAI = 13,
    SMARTSENS_SC438HAI = 14,
    SMARTSENS_SC533HAI = 15,
    SMARTSENS_SC465SL = 16,

    /* imagedesign sensor */
    IMAGEDESIGN_MIS20S1 = 20,

    QS_SAMPLE_SNS_TYPE_BUTT,
} QS_SAMPLE_SNS_TYPE_E;

typedef enum {
    QS_COMMON_VIN_NONE = -1,
    QS_COMMON_VIN_LOADRAW = 0,
    QS_COMMON_VIN_SENSOR = 1,
    QS_COMMON_VIN_TPG = 2,
    QS_COMMON_VIN_BUTT
} QS_COMMON_VIN_MODE_E;

typedef enum {
    LOAD_RAW_NONE = -1,
    LOAD_RAW_IFE = 0,
    LOAD_RAW_ITP = 1,
    LOAD_RAW_RGB = 2,
    LOAD_RAW_YUV = 3,
    LOAD_RAW_BUTT
} QS_SAMPLE_LOAD_RAW_NODE_E;

#ifndef QS_COMM_VIN_PRT
#define QS_COMM_VIN_PRT(fmt...)   \
do {\
    printf("[COMM_VIN][%s][%5d] ", __FUNCTION__, __LINE__);\
    printf(fmt);\
}while(0)
#endif


#ifdef __cplusplus
extern "C"
{
#endif

AX_S32 QS_COMMON_VIN_StartMipi(AX_U8 nRxDev, AX_INPUT_MODE_E eInputMode, AX_MIPI_RX_ATTR_T *ptMipiAttr, AX_LANE_COMBO_MODE_E eLaneComboMode);
AX_S32 QS_COMMON_VIN_StopMipi(AX_U8 devId);
AX_S32 QS_COMMON_VIN_CreateDev(AX_U8 devId, AX_U32 nRxDev, AX_VIN_DEV_ATTR_T *pDevAttr, AX_VIN_DEV_BIND_PIPE_T *ptDevBindPipe);
AX_S32 QS_COMMON_VIN_DestroyDev(AX_U8 devId);
AX_S32 QS_COMMON_VIN_StartDev(AX_U8 devId, AX_BOOL bEnableDev, AX_VIN_DEV_ATTR_T *pDevAttr);
AX_S32 QS_COMMON_VIN_StopDev(AX_U8 devId, AX_BOOL bEnableDev);

AX_S32 QS_COMMON_VIN_SetPipeAttr(QS_COMMON_VIN_MODE_E eSysMode, QS_SAMPLE_LOAD_RAW_NODE_E eLoadRawNode, AX_U8 nPipeId, AX_VIN_PIPE_ATTR_T *pPipeAttr);

AX_S32 QS_COMMON_VIN_StartChn(AX_U8 pipe, AX_VIN_CHN_ATTR_T *ptChnAttr, AX_BOOL *pChnEn, AX_S32 *pChnFrmMode);
AX_S32 QS_COMMON_VIN_StopChn(AX_U8 pipe);

#ifdef __cplusplus
}
#endif

#endif
