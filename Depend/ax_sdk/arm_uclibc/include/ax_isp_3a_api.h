/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/
//clang-format off
#ifndef _AX_ISP_3A_API_H_
#define _AX_ISP_3A_API_H_

#include "ax_base_type.h"
#include "ax_isp_api.h"
#include "ax_isp_3a_struct.h"
#include "ax_sensor_struct.h"
#include "ax_lens_struct.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * AWB (Auto White Balance) IQ Control APIs
 *****************************************************************************/

/**
 * @brief Enable or disable the Gray World AWB algorithm.
 *        When enabled (1), the algorithm uses gray-world assumption to estimate
 *        the white balance gains. When disabled (0), the normal AWB algorithm runs.
 * @param pipe ISP pipeline ID
 * @param nAWBMode 0: normal (disable special gray world), 1: enable special gray world
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAwbMode(AX_U8 pipe, int nAWBMode);

/**
 * @brief Set AWB IQ parameters (tuning + manual gains + algo config).
 *        Internally converts IQ params to algo config, resets convergence state,
 *        and triggers gain table update if tuning changed.
 *        Must be called after AX_ISP_ALG_AwbInit; protected by per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param pIspAwbParam Pointer to AWB parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAwbParam(AX_U8 pipe, AX_ISP_IQ_AWB_PARAM_T *pIspAwbParam);

/**
 * @brief Get current AWB IQ parameters.
 *        Reads back the stored IQ param from the algorithm context,
 *        including live status fields (e.g. current frame-rate mode).
 * @param pipe ISP pipeline ID
 * @param pIspAwbParam Output pointer to AWB parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAwbParam(AX_U8 pipe, AX_ISP_IQ_AWB_PARAM_T *pIspAwbParam);

/**
 * @brief Set AWB tuning parameters for adjustment.
 *        Updates the tuning sub-structure inside the AWB context;
 * @param pipe ISP pipeline ID
 * @param pIspAwbTuning Pointer to AWB tuning parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAwbTuningParam(AX_U8 pipe, AX_ISP_IQ_AWB_TUNING_T *pIspAwbTuning);

/**
 * @brief Get current AWB tuning parameters from the algorithm context.
 * @param pipe ISP pipeline ID
 * @param pIspAwbTuning Output pointer to AWB tuning parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAwbTuningParam(AX_U8 pipe, AX_ISP_IQ_AWB_TUNING_T *pIspAwbTuning);

/**
 * @brief Get current AWB run-time status (R/G/B gains, color temperature, etc.).
 *        Reads live values from the algorithm status context.
 * @param pipe ISP pipeline ID
 * @param pIspAwbStatus Output pointer to AWB status structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAwbStatus(AX_U8 pipe, AX_ISP_IQ_AWB_STATUS_T *pIspAwbStatus);

/**
 * @brief Initialize AWB sync functionality for multi-pipe splice mode.
 *        Enables the calibration flag for the configured stitch pipe pairs.
 *        Must be called after AX_ISP_ALG_AwbInit for both pipes.
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_AwbSyncInit();

/**
 * @brief Deinitialize AWB sync; disables multi-pipe splice calibration.
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_AwbSyncDeInit();

/**
 * @brief Perform AWB sync calibration to compute the luma ratio between
 *        the right edge of pipe-0 and the left edge of pipe-1 overlap region.
 *        Collects 20 frames of overlap luma and averages stable samples.
 * @param pAwbSyncRatio Output sync ratio (Q4.20 fixed-point)
 * @return 0 on success, -1 if overlap luma is invalid or pipes not initialized
 */
AX_S32 AX_ISP_IQ_AwbSyncCalibration(AX_ISP_IQ_AWB_SYNC_RATIO_T *pAwbSyncRatio);

/**
 * @brief Apply a previously calibrated AWB sync ratio to the algorithm context.
 *        The ratio is stored as a float in the multi-camera config.
 * @param pAwbSyncRatio Pointer to sync ratio structure to apply
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAwbSyncParam(AX_ISP_IQ_AWB_SYNC_RATIO_T *pAwbSyncRatio);

/**
 * @brief Read back the currently applied AWB sync ratio.
 * @param pAwbSyncRatio Output pointer to sync ratio structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAwbSyncParam(AX_ISP_IQ_AWB_SYNC_RATIO_T *pAwbSyncRatio);

/******************************************************************************
 * AWB Algorithm Sensor Registration APIs
 *****************************************************************************/

