/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __AX_TX_COMMON_H__
#define __AX_TX_COMMON_H__

#include "ax_global_type.h"

#define VI_MAX_DATA_LINE    (16)

/*sub module*/
#define AX_ID_TX_VI  (0x01)
#define AX_ID_TX_VO  (0x02)
#define AX_ID_TX_CLK (0x03)

/* invlalid device ID */
#define AX_ERR_TX_VI_INVALID_DEVID     AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_INVALID_DEVID)
/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AX_ERR_TX_VI_ILLEGAL_PARAM     AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define AX_ERR_TX_VI_NULL_PTR          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_NULL_PTR)
/* try to enable or initialize system,device or channel, before configing attribute */
#define AX_ERR_TX_VI_NOT_CONFIG        AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_NOT_CONFIG)
/* operation is not supported by NOW */
#define AX_ERR_TX_VI_NOT_SUPPORT       AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_NOT_SUPPORT)
/* operation is not permitted ,eg, try to change static attribute */
#define AX_ERR_TX_VI_NOT_PERM          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_NOT_PERM)
/* failure caused by malloc memory */
#define AX_ERR_TX_VI_NOMEM             AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_NOMEM)
/* failure caused by request irq */
#define AX_ERR_TX_VI_REQUEST_IRQFAIL   AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VI, AX_ERR_REQ_IRQ_FAIL)

/*invlalid device ID*/
#define AX_ERR_TX_VO_INVALID_DEVID     AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_INVALID_DEVID)
/*invlalid channel ID*/
#define AX_ERR_TX_VO_INVALID_CHNID     AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_INVALID_CHNID)
/*at lease one parameter is illagal ,eg, an illegal enumeration value*/
#define AX_ERR_TX_VO_ILLEGAL_PARAM     AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_ILLEGAL_PARAM)
/*lib exists*/
#define AX_ERR_TX_VO_LIB_EXIST         AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_EXIST)
/*lib unexists*/
#define AX_ERR_TX_VO_LIB_UNEXIST       AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_UNEXIST)
/*using a NULL point*/
#define AX_ERR_TX_VO_NULL_PTR          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_NULL_PTR)
/*try to enable or initialize system,device or channel, before configing attribute*/
#define AX_ERR_TX_VO_NOT_CONFIG        AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_NOT_CONFIG)
/*operation is not supported by NOW*/
#define AX_ERR_TX_VO_NOT_SUPPORT       AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_NOT_SUPPORT)
/*operation is not permitted ,eg, try to change static attribute*/
#define AX_ERR_TX_VO_NOT_PERM          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_NOT_PERM)
/*failure caused by malloc memory*/
#define AX_ERR_TX_VO_NOMEM             AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_NOMEM)
/*failure caused by malloc buffer*/
#define AX_ERR_TX_VO_NOBUF             AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_NOBUF)
/*no data in buffer*/
#define AX_ERR_TX_VO_BUF_EMPTY         AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_BUF_EMPTY)
/*no buffer for new data*/
#define AX_ERR_TX_VO_BUF_FULL          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_BUF_FULL)
/*not initialize*/
#define AX_ERR_TX_VO_NOT_INIT          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_SYS_NOTREADY)

#define AX_ERR_TX_VO_REQUEST_IRQFAIL   AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_REQ_IRQ_FAIL)

#define AX_ERR_TX_VO_TIMEOUT           AX_DEF_ERR(AX_ID_TX, AX_ID_TX_VO, AX_ERR_TIMED_OUT)

/* at lease one parameter is illagal ,eg, an illegal enumeration value  */
#define AX_ERR_TX_CLK_ILLEGAL_PARAM     AX_DEF_ERR(AX_ID_TX, AX_ID_TX_CLK, AX_ERR_ILLEGAL_PARAM)
/* using a NULL point */
#define AX_ERR_TX_CLK_NULL_PTR          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_CLK, AX_ERR_NULL_PTR)

#define AX_ERR_TX_CLK_NOT_PERM          AX_DEF_ERR(AX_ID_TX, AX_ID_TX_CLK, AX_ERR_NOT_PERM)

/**************************************TX_VI********************************************************* */
typedef struct tagSize {
    AX_U32 u32Width;
    AX_U32 u32Height;
} SIZE_S;

/* The sequence for yuv */
typedef enum tagYuvSeq {
    YUV_SEQ_UYVY = 0,
    YUV_SEQ_VYUY = 1,
    YUV_SEQ_YUYV = 2,
    YUV_SEQ_YVYU = 3,

    YUV_SEQ_BUTT
} YUV_SEQ_E;

