#include "iv_usrex.h"
#include "iv_def.h"
/*-----------------data config start  -------------------*/ 

#define TIMEZONESETTING_MEMBER_COUNT 2
static sDataPoint  sg_TimeZoneSetting_DataTemplate[TIMEZONESETTING_MEMBER_COUNT];

#define WIFICONFIGURATION_MEMBER_COUNT 3
static sDataPoint  sg_WifiConfiguration_DataTemplate[WIFICONFIGURATION_MEMBER_COUNT];

#define RTMPINFO_MEMBER_COUNT 3
static sDataPoint  sg_RTMPInfo_DataTemplate[RTMPINFO_MEMBER_COUNT];

#define DYNAMICINFORMATION_MEMBER_COUNT 3
static sDataPoint  sg_DynamicInformation_DataTemplate[DYNAMICINFORMATION_MEMBER_COUNT];

#define TIMINGREBOOT_MEMBER_COUNT 3
static sDataPoint  sg_TimingReboot_DataTemplate[TIMINGREBOOT_MEMBER_COUNT];

#define SNAPSHOTSTATUS_MEMBER_COUNT 3
static sDataPoint  sg_SnapshotStatus_DataTemplate[SNAPSHOTSTATUS_MEMBER_COUNT];

#define ONVIF_MEMBER_COUNT 2
static sDataPoint  sg_Onvif_DataTemplate[ONVIF_MEMBER_COUNT];

#define MAINSTREAMBITRATE_MEMBER_COUNT 2
static sDataPoint  sg_MainStreamBitrate_DataTemplate[MAINSTREAMBITRATE_MEMBER_COUNT];

#define DEVICELOGFILEUPLOAD_MEMBER_COUNT 2
static sDataPoint  sg_DeviceLogFileUpload_DataTemplate[DEVICELOGFILEUPLOAD_MEMBER_COUNT];

#define DEVICELOGREDIRECT_MEMBER_COUNT 1
static sDataPoint  sg_DeviceLogRedirect_DataTemplate[DEVICELOGREDIRECT_MEMBER_COUNT];

#define DEVICEINFORMATION_MEMBER_COUNT 9
static sDataPoint  sg_DeviceInformation_DataTemplate[DEVICEINFORMATION_MEMBER_COUNT];

#define RESOLUTIONFUNCTIONSET_MEMBER_COUNT 5
static sDataPoint  sg_ResolutionFunctionSet_DataTemplate[RESOLUTIONFUNCTIONSET_MEMBER_COUNT];

#define FUNCTIONSET_MEMBER_COUNT 29
static sDataPoint  sg_FunctionSet_DataTemplate[FUNCTIONSET_MEMBER_COUNT];

#define INFORMATION4G_MEMBER_COUNT 9
static sDataPoint  sg_Information4G_DataTemplate[INFORMATION4G_MEMBER_COUNT];

#define TOTAL_WR_PROPERTY_COUNT 61

static sDataPoint    sg_WR_DataTemplate[TOTAL_WR_PROPERTY_COUNT];

#define TOTAL_RO_PROPERTY_COUNT 15

static sDataPoint    sg_RO_DataTemplate[TOTAL_RO_PROPERTY_COUNT];

ivm_extend_param_t    g_ivm_objs;