/**
 * @brief Register sensor callback functions for the AWB algorithm.
 *        Uses pipe ID as both pipe and handle ID (single-sensor shortcut).
 *        Requires pfn_sensor_get_3a_default_params to be non-null.
 * @param pipe ISP pipeline ID
 * @param pSensorHandle Pointer to sensor callback function structure
 * @return 0 on success, AX_3A_ALG_SNS_UNREGISTER on failure
 */
AX_S32 AX_ISP_ALG_AwbRegisterSensor(AX_U8 pipe, AX_SENSOR_REGISTER_FUNC_T *pSensorHandle);

/**
 * @brief Unregister the sensor callback for the AWB algorithm (single-sensor).
 * @param pipe ISP pipeline ID
 * @return 0 on success, AX_3A_ALG_SNS_UNREGISTER on failure
 */
AX_S32 AX_ISP_ALG_AwbUnRegisterSensor(AX_U8 pipe);

/**
 * @brief Register sensor callbacks for AWB with an explicit handle ID,
 *        allowing multiple sensors on the same pipe.
 * @param pipe ISP pipeline ID
 * @param hdlId Sensor handle ID (used as key in the callback table)
 * @param pSensorHandle Pointer to sensor callback function structure
 * @return 0 on success, AX_3A_ALG_SNS_UNREGISTER on failure
 */
AX_S32 AX_ISP_ALG_AwbRegisterSensorExt(AX_U8 pipe, AX_U8 hdlId, AX_SENSOR_REGISTER_FUNC_T *pSensorHandle);

/**
 * @brief Unregister the sensor callback for AWB by handle ID.
 * @param pipe ISP pipeline ID
 * @param hdlId Sensor handle ID to unregister
 * @return 0 on success, AX_3A_ALG_SNS_UNREGISTER on failure
 */
AX_S32 AX_ISP_ALG_AwbUnRegisterSensorExt(AX_U8 pipe, AX_U8 hdlId);

/******************************************************************************
 * AE (Auto Exposure) IQ Control APIs
 *****************************************************************************/

/**
 * @brief Set AE IQ parameters.
 *        Converts IQ params to internal algo config, updates sensor params,
 *        resets convergence / gain-table / HDR / fps-mode state as needed,
 *        and recalculates the apex exposure program.
 *        Protected by per-pipe mutex; requires AX_ISP_ALG_AeInit first.
 * @param pipe ISP pipeline ID
 * @param pIspAeParam Pointer to AE parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAeParam(AX_U8 pipe, AX_ISP_IQ_AE_PARAM_T *pIspAeParam);

/**
 * @brief Get current AE IQ parameters.
 *        Reads back config and live status (e.g. current frame-rate mode) from
 *        the algorithm context.
 * @param pipe ISP pipeline ID
 * @param pIspAeParam Output pointer to AE parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAeParam(AX_U8 pipe, AX_ISP_IQ_AE_PARAM_T *pIspAeParam);

/**
 * @brief Get current AE run-time status.
 *        Reports lux, mean luma, current exposure/gain settings,
 *        convergence state, HDR ratio, etc. via axae_ui_get_ae_status().
 * @param pipe ISP pipeline ID
 * @param pIspAeStatus Output pointer to AE status structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAeStatus(AX_U8 pipe, AX_ISP_IQ_AE_STATUS_T *pIspAeStatus);

/**
 * @brief Get hardware exposure limits from the sensor driver.
 *        Queries the sensor via axae_sns_get_hw_exposure_params() and fills
 *        min/max integration time, gain limits, etc.
 *        Useful when sensor FPS changes (e.g. 30fps->25fps max shutter changes).
 * @param pipe ISP pipeline ID
 * @param pIspAeHwLimit Output pointer to hardware limit structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAeHwLimit(AX_U8 pipe, AX_ISP_IQ_AE_EXP_HW_LIMIT_T *pIspAeHwLimit);

/**
 * @brief Enable AE splice-sync mode for the specified pipe.
 *        Sets nMultiCamSyncMode = SPLICE_MODE and nMultiCamCalibEn = TRUE
 *        on both this pipe and its stitch partner.
 *        Requires AX_ISP_ALG_AeInit to have been called on both pipes.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 if pipe not initialized or stitch partner missing
 */