/* VI Working Mode Selection  */
typedef	enum vi_work_mode
{
    VI_WORK_MODE_NORMAL = 0,               /* Normal */
    VI_WORK_MODE_COLORBAR,                 /* Colorbar Test*/

    VI_WORK_MODE_BUTT
} VI_WORK_MODE_E;

typedef	enum tagViColorBarMode
{
    VI_COLORBAR_PURE = 0,
    VI_COLORBAR_100P,
    VI_COLORBAR_75P,
    VI_COLORBAR_BLANK,

    VI_COLORBAR_BUTT
} VI_COLORBAR_MODE_E;

typedef	enum tagViColorBarClkSel
{
    VI_COLORBAR_CLK_PLL = 0,
    VI_COLORBAR_CLK_BT,

    VI_COLORBAR_CLK_BUTT
} VI_COLORBAR_CLK_E;

/* Define color values for color bar solid screen YUV three components */
typedef struct tagViColorYuv
{
    AX_U16   u16Y;            
    AX_U16   u16U;          
    AX_U16   u16V;        
    AX_U16   u16Reserved;
} VI_COLOR_YUV_S;

typedef struct tagViWorkParam
{
    VI_WORK_MODE_E      enWorkMode;       /* Worke Mode */
    VI_COLORBAR_MODE_E  enColorbarMode;   /* Monochrome,100% Color Bars,75% Color Bars,Pure Black*/
    VI_COLORBAR_CLK_E   enColorbarClk;    /* Internal System Clock, External BT Clock*/
    VI_COLOR_YUV_S      stSingleColor;    /* Color Parameter Configuration in Monochrome Screen Test Mode */
} VI_WORK_PARAM_S;


/* interface tpye of video input */
typedef	enum tagViIntfType
{
    VI_INTF_TYPE_BT = 0,                  /* BT type */

    VI_INTF_TYPE_BUTT
} VI_INTF_TYPE_E;

/* BT Working Mode */
typedef enum tagViBtWorkMode
{
    VI_MODE_BT1120        = 0,            /* BT1120 Mode */
    VI_MODE_BT656_HD1     = 1,            /* BT655 Single Edge Single Sync Header Frame Mode，Single Header:FF 00 00 XY */
    VI_MODE_BT656_SD      = 2,            /* BT655 Single Edge Single Sync Header Field， only support 960H */
    VI_MODE_BT656_HD2     = 3,            /* BT655 Single Edge Dual Sync Header，Dual Header:FF FF 00 00 00 00 XY XY */
    VI_MODE_BT656_DUL_HD1 = 4,            /* BT655 Dual Edge Single Sync Header，Single Header:FF 00 00 XY */
    VI_MODE_BT656_DUL_HD2 = 5,            /* BT655 Dual Edge Dual Sync Header，Dual Header:FF FF 00 00 00 00 XY XY */
    VI_MODE_BT_BUTT,
} VI_BT_WORK_MODE_E;

/* BT Sampling Mode */
typedef enum tagViBtSampMode
{
    VI_BT_SAMP_RISING    = 0,             /* Rising Edge Priority Sampling */
    VI_BT_SAMP_FALLING   = 1,             /* Falling Edge Priority Sampling */

    VI_BT_SAMP_BUTT,
} VI_BT_SAMP_MODE_E;

typedef struct tagViDevAttr {
    VI_INTF_TYPE_E      enIntfType;       /* Input Type */
    VI_BT_WORK_MODE_E   enBtWorkMode;     /* Distinguish BT1120 and BT656, and Single Edge vs Dual Edge Sampling*/
    VI_BT_SAMP_MODE_E   enBtSampMode;     /* BT Sampling Order */   
    YUV_SEQ_E           enYuvSeq;         /* YUV Format Arrangement Order, BT Data Output Format */
    SIZE_S              stImageSize;      /* Effective width and height mainly for color bar output */
    SIZE_S              stDestSize;       /* Total width and height primarily for color bar output image, including blanking area */
} VI_DEV_ATTR_S;

typedef	enum tagViBtFps
{
    VI_BT_12_5FPS = 0,
    VI_BT_15FPS,
    VI_BT_20FPS,
    VI_BT_25FPS,
    VI_BT_30FPS,
    VI_BT_50FPS,
    VI_BT_59_94FPS,
    VI_BT_60FPS,
    VI_BT_INVALID_FPS,

    VI_BT_FPS_BUTT
} VI_BT_FPS_E;

