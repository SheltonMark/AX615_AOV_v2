/*
    用户实现本文件中定义的各个功能回调函数，即实现了具体的业务功能.
    回调函数实现注意事项:

    1.在回调函数中，不能做阻塞式操作，禁止在回调函数中做耗时的操作;
    2.在非回调函数中访问物模型对象资源，应该使用ivm_lock()/ivm_unlock()加锁和解锁;
    3.在回调函数处理中，不要进行加锁/解锁操作。（因为通讯线程已经加锁了）
*/

#include <assert.h>
#include <stdio.h>
#include "iv_usrex.h"
#include "iv_cm.h"
#include "app/cloud/tencent/tencent_generated_callback_dispatcher.h"

static int aov_push_array_property(const char *property_id, const TYPE_DEF_TEMPLATE_ARRAY *value)
{
    return aov_tencent_generated_push_property(property_id, value != NULL ? value : "", 0, 0);
}

static int aov_push_bool_property(const char *property_id, const TYPE_DEF_TEMPLATE_BOOL *value)
{
    return aov_tencent_generated_push_property(
        property_id,
        (value != NULL && *value != 0) ? "true" : "false",
        0,
        0);
}

static int aov_push_enum_property(const char *property_id, const TYPE_DEF_TEMPLATE_ENUM *value)
{
    char buffer[32];
    if (value == NULL) {
        return -1;
    }
    snprintf(buffer, sizeof(buffer), "%d", (int)*value);
    return aov_tencent_generated_push_property(property_id, buffer, 0, 0);
}

static int aov_push_action(const char *action_id, const char *params)
{
    return aov_tencent_generated_push_action(action_id, params != NULL ? params : "", "", 0);
}

int iv_usrcb_ProWritable_record_enable(const TYPE_DEF_TEMPLATE_BOOL *record_enable)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_key_user_1(const TYPE_DEF_TEMPLATE_STRING *key_user_1)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_key_user_2(const TYPE_DEF_TEMPLATE_STRING *key_user_2)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_VideoColor(const TYPE_DEF_TEMPLATE_ARRAY *VideoColor)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable__set_attribute(const TYPE_DEF_TEMPLATE_STRING *_set_attribute)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_VideoBitrate(const TYPE_DEF_TEMPLATE_ARRAY *VideoBitrate)
{
    return aov_push_array_property("VideoBitrate", VideoBitrate);
}


int iv_usrcb_ProWritable_DeviceInputVolume(const TYPE_DEF_TEMPLATE_INT *DeviceInputVolume)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_DeviceOutputVolume(const TYPE_DEF_TEMPLATE_INT *DeviceOutputVolume)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_ChannelOSD(const TYPE_DEF_TEMPLATE_ARRAY *ChannelOSD)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_TDLogoOSD(const TYPE_DEF_TEMPLATE_ARRAY *TDLogoOSD)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_PowerLineFrequency(const TYPE_DEF_TEMPLATE_ENUM *PowerLineFrequency)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_VoiceIntercomMode(const TYPE_DEF_TEMPLATE_ENUM *VoiceIntercomMode)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_OneKeyCallSwitch(const TYPE_DEF_TEMPLATE_BOOL *OneKeyCallSwitch)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_AudioEncoding(const TYPE_DEF_TEMPLATE_ARRAY *AudioEncoding)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_CruiseFrequencyLevel(const TYPE_DEF_TEMPLATE_ARRAY *CruiseFrequencyLevel)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_CruiseTimePlan(const TYPE_DEF_TEMPLATE_ARRAY *CruiseTimePlan)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_PtzStation(const TYPE_DEF_TEMPLATE_ARRAY *PtzStation)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_PtzSpeed(const TYPE_DEF_TEMPLATE_ARRAY *PtzSpeed)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_OneClickMasking(const TYPE_DEF_TEMPLATE_ARRAY *OneClickMasking)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_VideoResolution(const TYPE_DEF_TEMPLATE_ARRAY *VideoResolution)
{
    return aov_push_array_property("VideoResolution", VideoResolution);
}


int iv_usrcb_ProWritable_VideoBitRateControl(const TYPE_DEF_TEMPLATE_ARRAY *VideoBitRateControl)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_VideoFPS(const TYPE_DEF_TEMPLATE_ARRAY *VideoFPS)
{
    return aov_push_array_property("VideoFPS", VideoFPS);
}


int iv_usrcb_ProWritable_DevBindStatus(const TYPE_DEF_TEMPLATE_BOOL *DevBindStatus)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_TimedCruiseStatus(const TYPE_DEF_TEMPLATE_ARRAY *TimedCruiseStatus)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_TimeZoneSetting(const ivm_TimeZoneSetting *TimeZoneSetting)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_StatusLightSwitch(const TYPE_DEF_TEMPLATE_BOOL *StatusLightSwitch)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_CloudStorageSwitch(const TYPE_DEF_TEMPLATE_BOOL *CloudStorageSwitch)
{
    return aov_push_bool_property("CloudStorageSwitch", CloudStorageSwitch);
}


int iv_usrcb_ProWritable_ImageFlipState(const TYPE_DEF_TEMPLATE_ENUM *ImageFlipState)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_StorageRecordMode(const TYPE_DEF_TEMPLATE_ENUM *StorageRecordMode)
{
    return aov_push_enum_property("StorageRecordMode", StorageRecordMode);
}