AX_S32 AX_ISP_IQ_AeSyncInit(AX_U8 pipe);

/**
 * @brief Disable AE splice-sync calibration for the specified pipe.
 *        Clears nMultiCamCalibEn on both this pipe and its stitch partner.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 if pipe not initialized or stitch partner missing
 */
AX_S32 AX_ISP_IQ_AeSyncDeInit(AX_U8 pipe);

/**
 * @brief Compute the AE sync ratio between two splice pipes.
 *        Measures the overlap luma ratio between main and sub pipes over
 *        20 frames (40ms sleep between each), averages stable samples
 *        (adjacent-frame difference < 0.01), and returns result as Q4.20.
 * @param pipe ISP pipeline ID (can be main or sub)
 * @param pAeSyncRatio Output sync ratio (Q4.20 fixed-point)
 * @return 0 on success, -1 if overlap luma invalid or pipes not initialized
 */
AX_S32 AX_ISP_IQ_AeSyncCalibration(AX_U8 pipe, AX_ISP_IQ_AE_SYNC_RATIO_T *pAeSyncRatio);

/**
 * @brief Apply an AE sync ratio to the specified pipe's splice config.
 *        Converts Q4.20 fixed-point ratio to float and stores in
 *        fAeSpliceSyncRatio of the algorithm config.
 * @param pipe ISP pipeline ID
 * @param pAeSyncRatio Pointer to sync ratio to apply
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAeSyncParam(AX_U8 pipe, AX_ISP_IQ_AE_SYNC_RATIO_T *pAeSyncRatio);

/**
 * @brief Read back the currently applied AE sync ratio (Q4.20 fixed-point).
 * @param pipe ISP pipeline ID
 * @param pAeSyncRatio Output pointer to sync ratio structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAeSyncParam(AX_U8 pipe, AX_ISP_IQ_AE_SYNC_RATIO_T *pAeSyncRatio);

/**
 * @brief Set the detected-object ROI for face/person-priority AE.
 *        Validates that nObjectNum <= AX_ISP_IQ_AE_DETECT_OBJECT_MAX_NUM,
 *        then updates the object exposure status in the algorithm context.
 *        Protected by per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param pDetectObjectParam Pointer to object ROI parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAeDetectObjectROI(AX_U8 pipe, AX_ISP_IQ_AE_DETECT_OBJECT_PARAM_T *pDetectObjectParam);

/**
 * @brief Push the current AFD-detected flicker frequency to the ISP hardware.
 *        If AFD has not yet succeeded (nAfdSuccessFlag == FALSE), sets DISABLE;
 *        otherwise sets 100Hz or 120Hz based on nEcmTflicker value.
 *        Calls the registered pfnAeSetFlickerStatus callback.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetAeAntiFlickerMode(AX_U8 pipe);

/**
 * @brief Query the ISP hardware for the currently active anti-flicker mode.
 *        Calls the registered pfnAeGetFlickerStatus callback and updates
 *        nAfdSuccessFlag / nEcmTflicker in the algorithm status accordingly.
 * @param pipe ISP pipeline ID
 * @return 0 (status is stored in context, not returned as value)
 */
AX_S32 AX_ISP_IQ_GetAeAntiFlickerMode(AX_U8 pipe);

/**
 * @brief Set the long-frame HDR (LFHDR) mode for a 2X HDR pipe.
 *        Validates eLFHdrMode range and that the pipe is in HDR_2X_MODE,
 *        then stores the mode and propagates it to the sensor driver via
 *        ax_isp_sns_set_long_frame_mode() to update shutter range.
 *        Protected by per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param eLFHdrMode Long-frame HDR mode enum value
 * @return 0 on success, -1 if not in HDR 2X mode or invalid enum
 */