static void _ivm_init_data_template(void)
{
    sg_RO_DataTemplate[0].data_property.data = &g_ivm_objs.ProReadonly.m_TalkBackStartEnable;
    sg_RO_DataTemplate[0].data_property.key  = "TalkBackStartEnable";
    sg_RO_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_RO_DataTemplate[0].state = eNOCHANGE;

    sg_RO_DataTemplate[1].data_property.data = &g_ivm_objs.ProReadonly.m_OneKeyCallState;
    sg_RO_DataTemplate[1].data_property.key  = "OneKeyCallState";
    sg_RO_DataTemplate[1].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_RO_DataTemplate[1].state = eNOCHANGE;

    sg_RO_DataTemplate[2].data_property.data = sg_DeviceInformation_DataTemplate;
    sg_RO_DataTemplate[2].data_property.struct_obj_num = DEVICEINFORMATION_MEMBER_COUNT;
    sg_RO_DataTemplate[2].data_property.key  = "DeviceInformation";
    sg_RO_DataTemplate[2].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_RO_DataTemplate[2].state = eNOCHANGE;

    sg_RO_DataTemplate[3].data_property.data = sg_ResolutionFunctionSet_DataTemplate;
    sg_RO_DataTemplate[3].data_property.struct_obj_num = RESOLUTIONFUNCTIONSET_MEMBER_COUNT;
    sg_RO_DataTemplate[3].data_property.key  = "ResolutionFunctionSet";
    sg_RO_DataTemplate[3].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_RO_DataTemplate[3].state = eNOCHANGE;

    sg_RO_DataTemplate[4].data_property.data = &g_ivm_objs.ProReadonly.m_StorageTotalCapacity;
    sg_RO_DataTemplate[4].data_property.key  = "StorageTotalCapacity";
    sg_RO_DataTemplate[4].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_RO_DataTemplate[4].state = eNOCHANGE;

    sg_RO_DataTemplate[5].data_property.data = sg_FunctionSet_DataTemplate;
    sg_RO_DataTemplate[5].data_property.struct_obj_num = FUNCTIONSET_MEMBER_COUNT;
    sg_RO_DataTemplate[5].data_property.key  = "FunctionSet";
    sg_RO_DataTemplate[5].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_RO_DataTemplate[5].state = eNOCHANGE;

    sg_RO_DataTemplate[6].data_property.data = &g_ivm_objs.ProReadonly.m_SubFunctionSet;
    sg_RO_DataTemplate[6].data_property.key  = "SubFunctionSet";
    sg_RO_DataTemplate[6].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_RO_DataTemplate[6].state = eNOCHANGE;

    sg_RO_DataTemplate[7].data_property.data = &g_ivm_objs.ProReadonly.m_RebootStatus;
    sg_RO_DataTemplate[7].data_property.key  = "RebootStatus";
    sg_RO_DataTemplate[7].data_property.type = TYPE_TEMPLATE_INT;
    sg_RO_DataTemplate[7].state = eNOCHANGE;

    sg_RO_DataTemplate[8].data_property.data = &g_ivm_objs.ProReadonly.m_PTZCruise;
    sg_RO_DataTemplate[8].data_property.key  = "PTZCruise";
    sg_RO_DataTemplate[8].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_RO_DataTemplate[8].state = eNOCHANGE;

    sg_RO_DataTemplate[9].data_property.data = &g_ivm_objs.ProReadonly.m_RemainConnNum;
    sg_RO_DataTemplate[9].data_property.key  = "RemainConnNum";
    sg_RO_DataTemplate[9].data_property.type = TYPE_TEMPLATE_INT;
    sg_RO_DataTemplate[9].state = eNOCHANGE;

    sg_RO_DataTemplate[10].data_property.data = &g_ivm_objs.ProReadonly.m_PTZStatus;
    sg_RO_DataTemplate[10].data_property.key  = "PTZStatus";
    sg_RO_DataTemplate[10].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_RO_DataTemplate[10].state = eNOCHANGE;

    sg_RO_DataTemplate[11].data_property.data = &g_ivm_objs.ProReadonly.m_BindStatusReport;
    sg_RO_DataTemplate[11].data_property.key  = "BindStatusReport";
    sg_RO_DataTemplate[11].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_RO_DataTemplate[11].state = eNOCHANGE;

    sg_RO_DataTemplate[12].data_property.data = &g_ivm_objs.ProReadonly.m_PtzStep;
    sg_RO_DataTemplate[12].data_property.key  = "PtzStep";
    sg_RO_DataTemplate[12].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_RO_DataTemplate[12].state = eNOCHANGE;

    sg_RO_DataTemplate[13].data_property.data = sg_Information4G_DataTemplate;
    sg_RO_DataTemplate[13].data_property.struct_obj_num = INFORMATION4G_MEMBER_COUNT;
    sg_RO_DataTemplate[13].data_property.key  = "Information4G";
    sg_RO_DataTemplate[13].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_RO_DataTemplate[13].state = eNOCHANGE;

    sg_RO_DataTemplate[14].data_property.data = &g_ivm_objs.ProReadonly.m_CutCardState;
    sg_RO_DataTemplate[14].data_property.key  = "CutCardState";
    sg_RO_DataTemplate[14].data_property.type = TYPE_TEMPLATE_INT;
    sg_RO_DataTemplate[14].state = eNOCHANGE;

    sg_WR_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_record_enable;
    sg_WR_DataTemplate[0].data_property.key  = "record_enable";
    sg_WR_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_WR_DataTemplate[0].state = eNOCHANGE;

    sg_WR_DataTemplate[1].data_property.data = g_ivm_objs.ProWritable.m_key_user_1;
    sg_WR_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_key_user_1)/sizeof(g_ivm_objs.ProWritable.m_key_user_1[1]);
    sg_WR_DataTemplate[1].data_property.key  = "key_user_1";
    sg_WR_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WR_DataTemplate[1].state = eNOCHANGE;

    sg_WR_DataTemplate[2].data_property.data = g_ivm_objs.ProWritable.m_key_user_2;
    sg_WR_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_key_user_2)/sizeof(g_ivm_objs.ProWritable.m_key_user_2[2]);
    sg_WR_DataTemplate[2].data_property.key  = "key_user_2";
    sg_WR_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WR_DataTemplate[2].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_VideoColor, 0, sizeof(g_ivm_objs.ProWritable.m_VideoColor));
    sg_WR_DataTemplate[3].data_property.data = &g_ivm_objs.ProWritable.m_VideoColor;
    sg_WR_DataTemplate[3].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[3].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[3].data_property.key  = "VideoColor";
    sg_WR_DataTemplate[3].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[3].state = eNOCHANGE;

    sg_WR_DataTemplate[4].data_property.data = g_ivm_objs.ProWritable.m__set_attribute;
    sg_WR_DataTemplate[4].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m__set_attribute)/sizeof(g_ivm_objs.ProWritable.m__set_attribute[4]);
    sg_WR_DataTemplate[4].data_property.key  = "_set_attribute";
    sg_WR_DataTemplate[4].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WR_DataTemplate[4].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_VideoBitrate, 0, sizeof(g_ivm_objs.ProWritable.m_VideoBitrate));
    sg_WR_DataTemplate[5].data_property.data = &g_ivm_objs.ProWritable.m_VideoBitrate;
    sg_WR_DataTemplate[5].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[5].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[5].data_property.key  = "VideoBitrate";
    sg_WR_DataTemplate[5].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[5].state = eNOCHANGE;

    sg_WR_DataTemplate[6].data_property.data = &g_ivm_objs.ProWritable.m_DeviceInputVolume;
    sg_WR_DataTemplate[6].data_property.key  = "DeviceInputVolume";
    sg_WR_DataTemplate[6].data_property.type = TYPE_TEMPLATE_INT;
    sg_WR_DataTemplate[6].state = eNOCHANGE;

    sg_WR_DataTemplate[7].data_property.data = &g_ivm_objs.ProWritable.m_DeviceOutputVolume;
    sg_WR_DataTemplate[7].data_property.key  = "DeviceOutputVolume";
    sg_WR_DataTemplate[7].data_property.type = TYPE_TEMPLATE_INT;
    sg_WR_DataTemplate[7].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_ChannelOSD, 0, sizeof(g_ivm_objs.ProWritable.m_ChannelOSD));
    sg_WR_DataTemplate[8].data_property.data = &g_ivm_objs.ProWritable.m_ChannelOSD;
    sg_WR_DataTemplate[8].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[8].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[8].data_property.key  = "ChannelOSD";
    sg_WR_DataTemplate[8].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[8].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_TDLogoOSD, 0, sizeof(g_ivm_objs.ProWritable.m_TDLogoOSD));
    sg_WR_DataTemplate[9].data_property.data = &g_ivm_objs.ProWritable.m_TDLogoOSD;
    sg_WR_DataTemplate[9].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[9].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[9].data_property.key  = "TDLogoOSD";
    sg_WR_DataTemplate[9].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[9].state = eNOCHANGE;

    sg_WR_DataTemplate[10].data_property.data = &g_ivm_objs.ProWritable.m_PowerLineFrequency;
    sg_WR_DataTemplate[10].data_property.key  = "PowerLineFrequency";
    sg_WR_DataTemplate[10].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[10].state = eNOCHANGE;

    sg_WR_DataTemplate[11].data_property.data = &g_ivm_objs.ProWritable.m_VoiceIntercomMode;
    sg_WR_DataTemplate[11].data_property.key  = "VoiceIntercomMode";
    sg_WR_DataTemplate[11].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[11].state = eNOCHANGE;

    sg_WR_DataTemplate[12].data_property.data = &g_ivm_objs.ProWritable.m_OneKeyCallSwitch;
    sg_WR_DataTemplate[12].data_property.key  = "OneKeyCallSwitch";
    sg_WR_DataTemplate[12].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_WR_DataTemplate[12].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_AudioEncoding, 0, sizeof(g_ivm_objs.ProWritable.m_AudioEncoding));
    sg_WR_DataTemplate[13].data_property.data = &g_ivm_objs.ProWritable.m_AudioEncoding;
    sg_WR_DataTemplate[13].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[13].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[13].data_property.key  = "AudioEncoding";
    sg_WR_DataTemplate[13].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[13].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_CruiseFrequencyLevel, 0, sizeof(g_ivm_objs.ProWritable.m_CruiseFrequencyLevel));
    sg_WR_DataTemplate[14].data_property.data = &g_ivm_objs.ProWritable.m_CruiseFrequencyLevel;
    sg_WR_DataTemplate[14].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[14].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[14].data_property.key  = "CruiseFrequencyLevel";
    sg_WR_DataTemplate[14].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[14].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_CruiseTimePlan, 0, sizeof(g_ivm_objs.ProWritable.m_CruiseTimePlan));
    sg_WR_DataTemplate[15].data_property.data = &g_ivm_objs.ProWritable.m_CruiseTimePlan;
    sg_WR_DataTemplate[15].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[15].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[15].data_property.key  = "CruiseTimePlan";
    sg_WR_DataTemplate[15].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[15].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_PtzStation, 0, sizeof(g_ivm_objs.ProWritable.m_PtzStation));
    sg_WR_DataTemplate[16].data_property.data = &g_ivm_objs.ProWritable.m_PtzStation;
    sg_WR_DataTemplate[16].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[16].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[16].data_property.key  = "PtzStation";
    sg_WR_DataTemplate[16].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[16].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_PtzSpeed, 0, sizeof(g_ivm_objs.ProWritable.m_PtzSpeed));
    sg_WR_DataTemplate[17].data_property.data = &g_ivm_objs.ProWritable.m_PtzSpeed;
    sg_WR_DataTemplate[17].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[17].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[17].data_property.key  = "PtzSpeed";
    sg_WR_DataTemplate[17].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[17].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_OneClickMasking, 0, sizeof(g_ivm_objs.ProWritable.m_OneClickMasking));
    sg_WR_DataTemplate[18].data_property.data = &g_ivm_objs.ProWritable.m_OneClickMasking;
    sg_WR_DataTemplate[18].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[18].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[18].data_property.key  = "OneClickMasking";
    sg_WR_DataTemplate[18].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[18].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_VideoResolution, 0, sizeof(g_ivm_objs.ProWritable.m_VideoResolution));
    sg_WR_DataTemplate[19].data_property.data = &g_ivm_objs.ProWritable.m_VideoResolution;
    sg_WR_DataTemplate[19].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[19].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[19].data_property.key  = "VideoResolution";
    sg_WR_DataTemplate[19].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[19].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_VideoBitRateControl, 0, sizeof(g_ivm_objs.ProWritable.m_VideoBitRateControl));
    sg_WR_DataTemplate[20].data_property.data = &g_ivm_objs.ProWritable.m_VideoBitRateControl;
    sg_WR_DataTemplate[20].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[20].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[20].data_property.key  = "VideoBitRateControl";
    sg_WR_DataTemplate[20].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[20].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_VideoFPS, 0, sizeof(g_ivm_objs.ProWritable.m_VideoFPS));
    sg_WR_DataTemplate[21].data_property.data = &g_ivm_objs.ProWritable.m_VideoFPS;
    sg_WR_DataTemplate[21].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[21].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[21].data_property.key  = "VideoFPS";
    sg_WR_DataTemplate[21].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[21].state = eNOCHANGE;

    sg_WR_DataTemplate[22].data_property.data = &g_ivm_objs.ProWritable.m_DevBindStatus;
    sg_WR_DataTemplate[22].data_property.key  = "DevBindStatus";
    sg_WR_DataTemplate[22].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_WR_DataTemplate[22].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_TimedCruiseStatus, 0, sizeof(g_ivm_objs.ProWritable.m_TimedCruiseStatus));
    sg_WR_DataTemplate[23].data_property.data = &g_ivm_objs.ProWritable.m_TimedCruiseStatus;
    sg_WR_DataTemplate[23].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[23].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[23].data_property.key  = "TimedCruiseStatus";
    sg_WR_DataTemplate[23].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[23].state = eNOCHANGE;

    sg_WR_DataTemplate[24].data_property.data = sg_TimeZoneSetting_DataTemplate;
    sg_WR_DataTemplate[24].data_property.data_buff_len = TIMEZONESETTING_MEMBER_COUNT;
    sg_WR_DataTemplate[24].data_property.key  = "TimeZoneSetting";
    sg_WR_DataTemplate[24].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[24].state = eNOCHANGE;

    sg_WR_DataTemplate[25].data_property.data = &g_ivm_objs.ProWritable.m_StatusLightSwitch;
    sg_WR_DataTemplate[25].data_property.key  = "StatusLightSwitch";
    sg_WR_DataTemplate[25].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_WR_DataTemplate[25].state = eNOCHANGE;

    sg_WR_DataTemplate[26].data_property.data = &g_ivm_objs.ProWritable.m_CloudStorageSwitch;
    sg_WR_DataTemplate[26].data_property.key  = "CloudStorageSwitch";
    sg_WR_DataTemplate[26].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_WR_DataTemplate[26].state = eNOCHANGE;

    sg_WR_DataTemplate[27].data_property.data = &g_ivm_objs.ProWritable.m_ImageFlipState;
    sg_WR_DataTemplate[27].data_property.key  = "ImageFlipState";
    sg_WR_DataTemplate[27].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[27].state = eNOCHANGE;

    sg_WR_DataTemplate[28].data_property.data = &g_ivm_objs.ProWritable.m_StorageRecordMode;
    sg_WR_DataTemplate[28].data_property.key  = "StorageRecordMode";
    sg_WR_DataTemplate[28].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[28].state = eNOCHANGE;

    sg_WR_DataTemplate[29].data_property.data = &g_ivm_objs.ProWritable.m_FormatStorageMediumProgress;
    sg_WR_DataTemplate[29].data_property.key  = "FormatStorageMediumProgress";
    sg_WR_DataTemplate[29].data_property.type = TYPE_TEMPLATE_INT;
    sg_WR_DataTemplate[29].state = eNOCHANGE;

    sg_WR_DataTemplate[30].data_property.data = &g_ivm_objs.ProWritable.m_StorageStatus;
    sg_WR_DataTemplate[30].data_property.key  = "StorageStatus";
    sg_WR_DataTemplate[30].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[30].state = eNOCHANGE;

    sg_WR_DataTemplate[31].data_property.data = sg_WifiConfiguration_DataTemplate;
    sg_WR_DataTemplate[31].data_property.data_buff_len = WIFICONFIGURATION_MEMBER_COUNT;
    sg_WR_DataTemplate[31].data_property.key  = "WifiConfiguration";
    sg_WR_DataTemplate[31].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[31].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_GetWiFiInfo, 0, sizeof(g_ivm_objs.ProWritable.m_GetWiFiInfo));
    sg_WR_DataTemplate[32].data_property.data = &g_ivm_objs.ProWritable.m_GetWiFiInfo;
    sg_WR_DataTemplate[32].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[32].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[32].data_property.key  = "GetWiFiInfo";
    sg_WR_DataTemplate[32].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[32].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_AntiFlickerSwitch, 0, sizeof(g_ivm_objs.ProWritable.m_AntiFlickerSwitch));
    sg_WR_DataTemplate[33].data_property.data = &g_ivm_objs.ProWritable.m_AntiFlickerSwitch;
    sg_WR_DataTemplate[33].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[33].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[33].data_property.key  = "AntiFlickerSwitch";
    sg_WR_DataTemplate[33].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[33].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_BackLightShooting, 0, sizeof(g_ivm_objs.ProWritable.m_BackLightShooting));
    sg_WR_DataTemplate[34].data_property.data = &g_ivm_objs.ProWritable.m_BackLightShooting;
    sg_WR_DataTemplate[34].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[34].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[34].data_property.key  = "BackLightShooting";
    sg_WR_DataTemplate[34].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[34].state = eNOCHANGE;

    sg_WR_DataTemplate[35].data_property.data = sg_RTMPInfo_DataTemplate;
    sg_WR_DataTemplate[35].data_property.data_buff_len = RTMPINFO_MEMBER_COUNT;
    sg_WR_DataTemplate[35].data_property.key  = "RTMPInfo";
    sg_WR_DataTemplate[35].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[35].state = eNOCHANGE;

    sg_WR_DataTemplate[36].data_property.data = g_ivm_objs.ProWritable.m_DeciceResolution;
    sg_WR_DataTemplate[36].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_DeciceResolution)/sizeof(g_ivm_objs.ProWritable.m_DeciceResolution[36]);
    sg_WR_DataTemplate[36].data_property.key  = "DeciceResolution";
    sg_WR_DataTemplate[36].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WR_DataTemplate[36].state = eNOCHANGE;

    sg_WR_DataTemplate[37].data_property.data = sg_DynamicInformation_DataTemplate;
    sg_WR_DataTemplate[37].data_property.data_buff_len = DYNAMICINFORMATION_MEMBER_COUNT;
    sg_WR_DataTemplate[37].data_property.key  = "DynamicInformation";
    sg_WR_DataTemplate[37].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[37].state = eNOCHANGE;

    sg_WR_DataTemplate[38].data_property.data = &g_ivm_objs.ProWritable.m_DayNightMode;
    sg_WR_DataTemplate[38].data_property.key  = "DayNightMode";
    sg_WR_DataTemplate[38].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[38].state = eNOCHANGE;

    sg_WR_DataTemplate[39].data_property.data = sg_TimingReboot_DataTemplate;
    sg_WR_DataTemplate[39].data_property.data_buff_len = TIMINGREBOOT_MEMBER_COUNT;
    sg_WR_DataTemplate[39].data_property.key  = "TimingReboot";
    sg_WR_DataTemplate[39].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[39].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_NightVisionMode, 0, sizeof(g_ivm_objs.ProWritable.m_NightVisionMode));
    sg_WR_DataTemplate[40].data_property.data = &g_ivm_objs.ProWritable.m_NightVisionMode;
    sg_WR_DataTemplate[40].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[40].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[40].data_property.key  = "NightVisionMode";
    sg_WR_DataTemplate[40].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[40].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_WhiteLightBrightness, 0, sizeof(g_ivm_objs.ProWritable.m_WhiteLightBrightness));
    sg_WR_DataTemplate[41].data_property.data = &g_ivm_objs.ProWritable.m_WhiteLightBrightness;
    sg_WR_DataTemplate[41].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[41].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[41].data_property.key  = "WhiteLightBrightness";
    sg_WR_DataTemplate[41].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[41].state = eNOCHANGE;

    sg_WR_DataTemplate[42].data_property.data = sg_SnapshotStatus_DataTemplate;
    sg_WR_DataTemplate[42].data_property.data_buff_len = SNAPSHOTSTATUS_MEMBER_COUNT;
    sg_WR_DataTemplate[42].data_property.key  = "SnapshotStatus";
    sg_WR_DataTemplate[42].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[42].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_TimingShield, 0, sizeof(g_ivm_objs.ProWritable.m_TimingShield));
    sg_WR_DataTemplate[43].data_property.data = &g_ivm_objs.ProWritable.m_TimingShield;
    sg_WR_DataTemplate[43].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[43].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[43].data_property.key  = "TimingShield";
    sg_WR_DataTemplate[43].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[43].state = eNOCHANGE;

    sg_WR_DataTemplate[44].data_property.data = sg_Onvif_DataTemplate;
    sg_WR_DataTemplate[44].data_property.data_buff_len = ONVIF_MEMBER_COUNT;
    sg_WR_DataTemplate[44].data_property.key  = "Onvif";
    sg_WR_DataTemplate[44].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[44].state = eNOCHANGE;

    sg_WR_DataTemplate[45].data_property.data = &g_ivm_objs.ProWritable.m_HomeGuardSwitch;
    sg_WR_DataTemplate[45].data_property.key  = "HomeGuardSwitch";
    sg_WR_DataTemplate[45].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_WR_DataTemplate[45].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_HumanDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_HumanDetectConfig));
    sg_WR_DataTemplate[46].data_property.data = &g_ivm_objs.ProWritable.m_HumanDetectConfig;
    sg_WR_DataTemplate[46].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[46].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[46].data_property.key  = "HumanDetectConfig";
    sg_WR_DataTemplate[46].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[46].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_SoundLightAlarmConfig, 0, sizeof(g_ivm_objs.ProWritable.m_SoundLightAlarmConfig));
    sg_WR_DataTemplate[47].data_property.data = &g_ivm_objs.ProWritable.m_SoundLightAlarmConfig;
    sg_WR_DataTemplate[47].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[47].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[47].data_property.key  = "SoundLightAlarmConfig";
    sg_WR_DataTemplate[47].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[47].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_CarDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_CarDetectConfig));
    sg_WR_DataTemplate[48].data_property.data = &g_ivm_objs.ProWritable.m_CarDetectConfig;
    sg_WR_DataTemplate[48].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[48].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[48].data_property.key  = "CarDetectConfig";
    sg_WR_DataTemplate[48].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[48].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_BikeDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_BikeDetectConfig));
    sg_WR_DataTemplate[49].data_property.data = &g_ivm_objs.ProWritable.m_BikeDetectConfig;
    sg_WR_DataTemplate[49].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[49].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[49].data_property.key  = "BikeDetectConfig";
    sg_WR_DataTemplate[49].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[49].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_PetDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_PetDetectConfig));
    sg_WR_DataTemplate[50].data_property.data = &g_ivm_objs.ProWritable.m_PetDetectConfig;
    sg_WR_DataTemplate[50].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[50].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[50].data_property.key  = "PetDetectConfig";
    sg_WR_DataTemplate[50].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[50].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_ImageDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_ImageDetectConfig));
    sg_WR_DataTemplate[51].data_property.data = &g_ivm_objs.ProWritable.m_ImageDetectConfig;
    sg_WR_DataTemplate[51].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[51].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[51].data_property.key  = "ImageDetectConfig";
    sg_WR_DataTemplate[51].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[51].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_HideDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_HideDetectConfig));
    sg_WR_DataTemplate[52].data_property.data = &g_ivm_objs.ProWritable.m_HideDetectConfig;
    sg_WR_DataTemplate[52].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[52].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[52].data_property.key  = "HideDetectConfig";
    sg_WR_DataTemplate[52].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[52].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_CryDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_CryDetectConfig));
    sg_WR_DataTemplate[53].data_property.data = &g_ivm_objs.ProWritable.m_CryDetectConfig;
    sg_WR_DataTemplate[53].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[53].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[53].data_property.key  = "CryDetectConfig";
    sg_WR_DataTemplate[53].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[53].state = eNOCHANGE;

    sg_WR_DataTemplate[54].data_property.data = sg_MainStreamBitrate_DataTemplate;
    sg_WR_DataTemplate[54].data_property.data_buff_len = MAINSTREAMBITRATE_MEMBER_COUNT;
    sg_WR_DataTemplate[54].data_property.key  = "MainStreamBitrate";
    sg_WR_DataTemplate[54].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[54].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_VideoEncoding, 0, sizeof(g_ivm_objs.ProWritable.m_VideoEncoding));
    sg_WR_DataTemplate[55].data_property.data = &g_ivm_objs.ProWritable.m_VideoEncoding;
    sg_WR_DataTemplate[55].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[55].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[55].data_property.key  = "VideoEncoding";
    sg_WR_DataTemplate[55].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[55].state = eNOCHANGE;

    sg_WR_DataTemplate[56].data_property.data = sg_DeviceLogFileUpload_DataTemplate;
    sg_WR_DataTemplate[56].data_property.data_buff_len = DEVICELOGFILEUPLOAD_MEMBER_COUNT;
    sg_WR_DataTemplate[56].data_property.key  = "DeviceLogFileUpload";
    sg_WR_DataTemplate[56].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[56].state = eNOCHANGE;

    sg_WR_DataTemplate[57].data_property.data = sg_DeviceLogRedirect_DataTemplate;
    sg_WR_DataTemplate[57].data_property.data_buff_len = DEVICELOGREDIRECT_MEMBER_COUNT;
    sg_WR_DataTemplate[57].data_property.key  = "DeviceLogRedirect";
    sg_WR_DataTemplate[57].data_property.type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[57].state = eNOCHANGE;

    sg_WR_DataTemplate[58].data_property.data = &g_ivm_objs.ProWritable.m_ImageMode;
    sg_WR_DataTemplate[58].data_property.key  = "ImageMode";
    sg_WR_DataTemplate[58].data_property.type = TYPE_TEMPLATE_ENUM;
    sg_WR_DataTemplate[58].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_RegionDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_RegionDetectConfig));
    sg_WR_DataTemplate[59].data_property.data = &g_ivm_objs.ProWritable.m_RegionDetectConfig;
    sg_WR_DataTemplate[59].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[59].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[59].data_property.key  = "RegionDetectConfig";
    sg_WR_DataTemplate[59].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[59].state = eNOCHANGE;

    memset(&g_ivm_objs.ProWritable.m_OverLineDetectConfig, 0, sizeof(g_ivm_objs.ProWritable.m_OverLineDetectConfig));
    sg_WR_DataTemplate[60].data_property.data = &g_ivm_objs.ProWritable.m_OverLineDetectConfig;
    sg_WR_DataTemplate[60].data_property.array_info.array_size = MAX_ARRAY_JSON_STR_LEN;
    sg_WR_DataTemplate[60].data_property.array_info.array_type = TYPE_TEMPLATE_JOBJECT;
    sg_WR_DataTemplate[60].data_property.key  = "OverLineDetectConfig";
    sg_WR_DataTemplate[60].data_property.type = TYPE_TEMPLATE_ARRAY;
    sg_WR_DataTemplate[60].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[0].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceType;
    sg_DeviceInformation_DataTemplate[0].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceType)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceType[0]);
    sg_DeviceInformation_DataTemplate[0].data_property.key  = "DeviceType";
    sg_DeviceInformation_DataTemplate[0].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[0].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[1].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceUuid;
    sg_DeviceInformation_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceUuid)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceUuid[0]);
    sg_DeviceInformation_DataTemplate[1].data_property.key  = "DeviceUuid";
    sg_DeviceInformation_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[1].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[2].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_WifiName;
    sg_DeviceInformation_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_WifiName)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_WifiName[0]);
    sg_DeviceInformation_DataTemplate[2].data_property.key  = "WifiName";
    sg_DeviceInformation_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[2].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[3].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_SystemVersion;
    sg_DeviceInformation_DataTemplate[3].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_SystemVersion)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_SystemVersion[0]);
    sg_DeviceInformation_DataTemplate[3].data_property.key  = "SystemVersion";
    sg_DeviceInformation_DataTemplate[3].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[3].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[4].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_IpAddress;
    sg_DeviceInformation_DataTemplate[4].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_IpAddress)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_IpAddress[0]);
    sg_DeviceInformation_DataTemplate[4].data_property.key  = "IpAddress";
    sg_DeviceInformation_DataTemplate[4].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[4].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[5].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_MacAddress;
    sg_DeviceInformation_DataTemplate[5].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_MacAddress)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_MacAddress[0]);
    sg_DeviceInformation_DataTemplate[5].data_property.key  = "MacAddress";
    sg_DeviceInformation_DataTemplate[5].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[5].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[6].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceSn;
    sg_DeviceInformation_DataTemplate[6].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceSn)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_DeviceSn[0]);
    sg_DeviceInformation_DataTemplate[6].data_property.key  = "DeviceSn";
    sg_DeviceInformation_DataTemplate[6].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[6].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[7].data_property.data = g_ivm_objs.ProReadonly.m_DeviceInformation.m_PublishTime;
    sg_DeviceInformation_DataTemplate[7].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_PublishTime)/sizeof(g_ivm_objs.ProReadonly.m_DeviceInformation.m_PublishTime[0]);
    sg_DeviceInformation_DataTemplate[7].data_property.key  = "PublishTime";
    sg_DeviceInformation_DataTemplate[7].data_property.type = TYPE_TEMPLATE_STRING;
    sg_DeviceInformation_DataTemplate[7].state = eNOCHANGE;

    sg_DeviceInformation_DataTemplate[8].data_property.data = &g_ivm_objs.ProReadonly.m_DeviceInformation.m_ShowIpMac;
    sg_DeviceInformation_DataTemplate[8].data_property.key  = "ShowIpMac";
    sg_DeviceInformation_DataTemplate[8].data_property.type = TYPE_TEMPLATE_INT;
    sg_DeviceInformation_DataTemplate[8].state = eNOCHANGE;

    sg_ResolutionFunctionSet_DataTemplate[0].data_property.data = &g_ivm_objs.ProReadonly.m_ResolutionFunctionSet.m_b720P;
    sg_ResolutionFunctionSet_DataTemplate[0].data_property.key  = "b720P";
    sg_ResolutionFunctionSet_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_ResolutionFunctionSet_DataTemplate[0].state = eNOCHANGE;

    sg_ResolutionFunctionSet_DataTemplate[1].data_property.data = &g_ivm_objs.ProReadonly.m_ResolutionFunctionSet.m_b1080P;
    sg_ResolutionFunctionSet_DataTemplate[1].data_property.key  = "b1080P";
    sg_ResolutionFunctionSet_DataTemplate[1].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_ResolutionFunctionSet_DataTemplate[1].state = eNOCHANGE;

    sg_ResolutionFunctionSet_DataTemplate[2].data_property.data = &g_ivm_objs.ProReadonly.m_ResolutionFunctionSet.m_b3MP;
    sg_ResolutionFunctionSet_DataTemplate[2].data_property.key  = "b3MP";
    sg_ResolutionFunctionSet_DataTemplate[2].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_ResolutionFunctionSet_DataTemplate[2].state = eNOCHANGE;

    sg_ResolutionFunctionSet_DataTemplate[3].data_property.data = &g_ivm_objs.ProReadonly.m_ResolutionFunctionSet.m_b4MP;
    sg_ResolutionFunctionSet_DataTemplate[3].data_property.key  = "b4MP";
    sg_ResolutionFunctionSet_DataTemplate[3].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_ResolutionFunctionSet_DataTemplate[3].state = eNOCHANGE;

    sg_ResolutionFunctionSet_DataTemplate[4].data_property.data = &g_ivm_objs.ProReadonly.m_ResolutionFunctionSet.m_b5MP;
    sg_ResolutionFunctionSet_DataTemplate[4].data_property.key  = "b5MP";
    sg_ResolutionFunctionSet_DataTemplate[4].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_ResolutionFunctionSet_DataTemplate[4].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[0].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_Reboot;
    sg_FunctionSet_DataTemplate[0].data_property.key  = "Reboot";
    sg_FunctionSet_DataTemplate[0].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[0].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[1].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_TimingReboot;
    sg_FunctionSet_DataTemplate[1].data_property.key  = "TimingReboot";
    sg_FunctionSet_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[1].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[2].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SpeedPlaySd;
    sg_FunctionSet_DataTemplate[2].data_property.key  = "SpeedPlaySd";
    sg_FunctionSet_DataTemplate[2].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[2].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[3].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SingleTalk;
    sg_FunctionSet_DataTemplate[3].data_property.key  = "SingleTalk";
    sg_FunctionSet_DataTemplate[3].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[3].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[4].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_OTAUpgradeProgress;
    sg_FunctionSet_DataTemplate[4].data_property.key  = "OTAUpgradeProgress";
    sg_FunctionSet_DataTemplate[4].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[4].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[5].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_OneKeyAlarm;
    sg_FunctionSet_DataTemplate[5].data_property.key  = "OneKeyAlarm";
    sg_FunctionSet_DataTemplate[5].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[5].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[6].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SdPlayerV2;
    sg_FunctionSet_DataTemplate[6].data_property.key  = "SdPlayerV2";
    sg_FunctionSet_DataTemplate[6].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[6].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[7].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_PowerLineFrequency;
    sg_FunctionSet_DataTemplate[7].data_property.key  = "PowerLineFrequency";
    sg_FunctionSet_DataTemplate[7].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[7].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[8].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SDSeek;
    sg_FunctionSet_DataTemplate[8].data_property.key  = "SDSeek";
    sg_FunctionSet_DataTemplate[8].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[8].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[9].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_RTMPVersions;
    sg_FunctionSet_DataTemplate[9].data_property.key  = "RTMPVersions";
    sg_FunctionSet_DataTemplate[9].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[9].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[10].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SDPlayFirst;
    sg_FunctionSet_DataTemplate[10].data_property.key  = "SDPlayFirst";
    sg_FunctionSet_DataTemplate[10].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[10].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[11].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportWLightBrightness;
    sg_FunctionSet_DataTemplate[11].data_property.key  = "SupportWLightBrightness";
    sg_FunctionSet_DataTemplate[11].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[11].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[12].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportSpeedPlayBack;
    sg_FunctionSet_DataTemplate[12].data_property.key  = "SupportSpeedPlayBack";
    sg_FunctionSet_DataTemplate[12].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[12].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[13].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportDefaultCfg;
    sg_FunctionSet_DataTemplate[13].data_property.key  = "SupportDefaultCfg";
    sg_FunctionSet_DataTemplate[13].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[13].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[14].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportCloseNightMode;
    sg_FunctionSet_DataTemplate[14].data_property.key  = "SupportCloseNightMode";
    sg_FunctionSet_DataTemplate[14].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[14].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[15].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportRegion;
    sg_FunctionSet_DataTemplate[15].data_property.key  = "SupportRegion";
    sg_FunctionSet_DataTemplate[15].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[15].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[16].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportOverline;
    sg_FunctionSet_DataTemplate[16].data_property.key  = "SupportOverline";
    sg_FunctionSet_DataTemplate[16].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[16].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[17].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportEventAudioLight;
    sg_FunctionSet_DataTemplate[17].data_property.key  = "SupportEventAudioLight";
    sg_FunctionSet_DataTemplate[17].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[17].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[18].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportChannelsLinkage;
    sg_FunctionSet_DataTemplate[18].data_property.key  = "SupportChannelsLinkage";
    sg_FunctionSet_DataTemplate[18].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[18].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[19].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportMoveToCoord;
    sg_FunctionSet_DataTemplate[19].data_property.key  = "SupportMoveToCoord";
    sg_FunctionSet_DataTemplate[19].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[19].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[20].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SDPlayBackVersion;
    sg_FunctionSet_DataTemplate[20].data_property.key  = "SDPlayBackVersion";
    sg_FunctionSet_DataTemplate[20].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[20].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[21].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_HomeGuardVersion;
    sg_FunctionSet_DataTemplate[21].data_property.key  = "HomeGuardVersion";
    sg_FunctionSet_DataTemplate[21].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[21].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[22].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportOneKeyCall;
    sg_FunctionSet_DataTemplate[22].data_property.key  = "SupportOneKeyCall";
    sg_FunctionSet_DataTemplate[22].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[22].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[23].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportManualTimeCalibration;
    sg_FunctionSet_DataTemplate[23].data_property.key  = "SupportManualTimeCalibration";
    sg_FunctionSet_DataTemplate[23].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[23].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[24].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportMainStreamBitrate;
    sg_FunctionSet_DataTemplate[24].data_property.key  = "SupportMainStreamBitrate";
    sg_FunctionSet_DataTemplate[24].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[24].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[25].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SDSeekPlayUseMS;
    sg_FunctionSet_DataTemplate[25].data_property.key  = "SDSeekPlayUseMS";
    sg_FunctionSet_DataTemplate[25].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[25].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[26].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_Support4G;
    sg_FunctionSet_DataTemplate[26].data_property.key  = "Support4G";
    sg_FunctionSet_DataTemplate[26].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[26].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[27].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportSwitchSimCards;
    sg_FunctionSet_DataTemplate[27].data_property.key  = "SupportSwitchSimCards";
    sg_FunctionSet_DataTemplate[27].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[27].state = eNOCHANGE;

    sg_FunctionSet_DataTemplate[28].data_property.data = &g_ivm_objs.ProReadonly.m_FunctionSet.m_SupportImageMode;
    sg_FunctionSet_DataTemplate[28].data_property.key  = "SupportImageMode";
    sg_FunctionSet_DataTemplate[28].data_property.type = TYPE_TEMPLATE_INT;
    sg_FunctionSet_DataTemplate[28].state = eNOCHANGE;

    sg_Information4G_DataTemplate[0].data_property.data = &g_ivm_objs.ProReadonly.m_Information4G.m_SignalStrength;
    sg_Information4G_DataTemplate[0].data_property.key  = "SignalStrength";
    sg_Information4G_DataTemplate[0].data_property.type = TYPE_TEMPLATE_INT;
    sg_Information4G_DataTemplate[0].state = eNOCHANGE;

    sg_Information4G_DataTemplate[1].data_property.data = g_ivm_objs.ProReadonly.m_Information4G.m_SIM1ICCID;
    sg_Information4G_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM1ICCID)/sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM1ICCID[0]);
    sg_Information4G_DataTemplate[1].data_property.key  = "SIM1ICCID";
    sg_Information4G_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_Information4G_DataTemplate[1].state = eNOCHANGE;

    sg_Information4G_DataTemplate[2].data_property.data = g_ivm_objs.ProReadonly.m_Information4G.m_SIM2ICCID;
    sg_Information4G_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM2ICCID)/sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM2ICCID[0]);
    sg_Information4G_DataTemplate[2].data_property.key  = "SIM2ICCID";
    sg_Information4G_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_Information4G_DataTemplate[2].state = eNOCHANGE;

    sg_Information4G_DataTemplate[3].data_property.data = &g_ivm_objs.ProReadonly.m_Information4G.m_TrafficUsage;
    sg_Information4G_DataTemplate[3].data_property.key  = "TrafficUsage";
    sg_Information4G_DataTemplate[3].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_Information4G_DataTemplate[3].state = eNOCHANGE;

    sg_Information4G_DataTemplate[4].data_property.data = &g_ivm_objs.ProReadonly.m_Information4G.m_UseSIMID;
    sg_Information4G_DataTemplate[4].data_property.key  = "UseSIMID";
    sg_Information4G_DataTemplate[4].data_property.type = TYPE_TEMPLATE_INT;
    sg_Information4G_DataTemplate[4].state = eNOCHANGE;

    sg_Information4G_DataTemplate[5].data_property.data = &g_ivm_objs.ProReadonly.m_Information4G.m_RSRP;
    sg_Information4G_DataTemplate[5].data_property.key  = "RSRP";
    sg_Information4G_DataTemplate[5].data_property.type = TYPE_TEMPLATE_INT;
    sg_Information4G_DataTemplate[5].state = eNOCHANGE;

    sg_Information4G_DataTemplate[6].data_property.data = &g_ivm_objs.ProReadonly.m_Information4G.m_SINR;
    sg_Information4G_DataTemplate[6].data_property.key  = "SINR";
    sg_Information4G_DataTemplate[6].data_property.type = TYPE_TEMPLATE_INT;
    sg_Information4G_DataTemplate[6].state = eNOCHANGE;

    sg_Information4G_DataTemplate[7].data_property.data = g_ivm_objs.ProReadonly.m_Information4G.m_SIM1Operator;
    sg_Information4G_DataTemplate[7].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM1Operator)/sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM1Operator[0]);
    sg_Information4G_DataTemplate[7].data_property.key  = "SIM1Operator";
    sg_Information4G_DataTemplate[7].data_property.type = TYPE_TEMPLATE_STRING;
    sg_Information4G_DataTemplate[7].state = eNOCHANGE;

    sg_Information4G_DataTemplate[8].data_property.data = g_ivm_objs.ProReadonly.m_Information4G.m_SIM2Operator;
    sg_Information4G_DataTemplate[8].data_property.data_buff_len = sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM2Operator)/sizeof(g_ivm_objs.ProReadonly.m_Information4G.m_SIM2Operator[0]);
    sg_Information4G_DataTemplate[8].data_property.key  = "SIM2Operator";
    sg_Information4G_DataTemplate[8].data_property.type = TYPE_TEMPLATE_STRING;
    sg_Information4G_DataTemplate[8].state = eNOCHANGE;

    sg_TimeZoneSetting_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_TimeZoneSetting.m_TimeZoneID;
    sg_TimeZoneSetting_DataTemplate[0].data_property.key  = "TimeZoneID";
    sg_TimeZoneSetting_DataTemplate[0].data_property.type = TYPE_TEMPLATE_INT;
    sg_TimeZoneSetting_DataTemplate[0].state = eNOCHANGE;

    sg_TimeZoneSetting_DataTemplate[1].data_property.data = g_ivm_objs.ProWritable.m_TimeZoneSetting.m_TimeZone;
    sg_TimeZoneSetting_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_TimeZoneSetting.m_TimeZone)/sizeof(g_ivm_objs.ProWritable.m_TimeZoneSetting.m_TimeZone[0]);
    sg_TimeZoneSetting_DataTemplate[1].data_property.key  = "TimeZone";
    sg_TimeZoneSetting_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_TimeZoneSetting_DataTemplate[1].state = eNOCHANGE;

    sg_WifiConfiguration_DataTemplate[0].data_property.data = g_ivm_objs.ProWritable.m_WifiConfiguration.m_WifiName;
    sg_WifiConfiguration_DataTemplate[0].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_WifiConfiguration.m_WifiName)/sizeof(g_ivm_objs.ProWritable.m_WifiConfiguration.m_WifiName[0]);
    sg_WifiConfiguration_DataTemplate[0].data_property.key  = "WifiName";
    sg_WifiConfiguration_DataTemplate[0].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WifiConfiguration_DataTemplate[0].state = eNOCHANGE;

    sg_WifiConfiguration_DataTemplate[1].data_property.data = g_ivm_objs.ProWritable.m_WifiConfiguration.m_WifiPasswd;
    sg_WifiConfiguration_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_WifiConfiguration.m_WifiPasswd)/sizeof(g_ivm_objs.ProWritable.m_WifiConfiguration.m_WifiPasswd[0]);
    sg_WifiConfiguration_DataTemplate[1].data_property.key  = "WifiPasswd";
    sg_WifiConfiguration_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WifiConfiguration_DataTemplate[1].state = eNOCHANGE;

    sg_WifiConfiguration_DataTemplate[2].data_property.data = g_ivm_objs.ProWritable.m_WifiConfiguration.m_Encryption;
    sg_WifiConfiguration_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_WifiConfiguration.m_Encryption)/sizeof(g_ivm_objs.ProWritable.m_WifiConfiguration.m_Encryption[0]);
    sg_WifiConfiguration_DataTemplate[2].data_property.key  = "Encryption";
    sg_WifiConfiguration_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_WifiConfiguration_DataTemplate[2].state = eNOCHANGE;

    sg_RTMPInfo_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPEnable;
    sg_RTMPInfo_DataTemplate[0].data_property.key  = "RTMPEnable";
    sg_RTMPInfo_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_RTMPInfo_DataTemplate[0].state = eNOCHANGE;

    sg_RTMPInfo_DataTemplate[1].data_property.data = g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPURL;
    sg_RTMPInfo_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPURL)/sizeof(g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPURL[0]);
    sg_RTMPInfo_DataTemplate[1].data_property.key  = "RTMPURL";
    sg_RTMPInfo_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_RTMPInfo_DataTemplate[1].state = eNOCHANGE;

    sg_RTMPInfo_DataTemplate[2].data_property.data = g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPKey;
    sg_RTMPInfo_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPKey)/sizeof(g_ivm_objs.ProWritable.m_RTMPInfo.m_RTMPKey[0]);
    sg_RTMPInfo_DataTemplate[2].data_property.key  = "RTMPKey";
    sg_RTMPInfo_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_RTMPInfo_DataTemplate[2].state = eNOCHANGE;

    sg_DynamicInformation_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_DynamicInformation.m_RunTime;
    sg_DynamicInformation_DataTemplate[0].data_property.key  = "RunTime";
    sg_DynamicInformation_DataTemplate[0].data_property.type = TYPE_TEMPLATE_INT;
    sg_DynamicInformation_DataTemplate[0].state = eNOCHANGE;

    sg_DynamicInformation_DataTemplate[1].data_property.data = &g_ivm_objs.ProWritable.m_DynamicInformation.m_StorageRemainCapacity;
    sg_DynamicInformation_DataTemplate[1].data_property.key  = "StorageRemainCapacity";
    sg_DynamicInformation_DataTemplate[1].data_property.type = TYPE_TEMPLATE_FLOAT;
    sg_DynamicInformation_DataTemplate[1].state = eNOCHANGE;

    sg_DynamicInformation_DataTemplate[2].data_property.data = &g_ivm_objs.ProWritable.m_DynamicInformation.m_OnlineDuration;
    sg_DynamicInformation_DataTemplate[2].data_property.key  = "OnlineDuration";
    sg_DynamicInformation_DataTemplate[2].data_property.type = TYPE_TEMPLATE_INT;
    sg_DynamicInformation_DataTemplate[2].state = eNOCHANGE;

    sg_TimingReboot_DataTemplate[0].data_property.data = g_ivm_objs.ProWritable.m_TimingReboot.m_RebootDay;
    sg_TimingReboot_DataTemplate[0].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_TimingReboot.m_RebootDay)/sizeof(g_ivm_objs.ProWritable.m_TimingReboot.m_RebootDay[0]);
    sg_TimingReboot_DataTemplate[0].data_property.key  = "RebootDay";
    sg_TimingReboot_DataTemplate[0].data_property.type = TYPE_TEMPLATE_STRING;
    sg_TimingReboot_DataTemplate[0].state = eNOCHANGE;

    sg_TimingReboot_DataTemplate[1].data_property.data = &g_ivm_objs.ProWritable.m_TimingReboot.m_RebootEnable;
    sg_TimingReboot_DataTemplate[1].data_property.key  = "RebootEnable";
    sg_TimingReboot_DataTemplate[1].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_TimingReboot_DataTemplate[1].state = eNOCHANGE;

    sg_TimingReboot_DataTemplate[2].data_property.data = g_ivm_objs.ProWritable.m_TimingReboot.m_RebootTime;
    sg_TimingReboot_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_TimingReboot.m_RebootTime)/sizeof(g_ivm_objs.ProWritable.m_TimingReboot.m_RebootTime[0]);
    sg_TimingReboot_DataTemplate[2].data_property.key  = "RebootTime";
    sg_TimingReboot_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_TimingReboot_DataTemplate[2].state = eNOCHANGE;

    sg_SnapshotStatus_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_SnapshotStatus.m_channelID;
    sg_SnapshotStatus_DataTemplate[0].data_property.key  = "channelID";
    sg_SnapshotStatus_DataTemplate[0].data_property.type = TYPE_TEMPLATE_INT;
    sg_SnapshotStatus_DataTemplate[0].state = eNOCHANGE;

    sg_SnapshotStatus_DataTemplate[1].data_property.data = g_ivm_objs.ProWritable.m_SnapshotStatus.m_picUrl;
    sg_SnapshotStatus_DataTemplate[1].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_SnapshotStatus.m_picUrl)/sizeof(g_ivm_objs.ProWritable.m_SnapshotStatus.m_picUrl[0]);
    sg_SnapshotStatus_DataTemplate[1].data_property.key  = "picUrl";
    sg_SnapshotStatus_DataTemplate[1].data_property.type = TYPE_TEMPLATE_STRING;
    sg_SnapshotStatus_DataTemplate[1].state = eNOCHANGE;

    sg_SnapshotStatus_DataTemplate[2].data_property.data = g_ivm_objs.ProWritable.m_SnapshotStatus.m_iotId;
    sg_SnapshotStatus_DataTemplate[2].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_SnapshotStatus.m_iotId)/sizeof(g_ivm_objs.ProWritable.m_SnapshotStatus.m_iotId[0]);
    sg_SnapshotStatus_DataTemplate[2].data_property.key  = "iotId";
    sg_SnapshotStatus_DataTemplate[2].data_property.type = TYPE_TEMPLATE_STRING;
    sg_SnapshotStatus_DataTemplate[2].state = eNOCHANGE;

    sg_Onvif_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_Onvif.m_Enable;
    sg_Onvif_DataTemplate[0].data_property.key  = "Enable";
    sg_Onvif_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_Onvif_DataTemplate[0].state = eNOCHANGE;

    sg_Onvif_DataTemplate[1].data_property.data = &g_ivm_objs.ProWritable.m_Onvif.m_EnableSub;
    sg_Onvif_DataTemplate[1].data_property.key  = "EnableSub";
    sg_Onvif_DataTemplate[1].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_Onvif_DataTemplate[1].state = eNOCHANGE;

    sg_MainStreamBitrate_DataTemplate[0].data_property.data = g_ivm_objs.ProWritable.m_MainStreamBitrate.m_list;
    sg_MainStreamBitrate_DataTemplate[0].data_property.data_buff_len = sizeof(g_ivm_objs.ProWritable.m_MainStreamBitrate.m_list)/sizeof(g_ivm_objs.ProWritable.m_MainStreamBitrate.m_list[0]);
    sg_MainStreamBitrate_DataTemplate[0].data_property.key  = "list";
    sg_MainStreamBitrate_DataTemplate[0].data_property.type = TYPE_TEMPLATE_STRING;
    sg_MainStreamBitrate_DataTemplate[0].state = eNOCHANGE;

    sg_MainStreamBitrate_DataTemplate[1].data_property.data = &g_ivm_objs.ProWritable.m_MainStreamBitrate.m_CurrentBitRate;
    sg_MainStreamBitrate_DataTemplate[1].data_property.key  = "CurrentBitRate";
    sg_MainStreamBitrate_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;
    sg_MainStreamBitrate_DataTemplate[1].state = eNOCHANGE;

    sg_DeviceLogFileUpload_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_DeviceLogFileUpload.m_Enable;
    sg_DeviceLogFileUpload_DataTemplate[0].data_property.key  = "Enable";
    sg_DeviceLogFileUpload_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_DeviceLogFileUpload_DataTemplate[0].state = eNOCHANGE;

    sg_DeviceLogFileUpload_DataTemplate[1].data_property.data = &g_ivm_objs.ProWritable.m_DeviceLogFileUpload.m_ChunkSizeMB;
    sg_DeviceLogFileUpload_DataTemplate[1].data_property.key  = "ChunkSizeMB";
    sg_DeviceLogFileUpload_DataTemplate[1].data_property.type = TYPE_TEMPLATE_INT;
    sg_DeviceLogFileUpload_DataTemplate[1].state = eNOCHANGE;

    sg_DeviceLogRedirect_DataTemplate[0].data_property.data = &g_ivm_objs.ProWritable.m_DeviceLogRedirect.m_Enable;
    sg_DeviceLogRedirect_DataTemplate[0].data_property.key  = "Enable";
    sg_DeviceLogRedirect_DataTemplate[0].data_property.type = TYPE_TEMPLATE_BOOL;
    sg_DeviceLogRedirect_DataTemplate[0].state = eNOCHANGE;

};

