/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _AX_LENS_STRUCT_H_
#define _AX_LENS_STRUCT_H_

#include "ax_base_type.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define LENS_TRACK_TABLE_MAX_ROW         (128)
#define LENS_TRACK_TABLE_MAX_COL         (32)
#define LENS_IRIS_MAP_TABLE_NUM          (1024)
#define LENS_PLATFORM_NUM_MAX            (2)

typedef enum _AX_LENS_MOTOR_FUNC_E_ {
    // DO NOT CHANGE ORDER
    AX_MOTOR_FOCUS = 0,
    AX_MOTOR_ZOOM  = 1,
    AX_MOTOR_ZOOM2 = 2,
    AX_MOTOR_IRIS  = 3,
    AX_MOTOR_ICR   = 4,
    AX_MOTOR_FUNC_MAX,
} AX_LENS_MOTOR_FUNC_E;

typedef enum _AX_LENS_MOTOR_STATUS_E_ {
    AX_MOTOR_ST_IDLE,
    AX_MOTOR_ST_HOMING,
    AX_MOTOR_ST_PI_FOUND,
    AX_MOTOR_ST_RUNNING,
    AX_MOTOR_ST_FINISHED,
    AX_MOTOR_ST_HOLDING,
    AX_MOTOR_ST_ERROR,
} AX_LENS_MOTOR_STATUS_E;

typedef enum _AX_LENS_IRIS_CONTROL_MODE_E_ {
    AX_IRIS_CTRL_NONE,
    AX_IRIS_CTRL_STEP_MOTOR,
    AX_IRIS_CTRL_DC_MOTOR,
    AX_IRIS_CTRL_DC_WITH_HALL,
    AX_IRIS_CTRL_PWM,
    AX_IRIS_CTRL_GPIO,
} AX_LENS_IRIS_CONTROL_MODE_E;

typedef enum _AX_LENS_ICR_CONTROL_MODE_E_ {
    AX_ICR_CTRL_NONE,
    AX_ICR_CTRL_STEP_MOTOR,
    AX_ICR_CTRL_DC_MOTOR,
    AX_ICR_CTRL_DC_WITH_HALL,
    AX_ICR_CTRL_SOLENOID,
    AX_ICR_CTRL_GPIO,
} AX_LENS_ICR_CONTROL_MODE_E;

typedef enum _AX_LENS_IRIS_TYPE_E_ {
    AX_IRIS_TYPE_FIXED,
    AX_IRIS_TYPE_DC,
    AX_IRIS_TYPE_P,
} AX_LENS_IRIS_TYPE_E;

typedef enum _AX_LENS_IRIS_FNO_E_ {
    AX_IRIS_F_NO_32_0,
    AX_IRIS_F_NO_22_0,
    AX_IRIS_F_NO_16_0,
    AX_IRIS_F_NO_11_0,
    AX_IRIS_F_NO_8_0,
    AX_IRIS_F_NO_5_6,
    AX_IRIS_F_NO_4_0,
    AX_IRIS_F_NO_2_8,
    AX_IRIS_F_NO_2_0,
    AX_IRIS_F_NO_1_4,
    AX_IRIS_F_NO_1_0,
} AX_LENS_IRIS_FNO_E;

typedef struct _AX_LENS_ZOOM_TRACK_TABLE_T_ {
    AX_U8  nCurveTableRows;
    AX_U8  nCurveTableCols;
    AX_S32 nCurveTable[LENS_TRACK_TABLE_MAX_ROW * LENS_TRACK_TABLE_MAX_COL];
    AX_S32 nDistanceTable[LENS_TRACK_TABLE_MAX_COL];
} AX_LENS_ZOOM_TRACK_TABLE_T;