AX_S32 AX_ISP_IQ_SetAeLongFrameMode(AX_U8 pipe, AX_ISP_LFHDR_MODE_E eLFHdrMode);

/**
 * @brief Get the currently active long-frame HDR mode from the AE context.
 * @param pipe ISP pipeline ID
 * @param pELFHdrMode Output pointer to current LFHDR mode
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetAeLongFrameMode(AX_U8 pipe, AX_ISP_LFHDR_MODE_E *pELFHdrMode);

/******************************************************************************
 * AE Algorithm Scan / Calibration APIs
 *****************************************************************************/

/**
 * @brief Get the current AE scan state (running/finished) for tuning tools.
 *        Reads scanState from the linear-test sub-module status.
 * @param pipe ISP pipeline ID
 * @param pScanStatusToUI Output scan status
 * @return 0 on success, -1 if pScanStatusToUI is NULL
 */
AX_S32 AX_ISP_ALG_AeGetScanStatus(AX_U8 pipe, AX_ISP_IQ_AE_SCAN_STATUS *pScanStatusToUI);

/**
 * @brief Start an analog-gain scan for AE linearity calibration.
 *        Triggers the linear-test state machine to sweep AGain while
 *        keeping shutter and DGain fixed.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeAgainScanStart(AX_U8 pipe);

/**
 * @brief Start a digital-gain scan for AE linearity calibration.
 *        Sweeps DGain while keeping shutter and AGain fixed.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeDgainScanStart(AX_U8 pipe);

/**
 * @brief Start a shutter/integration-time scan for AE linearity calibration.
 *        Sweeps shutter time while keeping gain fixed.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeShutterScanStart(AX_U8 pipe);

/**
 * @brief Register flicker-status callbacks for the AFD (Auto Flicker Detection) module.
 *        Stores pfnAeSetFlickerStatus and pfnAeGetFlickerStatus in a module-level
 *        static struct; at least one must be non-NULL.
 * @param pipe ISP pipeline ID
 * @param pAfdFuncs Pointer to AFD callback function structure
 * @return 0 on success, -1 if both callbacks are NULL
 */
AX_S32 AX_ISP_ALG_AeAfdStatusCallback(AX_U8 pipe, AX_ISP_IQ_AE_AFD_STATUS_FUNC_T *pAfdFuncs);

/******************************************************************************
 * AE Algorithm Sensor / Iris Registration APIs
 *****************************************************************************/

/**
 * @brief Register sensor callbacks for the AE algorithm (single-sensor).
 *        Validates that set_again / set_dgain / set_integration_time callbacks
 *        are all non-NULL before registering.
 * @param pipe ISP pipeline ID
 * @param pSensorHandle Pointer to sensor callback function structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeRegisterSensor(AX_U8 pipe, AX_SENSOR_REGISTER_FUNC_T *pSensorHandle);

/**
 * @brief Unregister the sensor callback for the AE algorithm (single-sensor).
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeUnRegisterSensor(AX_U8 pipe);

/**
 * @brief Register sensor callbacks for AE with an explicit handle ID,
 *        supporting multiple sensors on the same pipe.
 * @param pipe ISP pipeline ID
 * @param hdlId Sensor handle ID
 * @param pSensorHandle Pointer to sensor callback function structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeRegisterSensorExt(AX_U8 pipe, AX_U8 hdlId, AX_SENSOR_REGISTER_FUNC_T *pSensorHandle);

/**
 * @brief Unregister the sensor callback for AE by handle ID.
 * @param pipe ISP pipeline ID
 * @param hdlId Sensor handle ID to unregister
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeUnRegisterSensorExt(AX_U8 pipe, AX_U8 hdlId);

/**
 * @brief Register lens iris (DC-iris or P-iris) callbacks for AE iris control.
 * @param pipe ISP pipeline ID
 * @param pLensIrisHandle Pointer to lens iris function structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeRegisterLensIris(AX_U8 pipe, AX_LENS_FUNCS_T *pLensIrisHandle);

/**
 * @brief Unregister the lens iris callbacks for AE.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeUnRegisterLensIris(AX_U8 pipe);

/******************************************************************************
 * AF (Auto Focus) IQ Control APIs
 *****************************************************************************/