typedef	struct tagViBtStatus
{
    AX_BOOL         bEnable;             /* Module Enabled State */
    AX_BOOL         bField;              /* Field Mode */
    AX_U32          u32DestWidth;        /* Total width of image */
    AX_U32          u32DestHeight;       /* Total height of image */
    AX_U32          u32ValueWidth;       /* Image effective width */
    AX_U32          u32ValueHeight;      /* Image effective height */
    AX_U32          u32Fps;              /* Image frame rate x100,Divide by 100 for actual use */
    AX_U32          u32DataInCnt;        /* Data input calculation */
    AX_U32          u32DataOutCnt;       /* Data output count */
    AX_U32          u32FifoStatus;       /* Reception status */
    AX_U32          u32ErrStatus;        /* Error status */
} VI_BT_STATUS_S;


/********************************************TX_VO********************************************************** */
typedef enum  video_fmt
{
    TX_VIDEO_FORMAT_720P_25FPS         = 0,
    TX_VIDEO_FORMAT_720P_30FPS         = 1,
    TX_VIDEO_FORMAT_720P_50FPS         = 2,
    TX_VIDEO_FORMAT_720P_54FPS         = 3,
    TX_VIDEO_FORMAT_720P_59FPS         = 4,
    TX_VIDEO_FORMAT_720P_60FPS         = 5,
    TX_VIDEO_FORMAT_1080P_12_5FPS,
    TX_VIDEO_FORMAT_1080P_15FPS,
    TX_VIDEO_FORMAT_1080P_25FPS,
    TX_VIDEO_FORMAT_1080P_30FPS,
    TX_VIDEO_FORMAT_1080P_50FPS,
    TX_VIDEO_FORMAT_1080P_60FPS,
    TX_VIDEO_FORMAT_480P_30FPS,
    TX_VIDEO_FORMAT_480P_60FPS,
    TX_VIDEO_FORMAT_576P_25FPS,
    TX_VIDEO_FORMAT_576P_50FPS,
    TX_VIDEO_FORMAT_960P_15FPS,
    TX_VIDEO_FORMAT_960P_25FPS,
    TX_VIDEO_FORMAT_960P_30FPS,
    TX_VIDEO_FORMAT_960P_50FPS,
    TX_VIDEO_FORMAT_2048x1536_25FPS,
    TX_VIDEO_FORMAT_2048x1536_30FPS,
    TX_VIDEO_FORMAT_2560x1440_12_5FPS,
    TX_VIDEO_FORMAT_2560x1440_15FPS,
    TX_VIDEO_FORMAT_2560x1440_20FPS,
    TX_VIDEO_FORMAT_2560x1440_25FPS,
    TX_VIDEO_FORMAT_2560x1440_30FPS,
    TX_VIDEO_FORMAT_3MP_20FPS,
    TX_VIDEO_FORMAT_3MP_25FPS,
    TX_VIDEO_FORMAT_3MP_30FPS,
    TX_VIDEO_FORMAT_4K_25FPS,
    TX_VIDEO_FORMAT_4K_30FPS,
    TX_VIDEO_FORMAT_640x364_50FPS,
    TX_VIDEO_FORMAT_640x356_60FPS,
    TX_VIDEO_FORMAT_960H_25FPS,
    TX_VIDEO_FORMAT_960H_30FPS,
    TX_VIDEO_FORMAT_960H_50FPS,
    TX_VIDEO_FORMAT_960H_60FPS,

    TX_VIDEO_FORMAT_2304x1296_20FPS,
    TX_VIDEO_FORMAT_2304x1296_25FPS,
    TX_VIDEO_FORMAT_2304x1296_30FPS,
    TX_VIDEO_FORMAT_2496x1404_20FPS,
    TX_VIDEO_FORMAT_2688x1520_25FPS,
    TX_VIDEO_FORMAT_2688x1520_30FPS,
    TX_VIDEO_FORMAT_2688x1944_30FPS,
    TX_VIDEO_FORMAT_2560x1944_12_5FPS,
    TX_VIDEO_FORMAT_2560x1944_20FPS,
    TX_VIDEO_FORMAT_2592x1944_10FPS,
    TX_VIDEO_FORMAT_2592x1944_12_5FPS,
    TX_VIDEO_FORMAT_2592x1944_20FPS,

    TX_VIDEO_FORMAT_2880x1620_10FPS,
    TX_VIDEO_FORMAT_2880x1620_12_5FPS,
    TX_VIDEO_FORMAT_2880x1620_15FPS,
    TX_VIDEO_FORMAT_2880x1620_20FPS,
    TX_VIDEO_FORMAT_2880x1620_25FPS,

	TX_VIDEO_FORMAT_2960x1664_20FPS,

    TX_VIDEO_FORMAT_3072x1728_20FPS,
    TX_VIDEO_FORMAT_3072x1728_25FPS,
    TX_VIDEO_FORMAT_3072x2048_18FPS,
    TX_VIDEO_FORMAT_3840x2160_12_5FPS,
    TX_VIDEO_FORMAT_3840x2160_15FPS,
    TX_VIDEO_FORMAT_3840x2160_25FPS,
    TX_VIDEO_FORMAT_3840x2160_30FPS,

    TX_VIDEO_FORMAT_BUTT
} VIDEO_FMT_E;

