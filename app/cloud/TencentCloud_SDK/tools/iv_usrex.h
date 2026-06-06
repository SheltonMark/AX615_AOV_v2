#ifndef __IVM_USREX_H_
#define __IVM_USREX_H_

#include "qcloud_iot_export.h"
#include "iv_def.h"
#include "iv_dm.h"

#ifdef __cplusplus
extern "C"
{
#endif

/*
以下函数由开发者应用层去实现，实现具体的业务功能:
1. 云平台在接收到对设备的设置操作(更改ProWritable)或控制操作(Action)时,会主动通知设备;
2. 设备SDK通过iv_usrcb_ProWritable_xxx/iv_usrcb_Action_xxx系列函数通知应用层;
3. 函数的参数，即为对应物模型对象的指针;
*/
#define MAX_ARRAY_JSON_STR_LEN (2048)
typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_DataChannelType;
        TYPE_DEF_TEMPLATE_STRING m_Data[2048+1];
    }action_in;
    struct {
        TYPE_DEF_TEMPLATE_INT m_DataChannelType;
        TYPE_DEF_TEMPLATE_STRING m_Data[2048+1];
    }action_out;
}ivm_TextQuery_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
    }action_in;
}ivm_SetDefaultVideoColor_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
        TYPE_DEF_TEMPLATE_BOOL m_Operate;
    }action_in;
}ivm_OneKeyAlarm_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_DataChannelType;
        TYPE_DEF_TEMPLATE_ENUM m_ReplyType;
    }action_in;
}ivm_ReplyCall_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
        TYPE_DEF_TEMPLATE_INT m_PresetID;
    }action_in;
}ivm_StartPTZCruise_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
        TYPE_DEF_TEMPLATE_INT m_PresetID;
        TYPE_DEF_TEMPLATE_STRING m_Presetname[2048+1];
    }action_in;
}ivm_AddPTZCruise_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
        TYPE_DEF_TEMPLATE_INT m_PresetID;
    }action_in;
}ivm_DelPTZCruise_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
    }action_in;
}ivm_PreviewImageCapture_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
    }action_in;
}ivm_ResetPTZ_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_INT m_ChannelID;
    }action_in;
}ivm_GotoPTZStation_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_TIME m_UTCTime;
    }action_in;
}ivm_ManualTimeCalibration_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_BOOL m_status;
    }action_in;
}ivm_CloudStorageStaus_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_STRING m_FileName[2048+1];
        TYPE_DEF_TEMPLATE_INT m_ChunkSizeMB;
    }action_in;
}ivm_DeviceFileUpload_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_STRING m_FileName[2048+1];
        TYPE_DEF_TEMPLATE_ENUM m_Status;
        TYPE_DEF_TEMPLATE_INT m_TotalChunks;
        TYPE_DEF_TEMPLATE_INT m_CompletedChunks;
        TYPE_DEF_TEMPLATE_STRING m_ErrorMsg[2048+1];
    }action_out;
}ivm_DeviceFileUploadStatus_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_ENUM m_command;
    }action_in;
}ivm_BindOrDelete_t;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_ENUM m_ActivateStatus;
    }action_in;
}ivm_TwecallActivate_t;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_DeviceType[32+1];
    TYPE_DEF_TEMPLATE_STRING m_DeviceUuid[32+1];
    TYPE_DEF_TEMPLATE_STRING m_WifiName[32+1];
    TYPE_DEF_TEMPLATE_STRING m_SystemVersion[64+1];
    TYPE_DEF_TEMPLATE_STRING m_IpAddress[16+1];
    TYPE_DEF_TEMPLATE_STRING m_MacAddress[32+1];
    TYPE_DEF_TEMPLATE_STRING m_DeviceSn[128+1];
    TYPE_DEF_TEMPLATE_STRING m_PublishTime[32+1];
    TYPE_DEF_TEMPLATE_INT m_ShowIpMac;
} ivm_DeviceInformation;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_b720P;
    TYPE_DEF_TEMPLATE_BOOL m_b1080P;
    TYPE_DEF_TEMPLATE_BOOL m_b3MP;
    TYPE_DEF_TEMPLATE_BOOL m_b4MP;
    TYPE_DEF_TEMPLATE_BOOL m_b5MP;
} ivm_ResolutionFunctionSet;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_Reboot;
    TYPE_DEF_TEMPLATE_INT m_TimingReboot;
    TYPE_DEF_TEMPLATE_INT m_SpeedPlaySd;
    TYPE_DEF_TEMPLATE_INT m_SingleTalk;
    TYPE_DEF_TEMPLATE_INT m_OTAUpgradeProgress;
    TYPE_DEF_TEMPLATE_INT m_OneKeyAlarm;
    TYPE_DEF_TEMPLATE_INT m_SdPlayerV2;
    TYPE_DEF_TEMPLATE_INT m_PowerLineFrequency;
    TYPE_DEF_TEMPLATE_INT m_SDSeek;
    TYPE_DEF_TEMPLATE_INT m_RTMPVersions;
    TYPE_DEF_TEMPLATE_INT m_SDPlayFirst;
    TYPE_DEF_TEMPLATE_INT m_SupportWLightBrightness;
    TYPE_DEF_TEMPLATE_INT m_SupportSpeedPlayBack;
    TYPE_DEF_TEMPLATE_INT m_SupportDefaultCfg;
    TYPE_DEF_TEMPLATE_INT m_SupportCloseNightMode;
    TYPE_DEF_TEMPLATE_INT m_SupportRegion;
    TYPE_DEF_TEMPLATE_INT m_SupportOverline;
    TYPE_DEF_TEMPLATE_INT m_SupportEventAudioLight;
    TYPE_DEF_TEMPLATE_INT m_SupportChannelsLinkage;
    TYPE_DEF_TEMPLATE_INT m_SupportMoveToCoord;
    TYPE_DEF_TEMPLATE_INT m_SDPlayBackVersion;
    TYPE_DEF_TEMPLATE_INT m_HomeGuardVersion;
    TYPE_DEF_TEMPLATE_INT m_SupportOneKeyCall;
    TYPE_DEF_TEMPLATE_INT m_SupportManualTimeCalibration;
    TYPE_DEF_TEMPLATE_INT m_SupportMainStreamBitrate;
    TYPE_DEF_TEMPLATE_INT m_SDSeekPlayUseMS;
    TYPE_DEF_TEMPLATE_INT m_Support4G;
    TYPE_DEF_TEMPLATE_INT m_SupportSwitchSimCards;
    TYPE_DEF_TEMPLATE_INT m_SupportImageMode;
} ivm_FunctionSet;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_SignalStrength;
    TYPE_DEF_TEMPLATE_STRING m_SIM1ICCID[32+1];
    TYPE_DEF_TEMPLATE_STRING m_SIM2ICCID[32+1];
    TYPE_DEF_TEMPLATE_FLOAT m_TrafficUsage;
    TYPE_DEF_TEMPLATE_INT m_UseSIMID;
    TYPE_DEF_TEMPLATE_INT m_RSRP;
    TYPE_DEF_TEMPLATE_INT m_SINR;
    TYPE_DEF_TEMPLATE_STRING m_SIM1Operator[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_SIM2Operator[2048+1];
} ivm_Information4G;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_Brightness;
    TYPE_DEF_TEMPLATE_INT m_Contrast;
    TYPE_DEF_TEMPLATE_INT m_Saturation;
    TYPE_DEF_TEMPLATE_INT m_Sharpeness;
    TYPE_DEF_TEMPLATE_INT m_Rotate;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_VideoColor;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_MainBitrate;
    TYPE_DEF_TEMPLATE_INT m_SubBitrate;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_VideoBitrate;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_STRING m_Name[128+1];
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_ChannelOSD;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_TDLogoOSD;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_MainCompration;
    TYPE_DEF_TEMPLATE_ENUM m_SubCompration;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_AudioEncoding;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_Param;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_CruiseFrequencyLevel;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_StartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_EndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_RepeatDate[2048+1];
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_CruiseTimePlan;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_ENUM m_WaitTime;
    TYPE_DEF_TEMPLATE_INT m_CoordX;
    TYPE_DEF_TEMPLATE_INT m_CoordY;
    TYPE_DEF_TEMPLATE_INT m_PosId;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_PtzStation;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_Param;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_PtzSpeed;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_OneClickMasking;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_MainResolution[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_SubResolution[2048+1];
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_VideoResolution;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_MainBitRateControl[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_SubBitRateControl[2048+1];
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_VideoBitRateControl;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_MainFPS;
    TYPE_DEF_TEMPLATE_INT m_SubFPS;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_VideoFPS;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_Param;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_TimedCruiseStatus;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_TimeZoneID;
    TYPE_DEF_TEMPLATE_STRING m_TimeZone[2048+1];
} ivm_TimeZoneSetting;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_WifiName[32+1];
    TYPE_DEF_TEMPLATE_STRING m_WifiPasswd[32+1];
    TYPE_DEF_TEMPLATE_STRING m_Encryption[128+1];
} ivm_WifiConfiguration;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_WifiName[128+1];
    TYPE_DEF_TEMPLATE_STRING m_WifiStrength[128+1];
    TYPE_DEF_TEMPLATE_STRING m_Encryption[128+1];
} ivm_GetWiFiInfo;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_AntiFlickerSwitch;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_BackLightShooting;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_RTMPEnable;
    TYPE_DEF_TEMPLATE_STRING m_RTMPURL[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_RTMPKey[2048+1];
} ivm_RTMPInfo;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_RunTime;
    TYPE_DEF_TEMPLATE_FLOAT m_StorageRemainCapacity;
    TYPE_DEF_TEMPLATE_INT m_OnlineDuration;
} ivm_DynamicInformation;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_RebootDay[2048+1];
    TYPE_DEF_TEMPLATE_BOOL m_RebootEnable;
    TYPE_DEF_TEMPLATE_STRING m_RebootTime[2048+1];
} ivm_TimingReboot;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_EnFillLightMode;
    TYPE_DEF_TEMPLATE_BOOL m_BMenuLight;
    TYPE_DEF_TEMPLATE_INT m_S32LightVal;
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
} ivm_NightVisionMode;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_INT m_Param;
} ivm_WhiteLightBrightness;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_channelID;
    TYPE_DEF_TEMPLATE_STRING m_picUrl[1024+1];
    TYPE_DEF_TEMPLATE_STRING m_iotId[1024+1];
} ivm_SnapshotStatus;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_STRING m_TimeSection[2048+1];
} ivm_TimingShield;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_BOOL m_EnableSub;
} ivm_Onvif;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_HumanDetectSwitch;
    TYPE_DEF_TEMPLATE_BOOL m_HumanTraceEnable;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_ElectronicRegion[2048+1];
} ivm_HumanDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmVoiceType;
    TYPE_DEF_TEMPLATE_STRING m_AlarmVoiceURL[2048+1];
    TYPE_DEF_TEMPLATE_INT m_VoiceID;
    TYPE_DEF_TEMPLATE_INT m_AlarmVoiceTimes;
    TYPE_DEF_TEMPLATE_BOOL m_WhiteLightAlarmEnable;
    TYPE_DEF_TEMPLATE_INT m_WhiteLightTimes;
} ivm_SoundLightAlarmConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_CarDetectSwitch;
    TYPE_DEF_TEMPLATE_BOOL m_StaticFilterEnable;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_ElectronicRegion[2048+1];
} ivm_CarDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_BikeDetectSwitch;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_ElectronicRegion[2048+1];
} ivm_BikeDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_PetDetectSwitch;
    TYPE_DEF_TEMPLATE_BOOL m_PetTraceEnable;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_ElectronicRegion[2048+1];
} ivm_PetDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_ImageDetectSwitch;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
} ivm_ImageDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_HideDetectSwitch;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
} ivm_HideDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_CryDetectSwitch;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
} ivm_CryDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_STRING m_list[2048+1];
    TYPE_DEF_TEMPLATE_INT m_CurrentBitRate;
} ivm_MainStreamBitrate;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_ENUM m_MainCompration;
    TYPE_DEF_TEMPLATE_ENUM m_SubCompration;
} ivm_VideoEncoding;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
    TYPE_DEF_TEMPLATE_INT m_ChunkSizeMB;
} ivm_DeviceLogFileUpload;