/**
 * @brief Set CAF (Continuous Auto Focus) IQ parameters.
 *        Stores the parameter set in the AF algorithm context for use by CAfRun.
 * @param pipe ISP pipeline ID
 * @param pIspCafParam Pointer to CAF parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_SetCafParam(AX_U8 pipe, AX_ISP_IQ_CAF_PARAM_T *pIspCafParam);

/**
 * @brief Get current CAF IQ parameters from the algorithm context.
 * @param pipe ISP pipeline ID
 * @param pIspCafParam Output pointer to CAF parameter structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_IQ_GetCafParam(AX_U8 pipe, AX_ISP_IQ_CAF_PARAM_T *pIspCafParam);

/******************************************************************************
 * AF Algorithm Lens Control APIs
 *****************************************************************************/

/**
 * @brief Command the zoom motor to move to an absolute position.
 *        Updates zoomPosTarget and triggers the AF state machine transition
 *        to axaf_idle -> axaf_move_to_target. No-op if already at target.
 *        Protected by per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param pZoomPosFromUI Desired zoom position
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfSetZoomPos(AX_U8 pipe, AX_ISP_CAF_ZOOM_POS *pZoomPosFromUI);

/**
 * @brief Command the focus motor to move to an absolute position.
 *        Updates focusPosTarget and triggers axaf_move_to_target.
 *        Protected by per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param pFocusPosFromUI Desired focus position
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfSetFocusPos(AX_U8 pipe, AX_ISP_CAF_FOCUS_POS *pFocusPosFromUI);

/**
 * @brief Set the zoom ratio and trigger the zoom tracking state machine.
 *        Converts Q5.10 fixed-point ratio to float, then calls axaf_set_zoom_ratio().
 * @param pipe ISP pipeline ID
 * @param pLensZoomRatioUI Zoom ratio in Q5.10 fixed-point
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfSetLensRatio(AX_U8 pipe, AX_ISP_CAF_ZOOM_RATIO *pLensZoomRatioUI);

/**
 * @brief Read back the current zoom motor position from the context.
 * @param pipe ISP pipeline ID
 * @param pResultToUI Output current zoom position
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfGetCurrentZoomPos(AX_U8 pipe, AX_ISP_CAF_ZOOM_POS *pResultToUI);

/**
 * @brief Read back the current focus motor position from the context.
 * @param pipe ISP pipeline ID
 * @param pResultToUI Output current focus position
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfGetCurrentFocusPos(AX_U8 pipe, AX_ISP_CAF_FOCUS_POS *pResultToUI);

/**
 * @brief Start an FV (Focus Value) scan for AF tuning / calibration.
 *        If scanType==0, enters full range scan (axaf_scan_entry);
 *        otherwise starts hill-climbing auto-focus (axaf_climb_hill_entry).
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfFvScanStart(AX_U8 pipe);

/**
 * @brief Get the current AF scan state (running/finished) from the context.
 * @param pipe ISP pipeline ID
 * @param pscanstatus Output scan state
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfGetScanStatus(AX_U8 pipe, AX_ISP_CAF_SCAN_STATUS *pscanstatus);

/**
 * @brief Get the FV scan results (data count, peak position, per-step FV values).
 *        Copies the scan result buffer from the AF context.
 * @param pipe ISP pipeline ID
 * @param afParams Output FV scan information
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfGetScanInfo(AX_U8 pipe, AX_ISP_CAF_SCAN_FV_INFO *afParams);

/**
 * @brief Command the zoom motor to move toward the widest angle position
 *        (zoomHwMaxPos). No-op if already at max. Triggers axaf_zoom_wide_entry.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfZoomWideMove(AX_U8 pipe);

/**
 * @brief Command the zoom motor to move toward the telephoto position
 *        (zoomHwMinPos). No-op if already at min. Triggers axaf_zoom_tele_entry.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfZoomTeleMove(AX_U8 pipe);

/**
 * @brief Immediately stop all lens motor movement.
 *        Sets isStopMove = 1 in the status context; the motor stops on the
 *        next CAfRun() invocation.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfStopMove(AX_U8 pipe);

/**
 * @brief Start continuous manual focus movement toward infinity (FAR direction).
 *        Sets focusDirCur = FOCUS_DIR_FAR, focusStepCur = speed,
 *        then enters axaf_focus_manual_entry state.
 * @param pipe ISP pipeline ID
 * @param speed Step size per frame (0–255)
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfFarMove(AX_U8 pipe, AX_U8 speed);

/**
 * @brief Start continuous manual focus movement toward minimum focus distance (NEAR).
 *        Sets focusDirCur = FOCUS_DIR_NEAR, focusStepCur = speed,
 *        then enters axaf_focus_manual_entry state.
 * @param pipe ISP pipeline ID
 * @param speed Step size per frame (0–255)
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_AfNearMove(AX_U8 pipe, AX_U8 speed);

/**
 * @brief Set the zoom motor step speed used during manual zoom moves.
 *        Stored in the AF context and used by subsequent zoom state-machine calls.
 * @param pipe ISP pipeline ID
 * @param UIzoomspeed Zoom step speed (0–255)
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfSetZoomSpeed(AX_U8 pipe, AX_U8 UIzoomspeed);

/**
 * @brief Get the current AF state machine state (e.g. IDLE, SCANNING, TRACKING).
 *        Reads afstate from the context status.
 * @param pipe ISP pipeline ID
 * @param pAfModleStateToUser Output AF state value
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfGetModleState(AX_U8 pipe, AX_U8 *pAfModleStateToUser);

/**
 * @brief Reset the lens to its hardware home/init position.
 *        Calls axaf_init_lens() to re-run the lens initialization sequence.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfResetLens(AX_U8 pipe);

/**
 * @brief Convert the currently set zoom ratio to a physical motor target position.
 *        Calculates the zoom motor position that corresponds to the stored ratio
 *        and writes it to pZoomPosTarget.
 * @param pipe ISP pipeline ID
 * @param pZoomPosTarget Output target zoom position
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfRatioToTarget(AX_U8 pipe, AX_ISP_CAF_ZOOM_POS *pZoomPosTarget);

/**
 * @brief Perform a zoom-tracking curve correction pass.
 *        Re-evaluates the focus tracking curve for the current zoom position
 *        to compensate for mechanical tolerances.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AfCurveCorrect(AX_U8 pipe);

/**
 * @brief Register lens AF motor callbacks for the CAF algorithm.
 *        The callbacks are used by CAfRun to drive focus/zoom motors.
 * @param pipe ISP pipeline ID
 * @param pLensAfHandle Pointer to lens AF function structure
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_CAfRegisterLensAf(AX_U8 pipe, AX_LENS_FUNCS_T *pLensAfHandle);

/**
 * @brief Unregister the lens AF callbacks for the CAF algorithm.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_CAfUnRegisterLensAf(AX_U8 pipe);

/**
 * @brief Post-process AF statistics to suppress over-exposed / invalid grid zones.
 *        For each grid zone in the specified channel, computes a validity mask
 *        based on high-luma pixel ratio and relative pixel-value thresholds,
 *        then zeroes out nFocusValue / nPixCount / nPixSum for masked zones.
 *        Operates in-place on pAfStat.
 * @param pipe ISP pipeline ID
 * @param nChannel AF statistics channel (V1/V2/H1/H2)
 * @param pAfStat AF statistics buffer (modified in-place)
 * @return 0 on success, -1 on invalid arguments or memory allocation failure
 */
