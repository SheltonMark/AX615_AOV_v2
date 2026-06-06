/**************************************************************************************************
 *
 * Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_VIN_KERNEL_EXT_H__
#define __AX_VIN_KERNEL_EXT_H__

#include "ax_base_type.h"

typedef enum {
    AX_VIN_INT_TYPE_SNS_FLSOF    = 0,
    AX_VIN_INT_TYPE_SNS_FSSOF    = 1,
    AX_VIN_INT_TYPE_SNS_FLEOF    = 2,
    AX_VIN_INT_TYPE_SNS_FSEOF    = 3,
    AX_VIN_INT_TYPE_MAX,
} ax_vin_interrupt_type_e;

typedef enum {
    AX_VIN_SNS_HDR_MODE_NONE    = 0,
    AX_VIN_SNS_HDR_LINEAR       = 1,
    AX_VIN_SNS_HDR_DOL_2        = 2,
    AX_VIN_SNS_HDR_DOL_3        = 3,
    AX_VIN_SNS_HDR_DOL_4        = 4,
    AX_VIN_SNS_HDR_MODE_BUT,
} ax_vin_sns_type_e;

typedef struct _ax_vin_int_notify_info_t_ {
    AX_U8   pipe_id;
    AX_U32  exp_time;
} ax_vin_int_notify_info_t;

typedef struct _ax_vin_int_notify_callback_t_ {
    AX_S32 (*pfn_vin_interrupt_event_handling)(ax_vin_interrupt_type_e int_type, AX_VOID *pdata);
} ax_vin_int_notify_callback_t;

typedef AX_S32 (*interrupt_callback)(ax_vin_interrupt_type_e int_type, AX_VOID *);



typedef struct _AX_VIN_KERNEL_EXPORT_FUNC_T_ {
    /* interrupt notification api */
    AX_S32 (*pfn_vin_int_notify_register_callback)(AX_U8 pipe_id, interrupt_callback cb);

    /*camera first frame info*/
    AX_S32 (*pfn_vin_sns_info_notify)(AX_U8 pipe_id, AX_U8 sns_id, AX_U8 sns_type, AX_BOOL first_frm_flag, AX_BOOL expo_inv_flag);

} AX_VIN_KERNEL_EXPORT_FUNC_T;


// AX_PROTON DRIVER EXPORT API
extern AX_VIN_KERNEL_EXPORT_FUNC_T ax_vin_kernel_ext_handle;// AX_PROTON DRIVER DEFINED

#define CB_VIN_INT_NOTIFY_RegisterCallback(pipe_id, interrupt_callback)\
    ax_vin_kernel_ext_handle.pfn_vin_int_notify_register_callback(pipe_id, interrupt_callback)

#define CB_VIN_SNS_INFO_NOTIFY(pipe_id, sns_id, lens_type, first_frm_flag, expo_inv_flag)\
    ax_vin_kernel_ext_handle.pfn_vin_sns_info_notify(pipe_id, sns_id, lens_type, first_frm_flag, expo_inv_flag)


#define CHECK_CB_VIN_INT_NOTIFY_RegisterCallback()\
    (AX_NULL != ax_vin_kernel_ext_handle.pfn_vin_int_notify_register_callback)

#define CHECK_CB_VIN_SNS_INFO_NOTIFY()\
    (AX_NULL != ax_vin_kernel_ext_handle.pfn_vin_sns_info_notify)

#endif //__AX_VIN_KERNEL_EXT_H__