/*Video output standard*/
typedef	enum video_out_fmt
{
    /*HDCVI standard*/
    /*720P*/
    HDCVI_1280x720_25FPS,
    HDCVI_1280x720_30FPS,
    HDCVI_1280x720_50FPS,
    HDCVI_1280x720_60FPS,
    /*1080P*/
    HDCVI_1920x1080_25FPS,
    HDCVI_1920x1080_30FPS,
    HDCVI_1920x1080_12_5FPS,
    HDCVI_1920x1080_15FPS,
    HDCVI_1920x1080_50FPS,
    HDCVI_1920x1080_60FPS,
    /*3M 20f*/
    HDCVI_2304x1296_20FPS,
    /*4M*/
    HDCVI_2560x1440_25FPS,
    HDCVI_2560x1440_30FPS,
    HDCVI_2560x1440_12_5FPS,
    HDCVI_2560x1440_15FPS,
    /*5M*/
    HDCVI_3072x1728_20FPS,
    HDCVI_3072x1728_25FPS,
    HDCVI_2592x1944_20FPS,
    HDCVI_2880x1620_20FPS,
    HDCVI_2880x1620_20FPS_V20,
    HDCVI_2880x1620_25FPS,
    HDCVI_2880x1620_25FPS_V20,
    HDCVI_2880x1620_10FPS,
    HDCVI_2880x1620_10FPS_V20,
    HDCVI_2880x1620_12_5FPS,
    HDCVI_2880x1620_12_5FPS_V20,
    /*4K*/
    HDCVI_3840x2160_12_5FPS,
    HDCVI_3840x2160_15FPS,
    /*2路*/
    HDCVI_2560x720_25FPS,
    HDCVI_2560x720_30FPS,
    HDCVI_3840x1080_25FPS,
    HDCVI_3840x1080_25FPS_V20,
    HDCVI_3840x1080_30FPS,
    HDCVI_3840x1080_30FPS_V20,
    HDCVI_5120x1440_12_5FPS,
    HDCVI_5120x1440_15FPS,
    HDCVI_5760x1620_12_5FPS,
    HDCVI_5760x1620_10FPS,
    HDCVI_5184x1944_10FPS,

    /*AHD standard*/
    /*720P*/
    A_1280x720_25FPS,
    A_1280x720_30FPS,
    /*1080P*/
    A_1920x1080_25FPS,
    A_1920x1080_30FPS,
    /*4M*/
    A_2560x1440_25FPS,
    A_2560x1440_30FPS,
    A_2560x1440_15FPS,
    /*5M*/
    A_2592x1944_12_5FPS,
    A_2592x1944_20FPS,
    /*4K*/
    A_3840x2160_15FPS,

    /*TVI standard*/
    /*720P*/
    T_1280x720_25FPS,
    T_1280x720_30FPS,
    T_1280x720_25FPS_V30,
    T_1280x720_30FPS_V30,
    /*1080P*/
    T_1920x1080_25FPS,
    T_1920x1080_30FPS,
    T_1920x1080_15FPS,
    /*4M*/
    T_2560x1440_25FPS,
    T_2560x1440_30FPS,
    T_2560x1440_15FPS,
    /*5M*/
    T_2592x1944_12_5FPS,
    T_2592x1944_20FPS,
    T_2560x1944_12_5FPS,
    T_2560x1944_20FPS,
    T_2960x1664_20FPS,
    /*4K*/
    T_3840x2160_12_5FPS,
    T_3840x2160_15FPS,

    /*SD standard*/
    H960_FMT_PAL,
    H960_FMT_NTSC,
    H940_FMT_PAL,
    H940_FMT_NTSC,

    /*SD standard in VPS output field mode*/
    H960_FMT_PAL_FIELD,
    H960_FMT_NTSC_FIELD,
    H940_FMT_PAL_FIELD,
    H940_FMT_NTSC_FIELD,

    /*BTOUT Output Format*/
    BT_1280x720_25FPS,
    BT_1280x720_30FPS,
    /*1080P*/
    BT_1920x1080_25FPS,
    BT_1920x1080_30FPS,
    /*4M*/
    BT_2560x1440_12_5FPS,
    BT_2560x1440_15FPS,
    BT_2560x1440_25FPS,
    BT_2560x1440_30FPS,
    /*5M*/
    BT_2880x1620_10FPS,
    BT_2880x1620_12_5FPS,
    BT_2880x1620_20FPS,
    BT_2880x1620_25FPS,
    BT_2592x1944_10FPS,
    BT_2592x1944_20FPS,

    VIDEO_OUT_FMT_BUTT,
} VIDEO_OUT_FMT_E;