typedef struct _AX_LENS_FZ_HW_PARAM_T_ {
    /* Focus Motor Params */
    AX_S32 nFocusHwMaxStep;                            /* Max Step of Focus Motor Hardware */
    AX_S32 nFocusHwMinPos;                             /* Min Position of Focus Motor Hardware */
    AX_S32 nFocusHwMaxPos;                             /* Max Position of Focus Motor Hardware */
    AX_S32 nFocusHwWideFarPos;                         /* Focus Motor Position of Far End in Wide Angle */
    AX_U8  nFocusRangeExtBase;                         /* Base-Extended-Range of Scan and Search (Unit:Step) */
    AX_U8  nFocusRangeExtRatio;                        /* How Many focusRangeExtBase are Added to Theoretical Far & Near End */

    /* Zoom Motors Params */
    AX_S32 nZoom1HwMaxStep;                            /* Max Step of Zoom1 Motor Hardware */
    AX_S32 nZoom1HwMinPos;                             /* Min Position of Zoom1 Motor Hardware */
    AX_S32 nZoom1HwMaxPos;                             /* Max Position of Zoom1 Motor Hardware */
    AX_S32 nZoom2HwMaxStep;                            /* Max Step of Zoom2 Motor Hardware */
    AX_S32 nZoom2HwMinPos;                             /* Min Position of Zoom2 Motor Hardware */
    AX_S32 nZoom2HwMaxPos;                             /* Max Position of Zoom2 Motor Hardware */
    AX_S32 nZoomSecondHwPos;                           /* The position of the zoom posetable second or third line */
    AX_S32 nZoomSpiltPos;                              /* Critical positions in linear and nonlinear regions */
    AX_S32 nDefaultDist;                               /* Default initial object distance */
    AX_U8  nZoomRatioToPosTableRows;
    AX_F32 nZoomRatioToPosTable[LENS_TRACK_TABLE_MAX_ROW * 3];

    /* Object Distance */
    AX_U8  nDistLimitNear;                             /* Nearest Object Distance (Unit:cm) */
    AX_LENS_ZOOM_TRACK_TABLE_T tTrackTable;            /* Zoom Track Table */

    AX_U8  *pZoomSpeedStepTable;                       /* Step table of different zoom lengths. */
    AX_S32 *pZoomSpeedPosTable;                        /* zoom pos table of Segmenting Zoom Length. */

    AX_S32 *pCaliActTable;
    AX_S32 *pPreCaliTable;
#if 0
    /* zoom track Params */
    AX_F32 Kp;
    AX_F32 Ki;
    AX_F32 Kd;
    AX_U8  ztStepDivisor;
    AX_U8  MaxProbeStep;
    AX_U8  MinProbeStep;
    AX_S32 zoomSpiltPoint;
    AX_S32 coffProStep;
    AX_S64 coffFvDiff1;
    AX_S64 coffFvDiff2;
    AX_S64 coffFvDiff3;
    AX_F32 coffDistDiff1;
    AX_F32 coffDistDiff2;
    AX_F32 coffDistDiff3;
#endif
} AX_LENS_FZ_HW_PARAM_T;

typedef struct _AX_LENS_DCIRIS_PARAMS_T_ {
    AX_S32 kp;                                        /* 比例增益 */
    AX_S32 ki;                                        /* 积分增益*/
    AX_S32 kd;                                        /* 微分增益 */
    AX_U32 nMinPwmDuty;                               /* 最小 PWM 占空比 */
    AX_U32 nMaxPwmDuty;                               /* 最大 PWM 占空比 */
    AX_U32 nOpenPwmDuty;                              /* 光圈打开时的 PWM 占空比 */
} AX_LENS_DCIRIS_PARAMS_T;

typedef struct _AX_LENS_PIRIS_PARAMS_T_ {
    AX_U8  bStepFNOTableChange;                       /* P-Iris 步进电机位置与光圈 F 值映射表是否更新标志 */
    AX_U8  bZeroIsMax;                                /* P-Iris 步进电机 Step 0 是否对应最大光圈位置标志 */
    AX_U8  bFNOExValid;                               /* 对接 P-Iris 时， AE 分配路线是否采用更高精度的光圈 F 值等效增益标志*/
    AX_U32 nMaxIrisFNOGainTarget;                     /* 最大光圈 F 值等效增益目标值*/
    AX_U32 nMinIrisFNOGainTarget;                     /* 最小光圈 F 值等效增益目标值 */
    AX_U32 nTotalStep;                                /* P-Iris 步进电机的总步数 */
    AX_U32 nStepCount;                                /* P-Iris 步进电机的可用步数 */
    AX_U32 nStepFNOTable[LENS_IRIS_MAP_TABLE_NUM];    /* P-Iris 步进电机位置与 F 值映射表 */
    AX_LENS_IRIS_FNO_E eMaxIrisFNOTarget;             /* 最大光圈目标值 */
    AX_LENS_IRIS_FNO_E eMinIrisFNOTarget;             /* 最小光圈目标值*/
} AX_LENS_PIRIS_PARAMS_T;