AX_S32 AX_ISP_IQ_AfsPostProcess(AX_U8 pipe, AX_ISP_IQ_AF_CHANNEL_T nChannel, AX_ISP_AF_STAT_INFO_T *pAfStat);

/******************************************************************************
 * Callback Functions Called by the AX Platform 3A Framework
 *****************************************************************************/

/**
 * @brief Allocate and initialize per-pipe AE algorithm context.
 *        Initialises the per-pipe mutex, allocates AX_ISP_ALG_AE_CTX_T,
 *        registers IQ-bin callback, and loads sensor default parameters.
 *        Must be called before AX_ISP_ALG_AeInit.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on allocation or mutex failure
 */
AX_S32 AX_ISP_ALG_AeCreate(AX_U8 pipe);

/**
 * @brief Free per-pipe AE algorithm context and unregister IQ-bin callback.
 *        Frees gain table, IQ param buffer, and context memory.
 *        Destroys the per-pipe mutex. Must be called after AX_ISP_ALG_AeDeInit.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeDestroy(AX_U8 pipe);

/**
 * @brief Initialize the AE algorithm for a specific pipeline.
 *        Steps: init config from IQ params, init AFD (if auto mode), get sensor
 *        hw params, init manual exposure config, read AE stat params, init
 *        context status, build apex exposure program, optionally set initial
 *        exposure from lux (bUseInitLux), drive the sensor to the initial
 *        exposure setting, and init IRIS.
 *        Returns the initial AE result in pAeResult.
 * @param pipe ISP pipeline ID
 * @param pAeInitParam Pointer to AE initialization attributes
 * @param pAeResult Output initial AE result (exposure/gain settings)
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeInit(AX_U8 pipe, AX_ISP_AE_INITATTR_T *pAeInitParam, AX_ISP_AE_RESULT_T *pAeResult);

/**
 * @brief Deinitialize the AE algorithm (release iris, AFD, multi-camera resources).
 *        Does NOT free the context memory; call AX_ISP_ALG_AeDestroy for that.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeDeInit(AX_U8 pipe);

/**
 * @brief Execute one frame of the AE algorithm.
 *        Skips frames according to nSkipNum. Each processed frame:
 *        updates real-effect and sensor-reg exposure params, detects invalid
 *        grid stats (triggers re-init), updates luma/histogram, estimates BV
 *        and lux, runs AFD if in auto mode, rebuilds the apex exposure program
 *        (with anti-flicker nodes if needed), computes convergence and the next
 *        exposure setting via the APEX lifecycle, drives the sensor, and fills
 *        pAeResult. Thread-safe via per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param pAeInputInfo AE statistics and current exposure setting from the platform
 * @param pAeResult Output AE result for the next frame
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeRun(AX_U8 pipe, AX_ISP_AE_INPUT_INFO_T *pAeInputInfo, AX_ISP_AE_RESULT_T *pAeResult);

/**
 * @brief Send a control command to the AE algorithm (e.g. lock, unlock, scene change).
 *        Dispatches to axae_ctrl_cmd() inside the per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param nAeCtrlCmd Control command enum value
 * @param pAeResult AE result structure (may be updated by the command)
 * @param pValue Command-specific parameter pointer (may be NULL)
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AeCtrl(AX_U8 pipe, AX_ISP_CTRL_CMD_E nAeCtrlCmd, AX_ISP_AE_RESULT_T *pAeResult, void *pValue);

/**
 * @brief Initialize the AFD (Auto Flicker Detection) sub-module for a pipe.
 *        Calls axae_afd_init() on the AFD status embedded in the AE context.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_Ae_AFDInit(AX_U8 pipe);

/**
 * @brief Deinitialize the AFD sub-module and release its resources.
 *        Calls axae_afd_deinit(). Safe to call even if context is NULL.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_Ae_AFDDeInit(AX_U8 pipe);

/**
 * @brief Run the AFD algorithm to detect scene flicker frequency.
 *        Analyzes AE input statistics (luma histogram) together with the
 *        current lux and sensor line time to identify 50Hz/60Hz flicker.
 *        Results are stored internally and retrieved via GetAeAntiFlickerMode.
 * @param pipe ISP pipeline ID
 * @param pAeInputInfo AE statistics from the current frame
 * @param refVal Scene lux or luma reference value for frequency estimation
 * @param fLineTime Sensor line time in microseconds
 * @param pAFDParam AFD tuning parameters
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_Ae_AFDRun(AX_U8 pipe, AX_ISP_AE_INPUT_INFO_T *pAeInputInfo, AX_F32 refVal, AX_F32 fLineTime,
                            AX_ISP_IQ_AE_AFD_PARAMS_T *pAFDParam);

/**
 * @brief Allocate and initialize the per-pipe AWB algorithm context.
 *        Allocates AX_ISP_ALG_AWB_CTX_T, initialises mutex, allocates IQ param
 *        buffer, registers IQ-bin callback, and loads sensor default AWB params.
 *        Must be called before AX_ISP_ALG_AwbInit.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on allocation or mutex failure
 */