typedef struct {
    TYPE_DEF_TEMPLATE_BOOL m_Enable;
} ivm_DeviceLogRedirect;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_RegionDetectSwitch;
    TYPE_DEF_TEMPLATE_BOOL m_LinkTraceSwitch;
    TYPE_DEF_TEMPLATE_ENUM m_LinkTraceMode;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_RegionInfo[2048+1];
    TYPE_DEF_TEMPLATE_BOOL m_HumanTraceEnable;
} ivm_RegionDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_INT m_ChannelID;
    TYPE_DEF_TEMPLATE_BOOL m_OverLineDetectSwitch;
    TYPE_DEF_TEMPLATE_BOOL m_LinkTraceSwitch;
    TYPE_DEF_TEMPLATE_ENUM m_LinkTraceMode;
    TYPE_DEF_TEMPLATE_ENUM m_Sensitivity;
    TYPE_DEF_TEMPLATE_ENUM m_AlarmPeriodMode;
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodStartTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_AlarmPeriodEndTime[2048+1];
    TYPE_DEF_TEMPLATE_STRING m_OverlineInfo[2048+1];
    TYPE_DEF_TEMPLATE_BOOL m_HumanTraceEnable;
} ivm_OverLineDetectConfig;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_EventType;
} ivm_OneKeyCall;

