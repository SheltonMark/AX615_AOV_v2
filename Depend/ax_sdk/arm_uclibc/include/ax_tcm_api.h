/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_TCM_API_H__
#define __AX_TCM_API_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "ax_base_type.h"
#include "ax_sensor_struct.h"

#define AX_TCM_LIGHT_NUM_MAX            (4)

typedef enum _ax_tcm_sync_polarity_e {
    AX_TCM_SYNC_POLARITY_HIGH = 0,        /* the valid horizontal/vertical synchronization signal is high-level */
    AX_TCM_SYNC_POLARITY_LOW,             /* the valid horizontal/vertical synchronization signal is low-level */
} ax_tcm_sync_polarity_e;

typedef struct _ax_tcm_sync_signal_attr_t {
    int                         nSyncIdx;
    ax_tcm_sync_polarity_e      eSyncInv;           /* the valid sync signal polarity */
    int                         nSyncFreq;          /* the sync signal frequency */
    int                         nSyncDutyRatio;     /* the sync signal duty ratio */
} ax_tcm_sync_signal_attr_t;

typedef enum _ax_tcm_sensor_workmode_e {
    AX_TCM_SNS_WORKMODE_LOW_POWER = 0,
    AX_TCM_SNS_WORKMODE_NORMAL,
} ax_tcm_sensor_workmode_e;

typedef struct _ax_tcm_sensor_info_t_ {
    AX_U8                       pipe_id;
    AX_U8                       mclk_idx;           /* sensor mclk index */
    AX_SNS_CLK_RATE_E           mclk_freq;          /* sensor mclk frequency */
    AX_SNS_CONNECT_TYPE_E       sns_type;
    AX_SNS_COMMBUS_T            com_bus;
    AX_SNS_STREAM_CTRL_T        stream_ctrl;
    ax_tcm_sensor_workmode_e    work_mode;
    AX_U32                      cap_frame_num;      /* AOV scene, the number of frames to per second. */
    AX_U32                      vts_val;
} ax_tcm_sensor_info_t;

typedef struct _ax_tcm_sensor_stream {
    AX_U8                       pipe_id;
    AX_BOOL                     enable;
} ax_tcm_sensor_stream_t;

typedef struct _ax_tcm_sensor_sleep {
    AX_U8                       pipe_id;
    AX_U32                      exp_time;           /* unit: us */
} ax_tcm_sensor_sleep_t;

typedef struct _ax_tcm_sensor_status {
    AX_U8                       pipe_id;
    /* The flag of early exposure, 1: pre-exposure enabled, 0: normal exposure */
    AX_U8                       pre_exposure_flag;
    AX_U32                      awake_sof_cnt;      /* Number of Start of Frame(SOF) in the awake state */
    AX_U32                      awake_eof_cnt;      /* Number of End of Frame(EOF) in the awake state */
} ax_tcm_sensor_status_t;


typedef enum {
    AX_TCM_LIGHT_MODE_CONST,    // Always-On Mode
    AX_TCM_LIGHT_MODE_PWM,      // PWM dimming mode (frequency and duty cycle can be set)
    AX_TCM_LIGHT_MODE_STROBE    // Flash mode (instantaneous highlight)
} AX_TCM_LIGHT_MODE_E;

typedef enum {
    AX_TCM_LIGHT_CTRL_MODE_ALWAYS_ON = 0,
    AX_TCM_LIGHT_CTRL_MODE_DELAY_ON_INSTANT_OFF,
    AX_TCM_LIGHT_CTRL_MODE_DELAY_ON_DELAY_OFF,
} AX_TCM_LIGHT_CTRL_MODE_E;

typedef struct _ax_tcm_light_init {
    AX_U8                       pipe_id;
    AX_U8                       light_id;
} ax_tcm_light_init_t;

typedef struct {
    AX_U8                       pipe_id;
    AX_TCM_LIGHT_MODE_E         mode;
    AX_TCM_LIGHT_CTRL_MODE_E    ctrl_mode;
    union {
        struct {
            AX_U16      level;      ///< brightness level 0~1000
        } const_cfg;
        struct {
            AX_U16      level;      // PWM brightness level 0~1000
            AX_U16      duty_pct;   // PWM duty cycle��0~100 %��
            AX_U32      freq_hz;    // PWM frequency
        } pwm_cfg;
        struct {
            AX_U16      duration_ms;
            AX_U16      interval_ms;
        } strobe_cfg;
    };
} ax_tcm_light_config_t;

/* light_id represents the PWM number or the GPIO number */
int ax_tcm_light_init(AX_U8 pipe, AX_U8 light_id);
int ax_tcm_light_deinit(AX_U8 pipe);
int ax_tcm_light_set_config(const ax_tcm_light_config_t *light_cfg);
int ax_tcm_light_get_config(AX_U8 pipe, ax_tcm_light_config_t *light_cfg);
int ax_tcm_light_enable(AX_U8 pipe);
int ax_tcm_light_disable(AX_U8 pipe);

int ax_tcm_sns_init(AX_VOID);
int ax_tcm_sns_deinit(AX_VOID);
int ax_tcm_sns_register_sensor(AX_U8 pipe, AX_SENSOR_REGISTER_FUNC_T *ptSnsHdl);
int ax_tcm_sns_info_set(AX_U8 pipe, ax_tcm_sensor_info_t *pSensorInfo);
int ax_tcm_sns_info_get(AX_U8 pipe, ax_tcm_sensor_info_t *pSensorInfo);
int ax_tcm_sns_stream_ctrl(AX_U8 pipe, AX_BOOL enable);
int ax_tcm_sns_sleep(AX_U8 pipe);
int ax_tcm_sns_wakeup(AX_U8 pipe);
int ax_tcm_sns_status_query(AX_U8 pipe, ax_tcm_sensor_status_t *pSensorStatus);

int ax_tcm_vsync_attr_set(ax_tcm_sync_signal_attr_t *pSyncAttr);
int ax_tcm_vsync_attr_get(ax_tcm_sync_signal_attr_t *pSyncAttr);
int ax_tcm_hsync_attr_set(ax_tcm_sync_signal_attr_t *pSyncAttr);
int ax_tcm_hsync_attr_get(ax_tcm_sync_signal_attr_t *pSyncAttr);
int ax_tcm_vsync_enable(AX_U8 index,  int enable);
int ax_tcm_hsync_enable(AX_U8 index,  int enable);


#ifdef __cplusplus
}
#endif

#endif