/*COLORBAR Mode*/
typedef	enum vo_colorbar_mode
{
    VO_COLORBAR_NONE = 0,               /*Disable color bar mode*/

    /*Configurable modes for HD color bars*/
    VO_COLORBAR_100,                    /*100% color bar mode*/
    VO_COLORBAR_75,                     /*75% color bar mode*/
    VO_COLORBAR_ONE_COLOR,              /*Monochrome screen mode*/
    VO_COLORBAR_WHITE_SHADE_INTO_BLACK, /*White-to-black gradient stripe mode*/

    /*Configurable modes for SD color bars*/
    VO_COLORBAR_SD,
    VO_COLORBAR_GRAY,
    VO_COLORBAR_BURST,
    VO_COLORBAR_AMPLITUDE_FREQ,

    VO_COLORBAR_BUTT
} VO_COLORBAR_MODE_E;

/*RS485 Parameters*/
typedef	struct tagVoRs485Param
{
    AX_U8   u8SendEn;           /*Forward Transmission Enable*/
    AX_U8   u8SendBitInvEn;     /*Forward Transmission Byte Data Bit Reversal Enable Switch, Default is Off*/
    AX_U8   u8RecvEn;           /*Receive enable*/
    AX_U8   u8RecvIntEn;        /*Receive Interrupt Enable*/
    AX_U8   u8RecvTimeoutIntEn; /*Receive Timeout Interrupt Enable*/
    AX_U8   u8RecvCheckEn;      /*Receive Parity Check Enable*/
    /* TVI Reverse 485 Data Header Search Enable，bit0 indicates data header 0(0x02)，bit1 indicates data header 0(0x03)，bit2 indicates data header 1，bit7 indicates data header 0(0xff) */
    /*
    | Data Header | Enable Status | Description Value | Reception Status |
    |-------------|---------------|-------------------|------------------|
    | Header 0 (0x02) | Disabled | - | Cannot receive |
    | Header 0 (0x03) | Disabled | - | Cannot receive |
    | Header 1 | Enabled | 0xb5 | Normal reception |
    | Header 0 (0xff) | Disabled | - | Cannot receive | 
    Configuration values 0x01-0x07 correspond to different data header reception modes:
    - 0x01: Only receive data header 0x02
    - 0x02: Only receive data header 0x03
    - 0x03: Receive either data header 0x02 or 0x03
    - 0x04: Reserved
    - 0x05: Only receive data header 0x02 with value 0xb5
    - 0x06: Only receive data header 0x03 with value 0xb5
    - 0x07: Receive either data header 0x02 0xb5 or 0x03 0xb5
    Configuration values 0x81-0x87 extend 0x01-0x07 with additional 0xff reception capability:
    - 0x81: Receive data header 0x02 or 0xff
    - 0x82: Receive data header 0x03 or 0xff
    - 0x83: Receive data header 0x02, 0x03 or 0xff
    - 0x85: Receive data header 0x02 (value 0xb5) or 0xff
    - 0x86: Receive data header 0x03 (value 0xb5) or 0xff
    - 0x87: Receive data header 0x02 0xb5, 0x03 0xb5 or 0xff
     */
    AX_U8   u8RecvTviHeadEn;
    AX_U8   u8RecvTviHead1;     /*TVI Reverse 485 Data Header 1 Content Setting, Default is 0xb5*/
    AX_U32  u32RecvIntCnt;      /*Number of receive interrupt triggers*/
    AX_U32  u32RecvTimeout;     /*Receive Timeout Interrupt Time Unit Ms*/
} VO_RS485_PARAM_S;