typedef struct {
    TYPE_DEF_TEMPLATE_ENUM m_EventType;
} ivm_NoParamsNormalEvent;

typedef struct {
    struct {
        TYPE_DEF_TEMPLATE_BOOL m_TalkBackStartEnable;
        TYPE_DEF_TEMPLATE_ENUM m_OneKeyCallState;
        ivm_DeviceInformation m_DeviceInformation;
        ivm_ResolutionFunctionSet m_ResolutionFunctionSet;
        TYPE_DEF_TEMPLATE_FLOAT m_StorageTotalCapacity;
        ivm_FunctionSet m_FunctionSet;
        TYPE_DEF_TEMPLATE_ARRAY m_SubFunctionSet[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_INT m_RebootStatus;
        TYPE_DEF_TEMPLATE_ARRAY m_PTZCruise[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_INT m_RemainConnNum;
        TYPE_DEF_TEMPLATE_ARRAY m_PTZStatus[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ENUM m_BindStatusReport;
        TYPE_DEF_TEMPLATE_ARRAY m_PtzStep[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_Information4G m_Information4G;
        TYPE_DEF_TEMPLATE_INT m_CutCardState;
    } ProReadonly;
    struct {
        TYPE_DEF_TEMPLATE_BOOL m_record_enable;
        TYPE_DEF_TEMPLATE_STRING m_key_user_1[100+1];
        TYPE_DEF_TEMPLATE_STRING m_key_user_2[100+1];
        TYPE_DEF_TEMPLATE_ARRAY m_VideoColor[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_STRING m__set_attribute[2048+1];
        TYPE_DEF_TEMPLATE_ARRAY m_VideoBitrate[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_INT m_DeviceInputVolume;
        TYPE_DEF_TEMPLATE_INT m_DeviceOutputVolume;
        TYPE_DEF_TEMPLATE_ARRAY m_ChannelOSD[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_TDLogoOSD[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ENUM m_PowerLineFrequency;
        TYPE_DEF_TEMPLATE_ENUM m_VoiceIntercomMode;
        TYPE_DEF_TEMPLATE_BOOL m_OneKeyCallSwitch;
        TYPE_DEF_TEMPLATE_ARRAY m_AudioEncoding[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_CruiseFrequencyLevel[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_CruiseTimePlan[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_PtzStation[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_PtzSpeed[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_OneClickMasking[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_VideoResolution[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_VideoBitRateControl[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_VideoFPS[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_BOOL m_DevBindStatus;
        TYPE_DEF_TEMPLATE_ARRAY m_TimedCruiseStatus[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_TimeZoneSetting m_TimeZoneSetting;
        TYPE_DEF_TEMPLATE_BOOL m_StatusLightSwitch;
        TYPE_DEF_TEMPLATE_BOOL m_CloudStorageSwitch;
        TYPE_DEF_TEMPLATE_ENUM m_ImageFlipState;
        TYPE_DEF_TEMPLATE_ENUM m_StorageRecordMode;
        TYPE_DEF_TEMPLATE_INT m_FormatStorageMediumProgress;
        TYPE_DEF_TEMPLATE_ENUM m_StorageStatus;
        ivm_WifiConfiguration m_WifiConfiguration;
        TYPE_DEF_TEMPLATE_ARRAY m_GetWiFiInfo[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_AntiFlickerSwitch[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_BackLightShooting[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_RTMPInfo m_RTMPInfo;
        TYPE_DEF_TEMPLATE_STRING m_DeciceResolution[32+1];
        ivm_DynamicInformation m_DynamicInformation;
        TYPE_DEF_TEMPLATE_ENUM m_DayNightMode;
        ivm_TimingReboot m_TimingReboot;
        TYPE_DEF_TEMPLATE_ARRAY m_NightVisionMode[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_WhiteLightBrightness[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_SnapshotStatus m_SnapshotStatus;
        TYPE_DEF_TEMPLATE_ARRAY m_TimingShield[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_Onvif m_Onvif;
        TYPE_DEF_TEMPLATE_BOOL m_HomeGuardSwitch;
        TYPE_DEF_TEMPLATE_ARRAY m_HumanDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_SoundLightAlarmConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_CarDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_BikeDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_PetDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_ImageDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_HideDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_CryDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_MainStreamBitrate m_MainStreamBitrate;
        TYPE_DEF_TEMPLATE_ARRAY m_VideoEncoding[MAX_ARRAY_JSON_STR_LEN +1];
        ivm_DeviceLogFileUpload m_DeviceLogFileUpload;
        ivm_DeviceLogRedirect m_DeviceLogRedirect;
        TYPE_DEF_TEMPLATE_ENUM m_ImageMode;
        TYPE_DEF_TEMPLATE_ARRAY m_RegionDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
        TYPE_DEF_TEMPLATE_ARRAY m_OverLineDetectConfig[MAX_ARRAY_JSON_STR_LEN +1];
    } ProWritable;
    struct {
        ivm_OneKeyCall m_OneKeyCall;
        ivm_NoParamsNormalEvent m_NoParamsNormalEvent;
    } Event;
} ivm_extend_param_t;

extern   ivm_extend_param_t     g_ivm_objs;

/*!    \brief	物模型实例全局变量,应用层可直接访问/修改这个对象的成员变量。
        注意:
        1. 设备开发者可直接访问g_ivm_objs全局变量访问/修改物模型
        如：对一个int32类型的物模型对象yyyy的状态置值，应用层可直接操作：
        ```
        ivm_lock();		//互斥加锁
        g_ivm_objs.ProReadonly.yyyy = 1;		       //置状态值
        iv_dm_property_report(yyy, cb, param); //置状态
        ivm_unlock();	//互斥解锁
        ```
        2. 访问互斥
        * 在iv_usrcb_xxxxx系列回调函数中访问时g_ivm_objs时，无需加锁
        * 在其它任何地方访问g_ivm_objs，都应该调用ivm_lock()/ivm_unlock()作互斥处理
        3. 不得在iv_usrcb_xxxxx系列回调函数进行导致阻塞或耗时的操作，这样会阻塞核心通讯线程.
        4. 读写表示该属性即可从设备端上报到云端也可从云端发起控制，只读表示该属性只从设备向云端上报。
        */
/*! 
 \brief  更新物模型 --整形
 */ 
#define ivm_ProReadonly_setInt(obj, _val) \
   do {                                   \
       ivm_lock();                        \
       g_ivm_objs.ProReadonly.m_##obj = _val; \
       iv_dm_property_report(#obj, NULL, NULL);\
       ivm_unlock();                      \
   } while (0)

#define ivm_ProWritable_setInt(obj, _val) \
   do {                                   \
       ivm_lock();                        \
       g_ivm_objs.ProWritable.m_##obj = _val; \
       iv_dm_property_report(#obj, NULL, NULL);\
       ivm_unlock();                      \
   } while (0)
/*! 
 \brief  更新物模型 --字符串
 */ 
#define ivm_ProReadonly_setString(obj, _str)  \
   do {                                   \
       ivm_lock();                        \
       strncpy(g_ivm_objs.ProReadonly.m_##obj, _str, sizeof(g_ivm_objs.ProReadonly.m_##obj) - 1); \
       iv_dm_property_report(#obj, NULL, NULL);       \
       ivm_unlock();                      \
   } while (0)

#define ivm_ProWritable_setString(obj, _str)  \
   do {                                   \
       ivm_lock();                        \
       strncpy(g_ivm_objs.ProWritable.m_##obj, _str, sizeof(g_ivm_objs.ProWritable.m_##obj) - 1); \
       iv_dm_property_report(#obj, NULL, NULL);       \
       ivm_unlock();                      \
   } while (0)
/*! 
 \brief  更新结构体--字符串成员
 */ 
#define ivm_ProWritable_setStructString(obj, member, _str)  \
   do {                                   \
       strncpy(g_ivm_objs.ProWritable.m_##obj.m_##member, _str, sizeof(g_ivm_objs.ProWritable.m_##obj.m_##member) - 1); \
   } while (0)

int iv_usrcb_ProWritable_record_enable(const TYPE_DEF_TEMPLATE_BOOL *record_enable);
int iv_usrcb_ProWritable_key_user_1(const TYPE_DEF_TEMPLATE_STRING *key_user_1);
int iv_usrcb_ProWritable_key_user_2(const TYPE_DEF_TEMPLATE_STRING *key_user_2);
int iv_usrcb_ProWritable_VideoColor(const TYPE_DEF_TEMPLATE_ARRAY *VideoColor);
int iv_usrcb_ProWritable__set_attribute(const TYPE_DEF_TEMPLATE_STRING *_set_attribute);
int iv_usrcb_ProWritable_VideoBitrate(const TYPE_DEF_TEMPLATE_ARRAY *VideoBitrate);
int iv_usrcb_ProWritable_DeviceInputVolume(const TYPE_DEF_TEMPLATE_INT *DeviceInputVolume);
int iv_usrcb_ProWritable_DeviceOutputVolume(const TYPE_DEF_TEMPLATE_INT *DeviceOutputVolume);
int iv_usrcb_ProWritable_ChannelOSD(const TYPE_DEF_TEMPLATE_ARRAY *ChannelOSD);
int iv_usrcb_ProWritable_TDLogoOSD(const TYPE_DEF_TEMPLATE_ARRAY *TDLogoOSD);
int iv_usrcb_ProWritable_PowerLineFrequency(const TYPE_DEF_TEMPLATE_ENUM *PowerLineFrequency);
int iv_usrcb_ProWritable_VoiceIntercomMode(const TYPE_DEF_TEMPLATE_ENUM *VoiceIntercomMode);
int iv_usrcb_ProWritable_OneKeyCallSwitch(const TYPE_DEF_TEMPLATE_BOOL *OneKeyCallSwitch);
int iv_usrcb_ProWritable_AudioEncoding(const TYPE_DEF_TEMPLATE_ARRAY *AudioEncoding);
int iv_usrcb_ProWritable_CruiseFrequencyLevel(const TYPE_DEF_TEMPLATE_ARRAY *CruiseFrequencyLevel);
int iv_usrcb_ProWritable_CruiseTimePlan(const TYPE_DEF_TEMPLATE_ARRAY *CruiseTimePlan);
int iv_usrcb_ProWritable_PtzStation(const TYPE_DEF_TEMPLATE_ARRAY *PtzStation);
int iv_usrcb_ProWritable_PtzSpeed(const TYPE_DEF_TEMPLATE_ARRAY *PtzSpeed);
int iv_usrcb_ProWritable_OneClickMasking(const TYPE_DEF_TEMPLATE_ARRAY *OneClickMasking);
int iv_usrcb_ProWritable_VideoResolution(const TYPE_DEF_TEMPLATE_ARRAY *VideoResolution);
int iv_usrcb_ProWritable_VideoBitRateControl(const TYPE_DEF_TEMPLATE_ARRAY *VideoBitRateControl);
int iv_usrcb_ProWritable_VideoFPS(const TYPE_DEF_TEMPLATE_ARRAY *VideoFPS);
int iv_usrcb_ProWritable_DevBindStatus(const TYPE_DEF_TEMPLATE_BOOL *DevBindStatus);
int iv_usrcb_ProWritable_TimedCruiseStatus(const TYPE_DEF_TEMPLATE_ARRAY *TimedCruiseStatus);
int iv_usrcb_ProWritable_TimeZoneSetting(const ivm_TimeZoneSetting *TimeZoneSetting);
int iv_usrcb_ProWritable_StatusLightSwitch(const TYPE_DEF_TEMPLATE_BOOL *StatusLightSwitch);
int iv_usrcb_ProWritable_CloudStorageSwitch(const TYPE_DEF_TEMPLATE_BOOL *CloudStorageSwitch);
int iv_usrcb_ProWritable_ImageFlipState(const TYPE_DEF_TEMPLATE_ENUM *ImageFlipState);
int iv_usrcb_ProWritable_StorageRecordMode(const TYPE_DEF_TEMPLATE_ENUM *StorageRecordMode);
int iv_usrcb_ProWritable_FormatStorageMediumProgress(const TYPE_DEF_TEMPLATE_INT *FormatStorageMediumProgress);
int iv_usrcb_ProWritable_StorageStatus(const TYPE_DEF_TEMPLATE_ENUM *StorageStatus);
int iv_usrcb_ProWritable_WifiConfiguration(const ivm_WifiConfiguration *WifiConfiguration);
int iv_usrcb_ProWritable_GetWiFiInfo(const TYPE_DEF_TEMPLATE_ARRAY *GetWiFiInfo);
int iv_usrcb_ProWritable_AntiFlickerSwitch(const TYPE_DEF_TEMPLATE_ARRAY *AntiFlickerSwitch);
int iv_usrcb_ProWritable_BackLightShooting(const TYPE_DEF_TEMPLATE_ARRAY *BackLightShooting);
int iv_usrcb_ProWritable_RTMPInfo(const ivm_RTMPInfo *RTMPInfo);
int iv_usrcb_ProWritable_DeciceResolution(const TYPE_DEF_TEMPLATE_STRING *DeciceResolution);
int iv_usrcb_ProWritable_DynamicInformation(const ivm_DynamicInformation *DynamicInformation);
int iv_usrcb_ProWritable_DayNightMode(const TYPE_DEF_TEMPLATE_ENUM *DayNightMode);
int iv_usrcb_ProWritable_TimingReboot(const ivm_TimingReboot *TimingReboot);
int iv_usrcb_ProWritable_NightVisionMode(const TYPE_DEF_TEMPLATE_ARRAY *NightVisionMode);
int iv_usrcb_ProWritable_WhiteLightBrightness(const TYPE_DEF_TEMPLATE_ARRAY *WhiteLightBrightness);
int iv_usrcb_ProWritable_SnapshotStatus(const ivm_SnapshotStatus *SnapshotStatus);
int iv_usrcb_ProWritable_TimingShield(const TYPE_DEF_TEMPLATE_ARRAY *TimingShield);
int iv_usrcb_ProWritable_Onvif(const ivm_Onvif *Onvif);
int iv_usrcb_ProWritable_HomeGuardSwitch(const TYPE_DEF_TEMPLATE_BOOL *HomeGuardSwitch);
int iv_usrcb_ProWritable_HumanDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *HumanDetectConfig);
int iv_usrcb_ProWritable_SoundLightAlarmConfig(const TYPE_DEF_TEMPLATE_ARRAY *SoundLightAlarmConfig);
int iv_usrcb_ProWritable_CarDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *CarDetectConfig);
int iv_usrcb_ProWritable_BikeDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *BikeDetectConfig);
int iv_usrcb_ProWritable_PetDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *PetDetectConfig);
int iv_usrcb_ProWritable_ImageDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *ImageDetectConfig);
int iv_usrcb_ProWritable_HideDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *HideDetectConfig);
int iv_usrcb_ProWritable_CryDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *CryDetectConfig);
int iv_usrcb_ProWritable_MainStreamBitrate(const ivm_MainStreamBitrate *MainStreamBitrate);
int iv_usrcb_ProWritable_VideoEncoding(const TYPE_DEF_TEMPLATE_ARRAY *VideoEncoding);
int iv_usrcb_ProWritable_DeviceLogFileUpload(const ivm_DeviceLogFileUpload *DeviceLogFileUpload);
int iv_usrcb_ProWritable_DeviceLogRedirect(const ivm_DeviceLogRedirect *DeviceLogRedirect);
int iv_usrcb_ProWritable_ImageMode(const TYPE_DEF_TEMPLATE_ENUM *ImageMode);
int iv_usrcb_ProWritable_RegionDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *RegionDetectConfig);
int iv_usrcb_ProWritable_OverLineDetectConfig(const TYPE_DEF_TEMPLATE_ARRAY *OverLineDetectConfig);

int iv_usrcb_Action_TextQuery(ivm_TextQuery_t *TextQuery);
int iv_usrcb_Action_SetDefaultVideoColor(ivm_SetDefaultVideoColor_t *SetDefaultVideoColor);
int iv_usrcb_Action_OneKeyAlarm(ivm_OneKeyAlarm_t *OneKeyAlarm);
int iv_usrcb_Action_ReplyCall(ivm_ReplyCall_t *ReplyCall);
int iv_usrcb_Action_FormatStorageMedium(void *arg);
int iv_usrcb_Action_Reboot(void *arg);
int iv_usrcb_Action_StartPTZCruise(ivm_StartPTZCruise_t *StartPTZCruise);
int iv_usrcb_Action_AddPTZCruise(ivm_AddPTZCruise_t *AddPTZCruise);
int iv_usrcb_Action_DelPTZCruise(ivm_DelPTZCruise_t *DelPTZCruise);
int iv_usrcb_Action_PreviewImageCapture(ivm_PreviewImageCapture_t *PreviewImageCapture);
int iv_usrcb_Action_ResetPTZ(ivm_ResetPTZ_t *ResetPTZ);
int iv_usrcb_Action_GotoPTZStation(ivm_GotoPTZStation_t *GotoPTZStation);
int iv_usrcb_Action_SetDefaultDevConfigs(void *arg);
int iv_usrcb_Action_ManualTimeCalibration(ivm_ManualTimeCalibration_t *ManualTimeCalibration);
int iv_usrcb_Action_CloudStorageStaus(ivm_CloudStorageStaus_t *CloudStorageStaus);
int iv_usrcb_Action_DeviceFileUpload(ivm_DeviceFileUpload_t *DeviceFileUpload);
int iv_usrcb_Action_DeviceFileUploadStatus(ivm_DeviceFileUploadStatus_t *DeviceFileUploadStatus);
int iv_usrcb_Action_BindOrDelete(ivm_BindOrDelete_t *BindOrDelete);
int iv_usrcb_Action_TwecallActivate(ivm_TwecallActivate_t *TwecallActivate);
int iv_usrcb_Action_OnSwitchingSimCards(void *arg);

#ifdef __cplusplus
}
#endif


#endif