#define TOTAL_ACTION_COUNTS     (20)

static ivm_TextQuery_t sg_TextQuery;
static DeviceProperty g_actionInput_TextQuery[] = {
   {.key = "DataChannelType", .data_buff_len = 0, .data = &sg_TextQuery.action_in.m_DataChannelType, .type = TYPE_TEMPLATE_INT},   {.key = "Data", .data_buff_len = sizeof(sg_TextQuery.action_in.m_Data) - 1, .data = sg_TextQuery.action_in.m_Data, .type = TYPE_TEMPLATE_STRING},
};
static DeviceProperty g_actionOutput_TextQuery[] = {
   {.key = "DataChannelType", .data_buff_len = 0, .data = &sg_TextQuery.action_out.m_DataChannelType, .type = TYPE_TEMPLATE_INT},   {.key = "Data", .data_buff_len = sizeof(sg_TextQuery.action_out.m_Data) - 1, .data = sg_TextQuery.action_out.m_Data, .type = TYPE_TEMPLATE_STRING},
};
static ivm_SetDefaultVideoColor_t sg_SetDefaultVideoColor;
static DeviceProperty g_actionInput_SetDefaultVideoColor[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_SetDefaultVideoColor.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_SetDefaultVideoColor[] = {

};
static ivm_OneKeyAlarm_t sg_OneKeyAlarm;
static DeviceProperty g_actionInput_OneKeyAlarm[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_OneKeyAlarm.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},   {.key = "Operate", .data_buff_len = 0, .data = &sg_OneKeyAlarm.action_in.m_Operate, .type = TYPE_TEMPLATE_BOOL},
};
static DeviceProperty g_actionOutput_OneKeyAlarm[] = {

};
static ivm_ReplyCall_t sg_ReplyCall;
static DeviceProperty g_actionInput_ReplyCall[] = {
   {.key = "DataChannelType", .data_buff_len = 0, .data = &sg_ReplyCall.action_in.m_DataChannelType, .type = TYPE_TEMPLATE_INT},   {.key = "ReplyType", .data_buff_len = 0, .data = &sg_ReplyCall.action_in.m_ReplyType, .type = TYPE_TEMPLATE_ENUM},
};
static DeviceProperty g_actionOutput_ReplyCall[] = {

};
static DeviceProperty g_actionInput_FormatStorageMedium[] = {

};
static DeviceProperty g_actionOutput_FormatStorageMedium[] = {

};
static DeviceProperty g_actionInput_Reboot[] = {

};
static DeviceProperty g_actionOutput_Reboot[] = {

};
static ivm_StartPTZCruise_t sg_StartPTZCruise;
static DeviceProperty g_actionInput_StartPTZCruise[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_StartPTZCruise.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},   {.key = "PresetID", .data_buff_len = 0, .data = &sg_StartPTZCruise.action_in.m_PresetID, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_StartPTZCruise[] = {

};
static ivm_AddPTZCruise_t sg_AddPTZCruise;
static DeviceProperty g_actionInput_AddPTZCruise[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_AddPTZCruise.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},   {.key = "PresetID", .data_buff_len = 0, .data = &sg_AddPTZCruise.action_in.m_PresetID, .type = TYPE_TEMPLATE_INT},   {.key = "Presetname", .data_buff_len = sizeof(sg_AddPTZCruise.action_in.m_Presetname) - 1, .data = sg_AddPTZCruise.action_in.m_Presetname, .type = TYPE_TEMPLATE_STRING},
};
static DeviceProperty g_actionOutput_AddPTZCruise[] = {

};
static ivm_DelPTZCruise_t sg_DelPTZCruise;
static DeviceProperty g_actionInput_DelPTZCruise[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_DelPTZCruise.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},   {.key = "PresetID", .data_buff_len = 0, .data = &sg_DelPTZCruise.action_in.m_PresetID, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_DelPTZCruise[] = {

};
static ivm_PreviewImageCapture_t sg_PreviewImageCapture;
static DeviceProperty g_actionInput_PreviewImageCapture[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_PreviewImageCapture.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_PreviewImageCapture[] = {

};
static ivm_ResetPTZ_t sg_ResetPTZ;
static DeviceProperty g_actionInput_ResetPTZ[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_ResetPTZ.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_ResetPTZ[] = {

};
static ivm_GotoPTZStation_t sg_GotoPTZStation;
static DeviceProperty g_actionInput_GotoPTZStation[] = {
   {.key = "ChannelID", .data_buff_len = 0, .data = &sg_GotoPTZStation.action_in.m_ChannelID, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_GotoPTZStation[] = {

};
static DeviceProperty g_actionInput_SetDefaultDevConfigs[] = {

};
static DeviceProperty g_actionOutput_SetDefaultDevConfigs[] = {

};
static ivm_ManualTimeCalibration_t sg_ManualTimeCalibration;
static DeviceProperty g_actionInput_ManualTimeCalibration[] = {
   {.key = "UTCTime", .data_buff_len = 0, .data = &sg_ManualTimeCalibration.action_in.m_UTCTime, .type = TYPE_TEMPLATE_TIME},
};
static DeviceProperty g_actionOutput_ManualTimeCalibration[] = {

};
static ivm_CloudStorageStaus_t sg_CloudStorageStaus;
static DeviceProperty g_actionInput_CloudStorageStaus[] = {
   {.key = "status", .data_buff_len = 0, .data = &sg_CloudStorageStaus.action_in.m_status, .type = TYPE_TEMPLATE_BOOL},
};
static DeviceProperty g_actionOutput_CloudStorageStaus[] = {

};
static ivm_DeviceFileUpload_t sg_DeviceFileUpload;
static DeviceProperty g_actionInput_DeviceFileUpload[] = {
   {.key = "FileName", .data_buff_len = sizeof(sg_DeviceFileUpload.action_in.m_FileName) - 1, .data = sg_DeviceFileUpload.action_in.m_FileName, .type = TYPE_TEMPLATE_STRING},   {.key = "ChunkSizeMB", .data_buff_len = 0, .data = &sg_DeviceFileUpload.action_in.m_ChunkSizeMB, .type = TYPE_TEMPLATE_INT},
};
static DeviceProperty g_actionOutput_DeviceFileUpload[] = {

};
static ivm_DeviceFileUploadStatus_t sg_DeviceFileUploadStatus;
static DeviceProperty g_actionInput_DeviceFileUploadStatus[] = {

};
static DeviceProperty g_actionOutput_DeviceFileUploadStatus[] = {
   {.key = "FileName", .data_buff_len = sizeof(sg_DeviceFileUploadStatus.action_out.m_FileName) - 1, .data = sg_DeviceFileUploadStatus.action_out.m_FileName, .type = TYPE_TEMPLATE_STRING},   {.key = "Status", .data_buff_len = 0, .data = &sg_DeviceFileUploadStatus.action_out.m_Status, .type = TYPE_TEMPLATE_ENUM},   {.key = "TotalChunks", .data_buff_len = 0, .data = &sg_DeviceFileUploadStatus.action_out.m_TotalChunks, .type = TYPE_TEMPLATE_INT},   {.key = "CompletedChunks", .data_buff_len = 0, .data = &sg_DeviceFileUploadStatus.action_out.m_CompletedChunks, .type = TYPE_TEMPLATE_INT},   {.key = "ErrorMsg", .data_buff_len = sizeof(sg_DeviceFileUploadStatus.action_out.m_ErrorMsg) - 1, .data = sg_DeviceFileUploadStatus.action_out.m_ErrorMsg, .type = TYPE_TEMPLATE_STRING},
};
static ivm_BindOrDelete_t sg_BindOrDelete;
static DeviceProperty g_actionInput_BindOrDelete[] = {
   {.key = "command", .data_buff_len = 0, .data = &sg_BindOrDelete.action_in.m_command, .type = TYPE_TEMPLATE_ENUM},
};
static DeviceProperty g_actionOutput_BindOrDelete[] = {

};
static ivm_TwecallActivate_t sg_TwecallActivate;
static DeviceProperty g_actionInput_TwecallActivate[] = {
   {.key = "ActivateStatus", .data_buff_len = 0, .data = &sg_TwecallActivate.action_in.m_ActivateStatus, .type = TYPE_TEMPLATE_ENUM},
};
static DeviceProperty g_actionOutput_TwecallActivate[] = {

};
static DeviceProperty g_actionInput_OnSwitchingSimCards[] = {

};
static DeviceProperty g_actionOutput_OnSwitchingSimCards[] = {

};

static DeviceAction g_actions[]={
    {
     .pActionId = "TextQuery",
     .timestamp = 0,
     .input_num = 2,
     .output_num = 2,
     .pInput = g_actionInput_TextQuery,
     .pOutput = g_actionOutput_TextQuery,
    },
    {
     .pActionId = "SetDefaultVideoColor",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_SetDefaultVideoColor,
     .pOutput = g_actionOutput_SetDefaultVideoColor,
    },
    {
     .pActionId = "OneKeyAlarm",
     .timestamp = 0,
     .input_num = 2,
     .output_num = 0,
     .pInput = g_actionInput_OneKeyAlarm,
     .pOutput = g_actionOutput_OneKeyAlarm,
    },
    {
     .pActionId = "ReplyCall",
     .timestamp = 0,
     .input_num = 2,
     .output_num = 0,
     .pInput = g_actionInput_ReplyCall,
     .pOutput = g_actionOutput_ReplyCall,
    },
    {
     .pActionId = "FormatStorageMedium",
     .timestamp = 0,
     .input_num = 0,
     .output_num = 0,
     .pInput = g_actionInput_FormatStorageMedium,
     .pOutput = g_actionOutput_FormatStorageMedium,
    },
    {
     .pActionId = "Reboot",
     .timestamp = 0,
     .input_num = 0,
     .output_num = 0,
     .pInput = g_actionInput_Reboot,
     .pOutput = g_actionOutput_Reboot,
    },
    {
     .pActionId = "StartPTZCruise",
     .timestamp = 0,
     .input_num = 2,
     .output_num = 0,
     .pInput = g_actionInput_StartPTZCruise,
     .pOutput = g_actionOutput_StartPTZCruise,
    },
    {
     .pActionId = "AddPTZCruise",
     .timestamp = 0,
     .input_num = 3,
     .output_num = 0,
     .pInput = g_actionInput_AddPTZCruise,
     .pOutput = g_actionOutput_AddPTZCruise,
    },
    {
     .pActionId = "DelPTZCruise",
     .timestamp = 0,
     .input_num = 2,
     .output_num = 0,
     .pInput = g_actionInput_DelPTZCruise,
     .pOutput = g_actionOutput_DelPTZCruise,
    },
    {
     .pActionId = "PreviewImageCapture",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_PreviewImageCapture,
     .pOutput = g_actionOutput_PreviewImageCapture,
    },
    {
     .pActionId = "ResetPTZ",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_ResetPTZ,
     .pOutput = g_actionOutput_ResetPTZ,
    },
    {
     .pActionId = "GotoPTZStation",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_GotoPTZStation,
     .pOutput = g_actionOutput_GotoPTZStation,
    },
    {
     .pActionId = "SetDefaultDevConfigs",
     .timestamp = 0,
     .input_num = 0,
     .output_num = 0,
     .pInput = g_actionInput_SetDefaultDevConfigs,
     .pOutput = g_actionOutput_SetDefaultDevConfigs,
    },
    {
     .pActionId = "ManualTimeCalibration",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_ManualTimeCalibration,
     .pOutput = g_actionOutput_ManualTimeCalibration,
    },
    {
     .pActionId = "CloudStorageStaus",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_CloudStorageStaus,
     .pOutput = g_actionOutput_CloudStorageStaus,
    },
    {
     .pActionId = "DeviceFileUpload",
     .timestamp = 0,
     .input_num = 2,
     .output_num = 0,
     .pInput = g_actionInput_DeviceFileUpload,
     .pOutput = g_actionOutput_DeviceFileUpload,
    },
    {
     .pActionId = "DeviceFileUploadStatus",
     .timestamp = 0,
     .input_num = 0,
     .output_num = 5,
     .pInput = g_actionInput_DeviceFileUploadStatus,
     .pOutput = g_actionOutput_DeviceFileUploadStatus,
    },
    {
     .pActionId = "BindOrDelete",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_BindOrDelete,
     .pOutput = g_actionOutput_BindOrDelete,
    },
    {
     .pActionId = "TwecallActivate",
     .timestamp = 0,
     .input_num = 1,
     .output_num = 0,
     .pInput = g_actionInput_TwecallActivate,
     .pOutput = g_actionOutput_TwecallActivate,
    },
    {
     .pActionId = "OnSwitchingSimCards",
     .timestamp = 0,
     .input_num = 0,
     .output_num = 0,
     .pInput = g_actionInput_OnSwitchingSimCards,
     .pOutput = g_actionOutput_OnSwitchingSimCards,
    },
};

#define EVENT_COUNTS     (2)

static DeviceProperty g_propertyEvent_OneKeyCall[] = {
   {.key = "EventType", .data = &g_ivm_objs.Event.m_OneKeyCall.m_EventType, .type = TYPE_TEMPLATE_ENUM},

};
static DeviceProperty g_propertyEvent_NoParamsNormalEvent[] = {
   {.key = "EventType", .data = &g_ivm_objs.Event.m_NoParamsNormalEvent.m_EventType, .type = TYPE_TEMPLATE_ENUM},

};

static sEvent g_events[]={
    {
     .event_name = "OneKeyCall",
     .type = "info",
     .timestamp = 0,
     .eventDataNum = 1,
     .pEventData = g_propertyEvent_OneKeyCall,
    },
    {
     .event_name = "NoParamsNormalEvent",
     .type = "info",
     .timestamp = 0,
     .eventDataNum = 1,
     .pEventData = g_propertyEvent_NoParamsNormalEvent,
    },
};

extern int ivm_init_Action(void *obj, int num, char *name, ivm_callback_Action cb);
extern int ivm_init_ProWritable(void *obj, int num, char *name, ivm_callback_ProWritable cb, char close_sync);
extern int ivm_init_ProReadonly(void *obj, int num, char *name);
extern int ivm_init_Event(void *obj, int num, char *name);

#define ivm_doi_init_ProReadonly(data) ivm_init_ProReadonly(sg_RO_DataTemplate, TOTAL_RO_PROPERTY_COUNT, #data)
#define ivm_doi_init_ProWritable(data, flag) ivm_init_ProWritable(sg_WR_DataTemplate, TOTAL_WR_PROPERTY_COUNT, #data, (ivm_callback_ProWritable)iv_usrcb_ProWritable_##data, flag)
#define ivm_doi_init_Action(data)  ivm_init_Action(g_actions, TOTAL_ACTION_COUNTS, #data, (ivm_callback_Action)iv_usrcb_Action_##data)
#define ivm_doi_init_Event(data) ivm_init_Event(g_events, EVENT_COUNTS, #data)


int ivm_env_init(void)
{
    memset((void *) &g_ivm_objs, 0, sizeof(ivm_extend_param_t));
    _ivm_init_data_template();

    ivm_doi_init_ProReadonly(TalkBackStartEnable);
    ivm_doi_init_ProReadonly(OneKeyCallState);
    ivm_doi_init_ProReadonly(DeviceInformation);
    ivm_doi_init_ProReadonly(ResolutionFunctionSet);
    ivm_doi_init_ProReadonly(StorageTotalCapacity);
    ivm_doi_init_ProReadonly(FunctionSet);
    ivm_doi_init_ProReadonly(SubFunctionSet);
    ivm_doi_init_ProReadonly(RebootStatus);
    ivm_doi_init_ProReadonly(PTZCruise);
    ivm_doi_init_ProReadonly(RemainConnNum);
    ivm_doi_init_ProReadonly(PTZStatus);
    ivm_doi_init_ProReadonly(BindStatusReport);
    ivm_doi_init_ProReadonly(PtzStep);
    ivm_doi_init_ProReadonly(Information4G);
    ivm_doi_init_ProReadonly(CutCardState);
    ivm_doi_init_ProWritable(record_enable, 0);
    ivm_doi_init_ProWritable(key_user_1, 0);
    ivm_doi_init_ProWritable(key_user_2, 0);
    ivm_doi_init_ProWritable(VideoColor, 0);
    ivm_doi_init_ProWritable(_set_attribute, 0);
    ivm_doi_init_ProWritable(VideoBitrate, 0);
    ivm_doi_init_ProWritable(DeviceInputVolume, 0);
    ivm_doi_init_ProWritable(DeviceOutputVolume, 0);
    ivm_doi_init_ProWritable(ChannelOSD, 0);
    ivm_doi_init_ProWritable(TDLogoOSD, 0);
    ivm_doi_init_ProWritable(PowerLineFrequency, 0);
    ivm_doi_init_ProWritable(VoiceIntercomMode, 0);
    ivm_doi_init_ProWritable(OneKeyCallSwitch, 0);
    ivm_doi_init_ProWritable(AudioEncoding, 0);
    ivm_doi_init_ProWritable(CruiseFrequencyLevel, 0);
    ivm_doi_init_ProWritable(CruiseTimePlan, 0);
    ivm_doi_init_ProWritable(PtzStation, 0);
    ivm_doi_init_ProWritable(PtzSpeed, 0);
    ivm_doi_init_ProWritable(OneClickMasking, 0);
    ivm_doi_init_ProWritable(VideoResolution, 0);
    ivm_doi_init_ProWritable(VideoBitRateControl, 0);
    ivm_doi_init_ProWritable(VideoFPS, 0);
    ivm_doi_init_ProWritable(DevBindStatus, 0);
    ivm_doi_init_ProWritable(TimedCruiseStatus, 0);
    ivm_doi_init_ProWritable(TimeZoneSetting, 0);
    ivm_doi_init_ProWritable(StatusLightSwitch, 0);
    ivm_doi_init_ProWritable(CloudStorageSwitch, 0);
    ivm_doi_init_ProWritable(ImageFlipState, 0);
    ivm_doi_init_ProWritable(StorageRecordMode, 0);
    ivm_doi_init_ProWritable(FormatStorageMediumProgress, 0);
    ivm_doi_init_ProWritable(StorageStatus, 0);
    ivm_doi_init_ProWritable(WifiConfiguration, 0);
    ivm_doi_init_ProWritable(GetWiFiInfo, 0);
    ivm_doi_init_ProWritable(AntiFlickerSwitch, 0);
    ivm_doi_init_ProWritable(BackLightShooting, 0);
    ivm_doi_init_ProWritable(RTMPInfo, 0);
    ivm_doi_init_ProWritable(DeciceResolution, 0);
    ivm_doi_init_ProWritable(DynamicInformation, 0);
    ivm_doi_init_ProWritable(DayNightMode, 0);
    ivm_doi_init_ProWritable(TimingReboot, 0);
    ivm_doi_init_ProWritable(NightVisionMode, 0);
    ivm_doi_init_ProWritable(WhiteLightBrightness, 0);
    ivm_doi_init_ProWritable(SnapshotStatus, 0);
    ivm_doi_init_ProWritable(TimingShield, 0);
    ivm_doi_init_ProWritable(Onvif, 0);
    ivm_doi_init_ProWritable(HomeGuardSwitch, 0);
    ivm_doi_init_ProWritable(HumanDetectConfig, 0);
    ivm_doi_init_ProWritable(SoundLightAlarmConfig, 0);
    ivm_doi_init_ProWritable(CarDetectConfig, 0);
    ivm_doi_init_ProWritable(BikeDetectConfig, 0);
    ivm_doi_init_ProWritable(PetDetectConfig, 0);
    ivm_doi_init_ProWritable(ImageDetectConfig, 0);
    ivm_doi_init_ProWritable(HideDetectConfig, 0);
    ivm_doi_init_ProWritable(CryDetectConfig, 0);
    ivm_doi_init_ProWritable(MainStreamBitrate, 0);
    ivm_doi_init_ProWritable(VideoEncoding, 0);
    ivm_doi_init_ProWritable(DeviceLogFileUpload, 0);
    ivm_doi_init_ProWritable(DeviceLogRedirect, 0);
    ivm_doi_init_ProWritable(ImageMode, 0);
    ivm_doi_init_ProWritable(RegionDetectConfig, 0);
    ivm_doi_init_ProWritable(OverLineDetectConfig, 0);

    ivm_doi_init_Action(TextQuery);
    ivm_doi_init_Action(SetDefaultVideoColor);
    ivm_doi_init_Action(OneKeyAlarm);
    ivm_doi_init_Action(ReplyCall);
    ivm_doi_init_Action(FormatStorageMedium);
    ivm_doi_init_Action(Reboot);
    ivm_doi_init_Action(StartPTZCruise);
    ivm_doi_init_Action(AddPTZCruise);
    ivm_doi_init_Action(DelPTZCruise);
    ivm_doi_init_Action(PreviewImageCapture);
    ivm_doi_init_Action(ResetPTZ);
    ivm_doi_init_Action(GotoPTZStation);
    ivm_doi_init_Action(SetDefaultDevConfigs);
    ivm_doi_init_Action(ManualTimeCalibration);
    ivm_doi_init_Action(CloudStorageStaus);
    ivm_doi_init_Action(DeviceFileUpload);
    ivm_doi_init_Action(DeviceFileUploadStatus);
    ivm_doi_init_Action(BindOrDelete);
    ivm_doi_init_Action(TwecallActivate);
    ivm_doi_init_Action(OnSwitchingSimCards);

    ivm_doi_init_Event(OneKeyCall);
    ivm_doi_init_Event(NoParamsNormalEvent);

    return 0;
}