/*485 Send and Receive Data Share This Structure*/
typedef	struct tagVoRs485Data
{
    AX_U8   *pu8DataBuf;        /*Data address*/
    AX_U32  u32Len;             /*data length*/
    AX_U32  u32Timeout;         /*Timeout time unit Ms*/
} VO_RS485_DATA_S;

/*485 Position Mode Selection, General Position or Reduced Row Count Position*/
typedef	enum tagVoRs485Position
{
    VO_RS485_POSITION_NORMAL = 0,
    VO_RS485_POSITION_REDUCE,

    VO_RS485_POSITION_BUTT
} VO_RS485_POSITION_E;

/*Audio Mode Selection, Old Analog Audio or New Digital Audio*/
typedef	enum vo_audio_type
{
    VO_AUDIO_TYPE_ANALOG = 0,
    VO_AUDIO_TYPE_DIGTAL,

    VO_AUDIO_TYPE_BUTT
} VO_AUDIO_TYPE_E;

/*Channel Selection*/
typedef	enum vo_audio_chn
{
    VO_AUDIO_CHN_LEFT = 0,          /* Left audio channel */
    VO_AUDIO_CHN_RIGHT,             /* Right audio channel */

    VO_AUDIO_CHN_BUTT
} VO_AUDIO_CHN_E;

/*Audio Parameters*/
typedef	struct tagVoAudioParam
{
    VO_AUDIO_TYPE_E enAudioType;    /*Audio Type*/
    VO_AUDIO_CHN_E  enAudioChn;     /*Left and right channels*/
    AX_U8           u8AudioEn;      /*Audio enable switch，enable audio acquisition work related to I2S*/
    AX_U8           u8AudioDebugEn; /*Audio debug enable*/
    AX_U16          u16DebugData;   /*debug data*/
} VO_AUDIO_PARAM_S;

/*Audio extension parameters,Reverse audio related*/
typedef	struct tagVoAudioExtParam
{
    AX_U8   u8TransMode;            /*Reverse audio transmission mode，0:Fuhan，1:TP*/
    AX_U8   u8DataWidth;            /*Reverse audio data precision selection，8bit - 16bit*/
    AX_U8   u8AudiobackwardEn;      /*Reverse audio enable*/
    AX_U8   u8EqEn;                 /*EQ enable*/
    AX_U8   u8EqCdata;              /*Capacitor configuration, default 0xf*/
    AX_U8   u8EqRdata;              /*Resistor configuration, default 0x5*/
} VO_AUDIO_EXT_PARAM_S;

/*VO DISP parameters */
typedef	struct tagVoDispParam
{
    AX_BOOL bDispCapThrEn;                      /*Disp Cap Thr enable*/
    AX_U32  u32DispCapVal;                      /*Disp Cap Thr level  */
} VO_DISP_PARAM_S;

/*VO video parameters*/
typedef	struct tagVoVideoParam
{
    VIDEO_FMT_E         enInFmt;                /*Sensor input video standard*/
    VIDEO_OUT_FMT_E     enOutFmt;               /*Output video standard*/
    VO_COLORBAR_MODE_E  enColorBar;             /*Color bar parameters*/
    VO_DISP_PARAM_S     stDispParam;            /*Manually configure VO startup point,Can be disabled by default,Use driver auto-adaptive configuration*/
} VO_VIDEO_PARAM_S;

/*video extension parameters*/
typedef	struct tagVoVideoExtParam
{
    AX_U8   u8UGain;                            /*HDCVI U gain，unsigned，step gain_v/128*/
    AX_U8   u8VGain;                            /*HDCVI V gain，unsigned，step gain_u/128*/
    AX_U8   u8YBlankGain;                       /*HDCVI BLANK gain，step gain_y_blnk/128*/
    AX_U8   u8YActiveGain;                      /*HDCVI Y ACTIVE gain，step gain_y_actv/128*/
    AX_U8   u8ColorBurstEn;                     /*Chroma carrier sync switch*/
    AX_U8   u8YModeEn;                          /*Black and white mode switch*/
    AX_U8   u8PnSeqEn;                          /*PN sequence enable*/
    /*in CVI, TVI, AHD modes, upper 4 bits control Y data channel delay, range [0-7], lower 4 bits control CbCr data channel delay, range [0-15]*/
    /*in CVBS mode, Y channel delay relative to C channel, range [0-255]*/
    AX_U8   u8YcDelay;
    AX_U32  u32PhsInc;                          /*chroma carrier frequency*/
} VO_VIDEO_EXT_PARAM_S;