int iv_usrcb_ProWritable_FormatStorageMediumProgress(const TYPE_DEF_TEMPLATE_INT *FormatStorageMediumProgress)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_StorageStatus(const TYPE_DEF_TEMPLATE_ENUM *StorageStatus)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_WifiConfiguration(const ivm_WifiConfiguration *WifiConfiguration)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_GetWiFiInfo(const TYPE_DEF_TEMPLATE_ARRAY *GetWiFiInfo)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_AntiFlickerSwitch(const TYPE_DEF_TEMPLATE_ARRAY *AntiFlickerSwitch)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_BackLightShooting(const TYPE_DEF_TEMPLATE_ARRAY *BackLightShooting)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_RTMPInfo(const ivm_RTMPInfo *RTMPInfo)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_DeciceResolution(const TYPE_DEF_TEMPLATE_STRING *DeciceResolution)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_DynamicInformation(const ivm_DynamicInformation *DynamicInformation)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_DayNightMode(const TYPE_DEF_TEMPLATE_ENUM *DayNightMode)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_TimingReboot(const ivm_TimingReboot *TimingReboot)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_NightVisionMode(const TYPE_DEF_TEMPLATE_ARRAY *NightVisionMode)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_WhiteLightBrightness(const TYPE_DEF_TEMPLATE_ARRAY *WhiteLightBrightness)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_SnapshotStatus(const ivm_SnapshotStatus *SnapshotStatus)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_TimingShield(const TYPE_DEF_TEMPLATE_ARRAY *TimingShield)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_Onvif(const ivm_Onvif *Onvif)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_HomeGuardSwitch(const TYPE_DEF_TEMPLATE_BOOL *HomeGuardSwitch)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_HumanDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *HumanDetectConfig)
{
    return aov_push_array_property("HumanDetectConfig", HumanDetectConfig);
}


int iv_usrcb_ProWritable_SoundLightAlarmConfig(const TYPE_DEF_TEMPLATE_ARRAY *SoundLightAlarmConfig)
{
    return aov_push_array_property("SoundLightAlarmConfig", SoundLightAlarmConfig);
}


int iv_usrcb_ProWritable_CarDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *CarDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_BikeDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *BikeDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_PetDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *PetDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_ImageDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *ImageDetectConfig)
{
    return aov_push_array_property("ImageDetectConfig", ImageDetectConfig);
}


int iv_usrcb_ProWritable_HideDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *HideDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_CryDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *CryDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_MainStreamBitrate(const ivm_MainStreamBitrate *MainStreamBitrate)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_VideoEncoding(const TYPE_DEF_TEMPLATE_ARRAY *VideoEncoding)
{
    return aov_push_array_property("VideoEncoding", VideoEncoding);
}


int iv_usrcb_ProWritable_DeviceLogFileUpload(const ivm_DeviceLogFileUpload *DeviceLogFileUpload)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_DeviceLogRedirect(const ivm_DeviceLogRedirect *DeviceLogRedirect)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_ImageMode(const TYPE_DEF_TEMPLATE_ENUM *ImageMode)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_RegionDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *RegionDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_ProWritable_OverLineDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *OverLineDetectConfig)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_TextQuery(ivm_TextQuery_t *TextQuery)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_SetDefaultVideoColor(ivm_SetDefaultVideoColor_t *SetDefaultVideoColor)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_OneKeyAlarm(ivm_OneKeyAlarm_t *OneKeyAlarm)
{
    (void)OneKeyAlarm;
    return aov_push_action("OneKeyAlarm", "duration=10");
}


int iv_usrcb_Action_ReplyCall(ivm_ReplyCall_t *ReplyCall)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_FormatStorageMedium(void *arg)
{
    (void)arg;
    return aov_push_action("FormatStorageMedium", "medium=sdcard");
}


int iv_usrcb_Action_Reboot(void *arg)
{
    (void)arg;
    return aov_push_action("Reboot", "delay=0");
}


int iv_usrcb_Action_StartPTZCruise(ivm_StartPTZCruise_t *StartPTZCruise)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_AddPTZCruise(ivm_AddPTZCruise_t *AddPTZCruise)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_DelPTZCruise(ivm_DelPTZCruise_t *DelPTZCruise)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_PreviewImageCapture(ivm_PreviewImageCapture_t *PreviewImageCapture)
{
    (void)PreviewImageCapture;
    return aov_push_action("PreviewImageCapture", "stream=main");
}


int iv_usrcb_Action_ResetPTZ(ivm_ResetPTZ_t *ResetPTZ)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_GotoPTZStation(ivm_GotoPTZStation_t *GotoPTZStation)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_SetDefaultDevConfigs(void *arg)
{
    (void)arg;
    return aov_push_action("SetDefaultDevConfigs", "");
}


int iv_usrcb_Action_ManualTimeCalibration(ivm_ManualTimeCalibration_t *ManualTimeCalibration)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_CloudStorageStaus(ivm_CloudStorageStaus_t *CloudStorageStaus)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_DeviceFileUpload(ivm_DeviceFileUpload_t *DeviceFileUpload)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_DeviceFileUploadStatus(ivm_DeviceFileUploadStatus_t *DeviceFileUploadStatus)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_BindOrDelete(ivm_BindOrDelete_t *BindOrDelete)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_TwecallActivate(ivm_TwecallActivate_t *TwecallActivate)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}


int iv_usrcb_Action_OnSwitchingSimCards(void *arg)
{
    //User implementation code
    //注意: 回调函数中,不能做阻塞式操作,不得做耗时的操作。会导致核心通讯线程阻塞!!!
    return 0;
}