AX_S32 AX_ISP_ALG_AwbCreate(AX_U8 pipe);

/**
 * @brief Free per-pipe AWB algorithm context and unregister IQ-bin callback.
 *        Deinitialises status, tuning, and calibration sub-structures,
 *        then frees context memory. Must be called after AX_ISP_ALG_AwbDeInit.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AwbDestroy(AX_U8 pipe);

/**
 * @brief Initialize the AWB algorithm for a specific pipeline.
 *        Validates that tuning parameters are loaded (from sensor default or
 *        loadbin), checks MLC validity, initialises calibration, tuning, and
 *        status sub-structures, sets initial R/G/B gains from the manual param,
 *        and computes the initial color temperature.
 * @param pipe ISP pipeline ID
 * @param pAwbInitParam Pointer to AWB initialization attributes
 * @param pAwbResult Output initial AWB result (R/G/B gains and color temperature)
 * @return 0 on success, -1 if tuning params not loaded or MLC invalid
 */
AX_S32 AX_ISP_ALG_AwbInit(AX_U8 pipe, AX_ISP_AWB_INITATTR_T *pAwbInitParam, AX_ISP_AWB_RESULT_T *pAwbResult);

/**
 * @brief Execute one frame of the AWB algorithm.
 *        Processes AWB statistics, runs the white-balance estimation algorithm,
 *        applies multi-camera sync ratio if in splice mode, and fills pAwbResult
 *        with R/G/B/Gr/Gb gains and color temperature.
 *        Thread-safe via per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param pAwbInputInfo AWB statistics from the current frame
 * @param pAwbResult Output AWB result for the next frame
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AwbRun(AX_U8 pipe, AX_ISP_AWB_INPUT_INFO_T *pAwbInputInfo, AX_ISP_AWB_RESULT_T *pAwbResult);

/**
 * @brief Send a control command to the AWB algorithm (e.g. lock, scene change).
 *        Dispatches to axawb_ctrl_cmd() inside the per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @param nAwbCtrlCmd Control command enum value
 * @param pAwbResult AWB result structure (may be updated by the command)
 * @param pValue Command-specific parameter pointer (may be NULL)
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AwbCtrl(AX_U8 pipe, AX_ISP_CTRL_CMD_E nAwbCtrlCmd, AX_ISP_AWB_RESULT_T *pAwbResult, void *pValue);

/**
 * @brief Deinitialize the AWB algorithm sub-structures (status, tuning, calibration).
 *        Does NOT free the context memory; call AX_ISP_ALG_AwbDestroy for that.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_AwbDeInit(AX_U8 pipe);

/**
 * @brief Initialize the CAF (Continuous Auto Focus) algorithm for a pipe.
 *        Allocates AX_ISP_ALG_AFC_CTX_T, registers AF IQ-bin callback,
 *        initialises the AF debug module, checks for registered lens callbacks,
 *        calls axaf_init_lens() if a lens is registered, and initialises the
 *        context status (state machine, FV history, etc.).
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on allocation or lens init failure
 */
AX_S32 AX_ISP_ALG_CAfInit(AX_U8 pipe);

/**
 * @brief Execute one frame of the CAF algorithm.
 *        Polls the motor status; if finished, updates AF stats, weighted FV,
 *        SAD/RSAD, scan results, then runs the CAF state machine callback and
 *        drives the motor. Skips frames when the motor is still moving.
 *        Thread-safe via per-pipe mutex.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure; 0 (no-op) if lens not registered
 */
AX_S32 AX_ISP_ALG_CAfRun(AX_U8 pipe);

/**
 * @brief Deinitialize the CAF algorithm: frees weight LUT, unregisters IQ-bin
 *        callback, exits the lens FZ driver, decrements valid pipe count,
 *        and frees the context.
 * @param pipe ISP pipeline ID
 * @return 0 on success, -1 on failure
 */
AX_S32 AX_ISP_ALG_CAfDeInit(AX_U8 pipe);

#ifdef __cplusplus
}
#endif

#endif  //_AX_ISP_3A_H_