typedef struct tagVoSignalParam
{
    AX_U8   u8DacLevel;                         /*DAC output overall amplitude value, value range [0-15]*/
    AX_U8   u8SyncLevel;                        /*sync signal level*/
    AX_U8   u8BurstLevel;                       /*chroma carrier signal amplitude*/
    AX_U8   u8BlankLevel;                       /*blank level*/
    AX_U8   u8DacScale;                         /*coaxial DAC ratio, value range [0-3]*/
    AX_U8   au8Reserved[3];                     /*reserved*/
} VO_SIGNAL_PARAM_S;

typedef	enum vo_buffer_sel_compref
{
    VO_BUFFER_SEL_COMPREF_EXT_R = 0,            /* External resistor voltage divider */
    VO_BUFFER_SEL_COMPREF_INT_DAC_R,            /* Internal DAC resistor voltage divider */

    VO_BUFFER_SEL_COMPREF_BUTT
} VO_BUFFER_SEL_COMPREF_E;

/*Audio extension parameters,Reverse audio related*/
typedef	struct tagVoBufferConfigParam
{
    VO_BUFFER_SEL_COMPREF_E enBufferSelCompref; /*Buffer Comp reference voltage selection*/
    AX_U8   u8BufferAcEn;                       /*Buffer AC enable*/
    AX_U8   u8BufferDacValue;                   /*Buffer Comp reference voltage value*/
    AX_U8   au8Reserved[2];                     /*reserved*/
} VO_BUFFER_CONFIG_PARAM_S;

/**************************************TX_CLK********************************************************* */
typedef enum pll_ref_clk_sel
{
	CLK_PLL_REF_BT = 0,
	CLK_PLL_REF_IO,

	CLK_PLL_REF_BUTT,
} PLL_REF_CLK_SEL_E;

typedef enum i2c_bus_clk_sel
{
	CLK_I2C_BUS_PLL = 0,
	CLK_I2C_BUS_IO,

	CLK_I2C_BUS_BUTT,
} I2C_BUS_CLK_SEL_E;

typedef	enum disp_tx_clk_sel
{
	CLK_DISP_TX_960H = 0,
	CLK_DISP_TX_CVI,

	CLK_DISP_TX_BUTT,
} DISP_TX_CLK_SEL_E;

typedef	enum utc_clk_sel
{
    CLK_UTC_CVI = 0,
    CLK_UTC_960H,

    CLK_UTC_BUTT,
} UTC_CLK_SEL_E;

typedef	enum dac_clk_sel
{
	CLK_DAC_960H = 0,
	CLK_DAC_CVI,

	CLK_DAC_BUTT,
} DAC_CLK_SEL_E;

typedef enum bt_in_clk_phase
{
	CLK_BT_IN_PHASE_NORMAL = 0,
	CLK_BT_IN_PHASE_INV,
    CLK_BT_IN_PHASE_DLY,
    CLK_BT_IN_PHASE_INV_DLY,

	CLK_BT_IN_PHASE_BUTT,
} BT_IN_CLK_PHASE_E;

typedef enum bt_in_clk_freq
{
    CLK_BT_IN_36M = 0,
    CLK_BT_IN_74P25M,
	CLK_BT_IN_148P5M,
	CLK_BT_IN_BUTT,
} BT_IN_CLK_FREQ_E;

typedef enum vi_clk_sel
{
	CLK_VI_BT = 0,
	CLK_VI_PLL,

	CLK_VI_BUTT,
} VI_CLK_SEL_E;

typedef enum aud_mclk_freq
{
    AUD_MCLK_2048 = 0,
    AUD_MCLK_4096,
	AUD_MCLK_8192,
	AUD_MCLK_12288,
	AUD_MCLK_12000,

    AUD_MCLK_BUTT,
} AUD_MCLK_FREQ_E;

typedef	struct tagClkSrcSel
{
	PLL_REF_CLK_SEL_E			enPllRefClk;
	I2C_BUS_CLK_SEL_E 			enI2cBusClk;
    VI_CLK_SEL_E                enViClk;
	DISP_TX_CLK_SEL_E           enDispTxClk;
	UTC_CLK_SEL_E               enUtcClk;
	DAC_CLK_SEL_E               enDacClk;
} CLK_SRC_SEL_S;