typedef struct _AX_LENS_IRIS_HW_PARAM_T_ {
    AX_U8                       nEnable;              /* 自动光圈使能 1: 自动光圈 0：手动光圈 */
    AX_LENS_IRIS_TYPE_E         eIrisType;            /* 光圈类型， Fixed, DC-Iris 或 P-Iris */
    AX_LENS_IRIS_CONTROL_MODE_E eIrisMode;            /* 光圈的控制方式 */
    union {
        AX_LENS_IRIS_FNO_E      ePIrisFNO;            /* 手动光圈大小， 根据光圈 F 值进行区分， 仅支持 P-Iris， 不支持 DC-Iris */
        AX_U32                  nDcIrisHoldValue;     /* 自动光圈校正值， 用于 DC-Iris 的调试 */
    } tManualParam;
    union {
        AX_LENS_PIRIS_PARAMS_T  tPIrisParam;
        AX_LENS_DCIRIS_PARAMS_T tDcIrisParam;
    } tAutoParam;
} AX_LENS_IRIS_HW_PARAM_T;

typedef struct _AX_LENS_ICR_HW_PARAM_T_ {
    AX_LENS_ICR_CONTROL_MODE_E eIcrMode;
} AX_LENS_ICR_HW_PARAM_T;

typedef struct _AX_LENS_HW_PLATFORM_PARAM_T_ {
    AX_U8  nSpiBusNum;
    AX_U8  nSpiCsNum;
    AX_U16 nResetGpioNum;
    AX_U16 nSysClk; // Mhz
    AX_U8  nPlatformId;
    AX_U8  nPlatformFunc;
    AX_U8  nMotorId[AX_MOTOR_FUNC_MAX];
} AX_LENS_HW_PLATFORM_PARAM_T;

typedef struct _AX_LENS_HW_MOTOR_PARAM_T_ {
    /* Lens driver rules. CW: PI low-->high, position small-->large.
     * 1. if PI not follow, set nPiGpioNum LENS_INVERT flag
     * 2. if position not follow, set bPosIncreaseCcw = AX_TRUE */
    AX_U16 nPiGpioNum;
    AX_U16 nVdGpioNum;
    AX_U16 nMaxPPS;
    AX_U16 nBacklash;
    AX_U8  bHalfStepMode;
    AX_U8  bPosIncreaseCcw;
} AX_LENS_HW_MOTOR_PARAM_T;

typedef struct _AX_LENS_HW_CFG_PARAM_T_ {
    AX_U16 nLensEnableGpioNum;
    AX_U16 nPiEnableGpioNum;
    AX_U16 nPulsePeriodUs;
    AX_U8  nPlatformNum;
    AX_LENS_HW_PLATFORM_PARAM_T tPlatformParam[LENS_PLATFORM_NUM_MAX];
    AX_LENS_HW_MOTOR_PARAM_T tMotorParam[AX_MOTOR_FUNC_MAX];
} AX_LENS_HW_CFG_PARAM_T;

/************************************************************************************
 *  API
 ************************************************************************************/

typedef enum _AX_LENS_CTRL_CMD_E_ {
    AX_LENS_CMD_HW_PARAM_GET,
    AX_LENS_CMD_TUNING_PARAM_GET,

    AX_LENS_CMD_CONTROL_MODE_SET,
    AX_LENS_CMD_CUR_DIRECTION_GET,
    AX_LENS_CMD_COMPENSATION_GET,
    AX_LENS_CMD_IRIS_POS_GET,
    AX_LENS_CMD_MAX,
} AX_LENS_CTRL_CMD_E;

typedef struct _AX_LENS_FZ_MOVE_PARAM_T_ {
    AX_S32 tFocusTargetPos;
    AX_S32 tZoomTargetPos;
} AX_LENS_FZ_MOVE_PARAM_T;

typedef union _AX_LENS_IRIS_MOVE_PARAM_U_ {
    AX_S32 nTargetPos;
    AX_F32 nDuty;
} AX_LENS_IRIS_MOVE_PARAM_U;

typedef union _AX_LENS_ICR_MOVE_PARAM_U_ {
    AX_S32  nTargetPos;
    AX_BOOL bEnable;
} AX_LENS_ICR_MOVE_PARAM_U;

typedef struct _AX_LENS_FUNCS_T_ {
    AX_S32(*pfn_lens_init)(AX_U8 nPipeId);
    AX_S32(*pfn_lens_exit)(AX_U8 nPipeId);
    AX_S32(*pfn_lens_reset)(AX_U8 nPipeId);
    AX_S32(*pfn_lens_ctrl)(AX_U8 nPipeId, AX_LENS_CTRL_CMD_E eCmd, AX_VOID *pData);
    AX_S32(*pfn_lens_get_status_timeout)(AX_U8 nPipeId, AX_S32 nTimeOut);
    AX_S32(*pfn_lens_move)(AX_U8 nPipeId, AX_VOID *pMoveParam);
} AX_LENS_FUNCS_T;

#ifdef __cplusplus
}
#endif

#endif