typedef	struct tagClkSrcEn
{
	AX_U32 u32BtClkEn:1;
	AX_U32 u32DispRxClkEn:1;
	AX_U32 u32DispTxClkEn:1;
    AX_U32 u32CviClkEn:1;
	AX_U32 u32H960ClkEn:1;
	AX_U32 u32AudClkEn:1;
    AX_U32 u32Reserved:26;
} CLK_SRC_EN_S;

typedef struct tagBtInClk
{
    BT_IN_CLK_FREQ_E    enBtClkFreq;
    BT_IN_CLK_PHASE_E	enBtClkPhs;
    AX_U32              u32BtClkDly;
} BT_IN_CLK_S;

typedef struct tagClkStatus
{
    AX_U32 u32RegClk;
    AX_U32 u32BtInClk;
    AX_U32 u32DispRxClk;
    AX_U32 u32DispTxClk;
} CLK_STATUS_S;

typedef enum tagAudioMclkFreq {
    AUDIO_MCLK_2048 = 0,                       /*MCLK = 2.048M*/
    AUDIO_MCLK_4096,                           /*MCLK = 4.096M*/
    AUDIO_MCLK_8192,
    AUDIO_MCLK_12288,
    AUDIO_MCLK_12000,

    AUDIO_MCLK_FREQ_BUTT
} AUDIO_MCLK_FREQ_E;

typedef enum tagAudioMode {
    AUDIO_MODE_I2S_MASTER = 0,                 /* I2S master mode */
    AUDIO_MODE_I2S_SLAVE,                      /* I2S slave mode */

    AUDIO_MODE_BUTT
} AUDIO_MODE_E;

typedef enum tagAudioSampleRate {
    AUDIO_SAMPLE_RATE_8K = 0,                  /* 8kHz sampling rate */
    AUDIO_SAMPLE_RATE_16K,                     /* 16kHz sampling rate */
    AUDIO_SAMPLE_RATE_32K,                     /* 32kHz sampling rate */

    AUDIO_SAMPLE_RATE_BUTT
} AUDIO_SAMPLE_RATE_E;

typedef enum tagAudioBitWidth {
    AUDIO_BIT_WIDTH_16 = 0,                    /* 16bit width */
    AUDIO_BIT_WIDTH_24,                        /* 24bit width */
    AUDIO_BIT_WIDTH_32,                        /* 32bit width */
    AUDIO_BIT_WIDTH_8,                         /* 8bit width */

    AUDIO_BIT_WIDTH_BUTT
} AUDIO_BIT_WIDTH_E;

typedef enum tagAudioOutputMode {
    AUDIO_OUTPUT_MODE_I2S = 0,                 /* 0：I2S */
    AUDIO_OUTPUT_MODE_PCM = 1,                 /* 1：DSP(PCM) */

    AUDIO_OUTPUT_MODE_BUTT
} AUDIO_OUTPUT_MODE_E;

typedef enum tagAudioOutputTimeEdge {
    AUDIO_OUTPUT_RISE_EDGE    = 0,             /*0：Rising edge*/
    AUDIO_OUTPUT_DESCEND_EDGE = 1,             /*1：Falling edge*/

    AUDIO_OUTPUT_EDGE_BUTT
} AUDIO_OUTPUT_TIME_EDGE_E;

typedef enum tagAudioPcmMode {
    AUDIO_PCM_STANDARD = 0,                    /*0：Standard format*/
    AUDIO_PCM_CUSTOM   = 1,                    /*1：Custom format*/

    AUDIO_PCM_BUTT
} AUDIO_PCM_MODE_E;

typedef struct tagAudioPubAttr {
    AUDIO_MCLK_FREQ_E        enMclkFreq;       /*MCLK clock configuration*/
    AUDIO_MODE_E             enAudioMod;       /*Master-slave mode configuration*/
    AUDIO_SAMPLE_RATE_E      enSampleRate;     /*Audio sampling rate*/
    AUDIO_BIT_WIDTH_E        enBitWidth;       /*Audio sampling bit width*/
    AUDIO_OUTPUT_MODE_E      enOutputMode;     /*Audio output interface data transmission method*/
    AUDIO_OUTPUT_TIME_EDGE_E enOutputTimeEdge; /*Clock edge used for audio output interface data transmission*/
    AUDIO_PCM_MODE_E         enPcmMode;        /*PCM output format selection*/
} VO_AUDIO_PUB_ATTR_S;

#endif /* __AX_TX_COMMON_H__ */
