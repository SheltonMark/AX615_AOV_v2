/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _AX_IVPS_TYPE_H_
#define _AX_IVPS_TYPE_H_
#include "ax_global_type.h"
#include "ax_pool_type.h"
#include "ax_ivps_comm.h"

#define AX_IVPS_MIN_IMAGE_WIDTH 8
#define AX_IVPS_MAX_IMAGE_WIDTH 8192

#define AX_IVPS_MIN_IMAGE_HEIGHT 2
#define AX_IVPS_MAX_IMAGE_HEIGHT 8192

#define AX_IVPS_MAX_GRP_NUM 16
#define AX_IVPS_MAX_OUTCHN_NUM 6
#define AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN 2
#define AX_IVPS_INVALID_FRMRATE (-1)

#define AX_IVPS_MAX_RGN_HANDLE_NUM (64)
#define AX_IVPS_INVALID_REGION_HANDLE (-1)
#define AX_IVPS_REGION_MAX_DISP_NUM (32)

#define AX_IVPS_MAX_POLYGON_POINT_NUM  10
#define AX_IVPS_MIN_POLYGON_POINT_NUM  3
#define AX_IVPS_MIN_LINE_WIDTH 1
#define AX_IVPS_MAX_LINE_WIDTH 16
#define AX_IVPS_VPP_MAX_CASCADE_NUM 4

typedef AX_S32 IVPS_GRP;
typedef AX_S32 IVPS_CHN;
typedef AX_S32 IVPS_FILTER;
typedef AX_S32 IVPS_FILTER_ID;
typedef AX_S32 IVPS_RGN_GRP;
typedef AX_U32 IVPS_RGB;
typedef AX_S32 IVPS_RGN_HANDLE;

typedef enum
{
    AX_IVPS_CHN_FLIP_NONE = 0,
    AX_IVPS_CHN_FLIP,
    AX_IVPS_CHN_MIRROR,
    AX_IVPS_CHN_FLIP_AND_MIRROR,
    AX_IVPS_CHN_FLIP_BUTT
} AX_IVPS_CHN_FLIP_MODE_E;

typedef struct
{
    AX_S16 nX;
    AX_S16 nY;
    AX_U16 nW;
    AX_U16 nH;
} AX_IVPS_RECT_T;

typedef struct
{
    AX_S16 nX;
    AX_S16 nY;
} AX_IVPS_POINT_T;

typedef struct
{
    AX_U16 nW;
    AX_U16 nH;
} AX_IVPS_SIZE_T;

typedef struct
{
    AX_S32 nHorRatio; /* RW; range: [-100000, 100000] */
    AX_S32 nVerRatio; /* RW; range: [-100000, 100000] */
} AX_IVPS_POINT_RATIO_T;

typedef struct
{
    AX_F32 fX;
    AX_F32 fY;
} AX_IVPS_POINT_NICE_T;

/* mosiac block size */
typedef enum
{
    AX_IVPS_MOSAIC_BLK_SIZE_2 = 0, /* block size 2*2 of MOSAIC */
    AX_IVPS_MOSAIC_BLK_SIZE_4,     /* block size 4*4 of MOSAIC */
    AX_IVPS_MOSAIC_BLK_SIZE_8,     /* block size 8*8 of MOSAIC */
    AX_IVPS_MOSAIC_BLK_SIZE_16,    /* block size 16*16 of MOSAIC */
    AX_IVPS_MOSAIC_BLK_SIZE_32,    /* block size 32*32 of MOSAIC */
    AX_IVPS_MOSAIC_BLK_SIZE_64,    /* block size 64*64 of MOSAIC */
    AX_IVPS_MOSAIC_BLK_SIZE_BUTT
} AX_IVPS_MOSAIC_BLK_SIZE_E;

typedef enum
{
    AX_IVPS_ROTATION_0   = 0,
    AX_IVPS_ROTATION_90  = 1,
    AX_IVPS_ROTATION_180 = 2,
    AX_IVPS_ROTATION_270 = 3,
    AX_IVPS_ROTATION_BUTT
} AX_IVPS_ROTATION_E;

typedef enum
{
    AX_IVPS_ASPECT_RATIO_STRETCH = 0, /* Fill buffer according to output image size */
    AX_IVPS_ASPECT_RATIO_AUTO = 1,    /* Fill buffer according to horizontal and vertical alignment specified by eAligns[] params */
    AX_IVPS_ASPECT_RATIO_MANUAL = 2,
    AX_IVPS_ASPECT_RATIO_BUTT
} AX_IVPS_ASPECT_RATIO_E;

typedef enum
{
/*
if (src_w / src_h < dst_w / dst_h)
    w_ratio = h_ratio = dst_h * 1.0 / src_h
else
    w_ratio = h_ratio = dst_w * 1.0 / src_w

Example1:
    src: 1920x1080  dst: 1920x1140
    IVPS_ASPECT_RATIO_HORIZONTAL_CENTER
    IVPS_ASPECT_RATIO_VERTICAL_BOTTOM
    then output image:
    |   fill area (0, 0, 1920, 60)      |  0
    |-----------------------------------| 60
    |                                   |
    |   Image area(0, 60, 1920, 1140)   |
    |                                   |
    |                                   | 1140

Example2:
    src: 1920x1080  dst: 1980x1080
    IVPS_ASPECT_RATIO_HORIZONTAL_LEFT
    IVPS_ASPECT_RATIO_VERTICAL_CENTER
    then output image:
    0                            1920   1980
    |                              |      |
    |      Image Area              | Fill |
    |      (0, 0, 1920, 1080)      | Area |
    |                              |      |
*/
    AX_IVPS_ASPECT_RATIO_HORIZONTAL_CENTER = 0,
    AX_IVPS_ASPECT_RATIO_HORIZONTAL_LEFT = 1,
    AX_IVPS_ASPECT_RATIO_HORIZONTAL_RIGHT = 2,
    AX_IVPS_ASPECT_RATIO_VERTICAL_CENTER = AX_IVPS_ASPECT_RATIO_HORIZONTAL_CENTER,
    AX_IVPS_ASPECT_RATIO_VERTICAL_TOP = AX_IVPS_ASPECT_RATIO_HORIZONTAL_LEFT,
    AX_IVPS_ASPECT_RATIO_VERTICAL_BOTTOM = AX_IVPS_ASPECT_RATIO_HORIZONTAL_RIGHT,
} AX_IVPS_ASPECT_RATIO_ALIGN_E;

typedef enum
{
    AX_IVPS_SCL_TYPE_AUTO = 0,     /* Select the appropriate scaling algorithm based on current hardware support */
    AX_IVPS_SCL_TYPE_BILINEAR,     /* Bilinear interpolation scaling algorithm, commonly used */
    AX_IVPS_SCL_TYPE_PHASE_FILTER, /* Phase filtering scaling algorithm, better than Bilinear */
    AX_IVPS_SCL_TYPE_BUTT
} AX_IVPS_SCL_TYPE_E;

typedef enum
{
    AX_IVPS_SCL_INPUT_MONOPOLY = 0,  /* Monopolize a scaler. If other users also choose this type, you will share this scaler with them. */
    AX_IVPS_SCL_INPUT_SHARE, /* Share with vpp in pipeline. If the scaler is not enough, you can choose it. There may be a delay in the response. */
    AX_IVPS_SCL_INPUT_BUTT
} AX_IVPS_SCL_INPUT_E;

typedef struct
{
    AX_IVPS_ASPECT_RATIO_E eMode;
    AX_U32 nBgColor;
    /* YUV color:
    31       23      15      7       0
    |--------|---Y---|---U---|---V---|
    */
    AX_IVPS_ASPECT_RATIO_ALIGN_E eAligns[2]; /* IVPS_ASPECT_RATIO_ALIGN: [0]: HORIZONTAL [1]: VERTICAL */
    AX_IVPS_RECT_T tRect;                    /* Should be set in ASPECT_RATIO_MANUAL mode */
} AX_IVPS_ASPECT_RATIO_T;

typedef struct
{
    AX_IVPS_ASPECT_RATIO_T tAspectRatio;
    AX_IVPS_SCL_TYPE_E eSclType;
    AX_IVPS_SCL_INPUT_E eSclInput;
    AX_U8 nAlpha;                            /* RW; range: [0, 255]; 0: transparent, 255: opaque */
} AX_IVPS_CROP_RESIZE_ATTR_T;

typedef struct
{
    /* src frame rate (<= 0: no FRC control) */
    AX_S32 nSrcFrameRate;
    /* dst frame rate (<= 0, or nSrcFrameRate must be set) */
    AX_S32 nDstFrameRate;
} AX_IVPS_FRAME_RATE_CTRL_T;

typedef enum
{
    AX_IVPS_FRC_RATIO = 0,
    AX_IVPS_FRC_ABS,
    AX_IVPS_FRC_BUTT
} AX_IVPS_FRC_MODE_E;

typedef struct
{
    AX_POOL_SOURCE_E ePoolSrc; /* RW; pool allocation method; 0(default): common pool; 1: private pool; 2: user pool */
    AX_U8 nFrmBufNum;          /* RW; private pool frame buffer count; active in private pool */
    AX_POOL PoolId;            /* RW; user_pool_id; active in user pool */
} AX_IVPS_POOL_ATTR_T;

typedef struct {
    AX_BOOL bEnable;
    AX_COORD_E eCoordMode;
    AX_IVPS_RECT_T tCropRect;
    AX_S8 nSnsId;
    AX_BOOL bCropFirst;
} AX_IVPS_CROP_INFO_T;

typedef struct {
    AX_CSC_MATRIX_T stUserMatrix;
    AX_U8 nGlobalAlpha;
} AX_CSC_PARAM_T;

/*
 * The style of rectangle is like below
 * [            ]
 *
 * [            ]
 * if bEnable is AX_TRUE, then LineWidth should be non-zero
 */
typedef struct
{
    AX_BOOL bEnable;
    AX_U16 nHorLength;
    AX_U16 nVerLength;
} AX_IVPS_CORNER_RECT_ATTR_T;

typedef enum
{
    AX_IVPS_SCALE_RANGE_0 = 0, /* scale range <  8/64  */
    AX_IVPS_SCALE_RANGE_1,     /* scale range >= 8/64  */
    AX_IVPS_SCALE_RANGE_2,     /* scale range >= 12/64 */
    AX_IVPS_SCALE_RANGE_3,     /* scale range >= 16/64 */
    AX_IVPS_SCALE_RANGE_4,     /* scale range >= 20/64 */
    AX_IVPS_SCALE_RANGE_5,     /* scale range >= 24/64 */
    AX_IVPS_SCALE_RANGE_6,     /* scale range >= 28/64 */
    AX_IVPS_SCALE_RANGE_7,     /* scale range >= 32/64 */
    AX_IVPS_SCALE_RANGE_8,     /* scale range >= 36/64 */
    AX_IVPS_SCALE_RANGE_9,     /* scale range >= 40/64 */
    AX_IVPS_SCALE_RANGE_10,    /* scale range >= 44/64 */
    AX_IVPS_SCALE_RANGE_11,    /* scale range >= 48/64 */
    AX_IVPS_SCALE_RANGE_12,    /* scale range >= 52/64 */
    AX_IVPS_SCALE_RANGE_13,    /* scale range >= 56/64 */
    AX_IVPS_SCALE_RANGE_14,    /* scale range >= 60/64 */
    AX_IVPS_SCALE_RANGE_15,    /* scale range >= 64/64 */
    AX_IVPS_SCALE_RANGE_BUTT
} AX_IVPS_SCALE_RANGE_TYPE_E;

typedef struct {
    AX_IVPS_SCALE_RANGE_TYPE_E eHorScaleRange;
    AX_IVPS_SCALE_RANGE_TYPE_E eVerScaleRange; /* Reserved */
} AX_IVPS_SCALE_RANGE_T;

typedef enum
{
    AX_IVPS_COEF_LEVEL_0 = 0, /* coefficient level 0 */
    AX_IVPS_COEF_LEVEL_1,     /* coefficient level 1 */
    AX_IVPS_COEF_LEVEL_2,     /* coefficient level 2 */
    AX_IVPS_COEF_LEVEL_3,     /* coefficient level 3 */
    AX_IVPS_COEF_LEVEL_4,     /* coefficient level 4 */
    AX_IVPS_COEF_LEVEL_5,     /* coefficient level 5 */
    AX_IVPS_COEF_LEVEL_BUTT,
} AX_IVPS_COEF_LEVEL_E;


typedef struct {
    AX_IVPS_COEF_LEVEL_E eHorLuma;   /* horizontal luminance coefficient level            */
    AX_IVPS_COEF_LEVEL_E eHorChroma; /* horizontal chrominance coefficient level Reserved */
    AX_IVPS_COEF_LEVEL_E eVerLuma;   /* vertical luminance coefficient level     Reserved */
    AX_IVPS_COEF_LEVEL_E eVerChroma; /* vertical chrominance coefficient level   Reserved */
} AX_IVPS_SCALE_COEF_LEVEL_T;

/***************************************************************************************************************/
/*                                                   GDC                                                       */
/***************************************************************************************************************/
typedef enum
{
    AX_IVPS_DEWARP_BYPASS = 0,     /* only support crop, rotation, mirror, flip or scaling */
    AX_IVPS_DEWARP_MAP_USER,       /* user defined map */
    AX_IVPS_DEWARP_PERSPECTIVE,    /* affine or perspective transformation */
    AX_IVPS_DEWARP_LDC,            /* lens distortion correction */
    AX_IVPS_DEWARP_LDC_V2,         /* lens distortion correction version 2 */
    AX_IVPS_DEWARP_LDC_PERSPECTIVE,/* LDC and PERSPECTIVE are done together */
    AX_IVPS_DEWARP_BUTT
} AX_IVPS_DEWARP_TYPE_E;

/*
 * AX_IVPS_MAP_USER_ATTR_T
 * nMeshWidth * (nMeshNumH - 1) >= DstWidth
 * nMeshHeight * (nMeshNumV - 1) >= DstHeight
 */
typedef struct
{
    AX_BOOL bPixelWise;        /* RW; point by point mesh enable */
    AX_U16 nMeshStartX;        /* RW; range: [0, output width]; x-coordinate of output picture in the mesh table */
    AX_U16 nMeshStartY;        /* RW; range: [0, output height]; y-coordinate of output picture in the mesh table */
    AX_U16 nMeshWidth;         /* RW; range: [16, 256]; 16 aligned; width of mesh block */
    AX_U16 nMeshHeight;        /* RW; range: [16, 256]; 16 aligned; height of mesh block */
    AX_U8 nMeshNumH;           /* RW; range: [33, 64]; number of mesh block in horizontal direction; Must be equal with nMeshNumV */
    AX_U8 nMeshNumV;           /* RW; range: [33, 64]; number of mesh block in vertical direction; Must be equal with nMeshNumH */
    AX_S32 *pUserMap;          /* RW; virtual address meshtable */
    AX_U64 nMeshTablePhyAddr;  /* RW; physical address of meshtable */
} AX_IVPS_MAP_USER_ATTR_T;

 typedef struct
 {
     AX_U16 nMeshStartX;        /* RW; range: [0, output width]; x-coordinate of output picture in the mesh table */
     AX_U16 nMeshStartY;        /* RW; range: [0, output height]; y-coordinate of output picture in the mesh table */
     AX_U16 nMeshWidth;         /* RW; range: [8, 1024); 8 aligned; width of mesh block */
     AX_U16 nMeshHeight;        /* RW; range: [8, 1024); 8 aligned; height of mesh block */
     AX_U16 nMeshNumH;          /* RW; range: when bPixelWise is true, nMeshNumH = output_width, or else in [8, 129]; number of mesh block in horizontal direction; Must be equal with nMeshNumV */
     AX_U16 nMeshNumV;          /* RW; range:  when bPixelWise is true, nMeshNumV = output_height, or else in [8, 129]; number of mesh block in vertical direction; Must be equal with nMeshNumH */
     AX_U64 *pYGainMesh;        /* RW; luma(y channel) gain mesh table */
     AX_U64 *pCMatMesh;         /* RW;  chroma(u/v channel) correction matrix mesh table */
     AX_U64 *pUVOffsetMesh;     /* RW; chroma(u/v channel) offset mesh table */
 } AX_IVPS_LGC_ATTR_T;

/*
 * AX_IVPS_PERSPECTIVE_ATTR_T
 * Perspective Matrix =
 *   [m(0,0),   m(0,1),   m(0,2)],
 *   [m(1,0),   m(1,1),   m(1,2)],
 *   [m(2,0),   m(2,1),   m(2,2)],
 * If [m(2,0), m(2,1), m(2,2)] = [0, 0, 1], the transformation is affine.
 * Note:
 * 1. m(2,2) is not 0.
 * 2. The matrix element has 6 decimal numbers.
 * i.e. If the element is 954301, 954301/1000000 = 0.954301,
 * the real value is 0.954301.
 */
typedef struct
{
    AX_S64 nMatrix[9];
} AX_IVPS_PERSPECTIVE_ATTR_T;

typedef struct
{
    AX_BOOL bAspect;         /* whether aspect ration is keep */
    AX_S16 nXRatio;          /* Range: [0, 100], field angle ration of horizontal, valid when bAspect = 0. */
    AX_S16 nYRatio;          /* Range: [0, 100], field angle ration of vertical, valid when bAspect = 0. */
    AX_S16 nXYRatio;         /* Range: [0, 100], field angle ration of all,valid when bAspect = 1. */
    AX_S16 nCenterXOffset;   /* Range: [-511, 511], horizontal offset of the image distortion center relative to image center. */
    AX_S16 nCenterYOffset;   /* Range: [-511, 511], vertical offset of the image distortion center relative to image center. */
    AX_S16 nDistortionRatio; /* Range: [-10000, 10000], LDC distortion ratio. [-10000, 0): pincushion distortion; (0, 10000]: barrel distortion */
    AX_S8 nSpreadCoef;       /* Range: [-18, 18], LDC spread coefficient */
} AX_IVPS_LDC_ATTR_T;

/*
 * AX_IVPS_LDC_V2_ATTR_T
 * Camera Matrix(internal parameter matrix) =
 *   [nXFocus,     0,        nXCenter],
 *   [0,          nYFocus,   nYCenter],
 *   [0,           0,              1 ],
 * The element has 2 decimal numbers.
 * i.e. If the element is 192029, 192029/100 = 1920.29,
 * the real value is 1920.29.
 *
 * Distortion Coefficients =
 *  (k1, k2, p1, p2, k3, k4, k5, k6)
 * The element has 6 decimal numbers.
 * i.e. If the element is 954301, 954301/1000000 = 0.954301,
 * the real value is 0.954301.
 */
typedef struct
{
    AX_U32 nXFocus;
    AX_U32 nYFocus;
    AX_U32 nXCenter;
    AX_U32 nYCenter;
    AX_S64 nDistortionCoeff[8];
} AX_IVPS_LDC_V2_ATTR_T;

typedef struct
{
    AX_IVPS_DEWARP_TYPE_E eDewarpType;

    AX_IVPS_ROTATION_E eRotation;
    AX_BOOL bMirror;
    AX_BOOL bFlip;

    union {
        /* AX_IVPS_DEWARP_MAP_USER */
        AX_IVPS_MAP_USER_ATTR_T tMapUserAttr;
        /* AX_IVPS_DEWARP_LDC */
        AX_IVPS_LDC_ATTR_T tLdcAttr;
        /* AX_IVPS_DEWARP_LDC_V2 */
        AX_IVPS_LDC_V2_ATTR_T tLdcV2Attr;
    };
    /* AX_IVPS_DEWARP_PERSPECTIVE */
    AX_IVPS_PERSPECTIVE_ATTR_T tPerspectiveAttr;
} AX_IVPS_GDC_CFG_T;

typedef struct
{
    AX_BOOL bOutRect;           /* RW; whether output rectangle is enabled */
    AX_IVPS_RECT_T tOutRect;    /* RW; 2 pixels aligned; output rectangle info */
    AX_BOOL bMeshOffset;

    AX_IVPS_DEWARP_TYPE_E eDewarpType;
    union {
        AX_IVPS_PERSPECTIVE_ATTR_T tPerspectiveAttr;
        AX_IVPS_MAP_USER_ATTR_T tMapUserAttr;
    };
} AX_IVPS_DEWARP_ATTR_T;

 /***************************************************************************************************************/
 /*                                                   RGN                                                       */
 /***************************************************************************************************************/

typedef enum
{
    AX_IVPS_RGN_TYPE_LINE = 0,
    AX_IVPS_RGN_TYPE_RECT = 1,
    AX_IVPS_RGN_TYPE_POLYGON = 2, /* convex quadrilateral */
    AX_IVPS_RGN_TYPE_MOSAIC = 3,
    AX_IVPS_RGN_TYPE_OSD = 4,
    AX_IVPS_RGN_TYPE_BUTT
} AX_IVPS_RGN_TYPE_E;

typedef struct
{
    AX_BOOL bEnable;
    AX_BOOL bColorKeyInv; /* RW; 0: winin threshold, 1: outside threshold */
    IVPS_RGB nBgColorLo;  /* RW; min value of color difference with background; 0xRRGGBB */
    IVPS_RGB nBgColorHi;  /* RW; max value of color difference with background; 0xRRGGBB */
} AX_IVPS_COLORKEY_T;

typedef struct
{
    AX_S16 nZindex;                 /* RW; if drawing OSD, for the same filter, different region handle owns different nZindex */
    AX_BOOL bSingleCanvas;          /* RW; AX_TURE: single canvas; AX_FALSE: double canvas */
    AX_U16 nAlpha;                  /* RW; range: (0, 255]; 0: transparent, 255: opaque */
    AX_IMG_FORMAT_E eFormat;
    AX_BITCOLOR_T nBitColor;        /* RW; reserved */
    AX_IVPS_COLORKEY_T nColorKey;   /* RW; reserved */
    AX_BOOL bVoRect;                /* RW; reserved */
} AX_IVPS_RGN_CHN_ATTR_T;

typedef struct
{
    AX_POINT_T tPTs[2];       /* RW; fixed two point coordinates, starting and ending points of the line */
    AX_U32 nLineWidth;        /* RW; range: [1, 16]  */
    IVPS_RGB nColor;          /* RGB Color: 0xRRGGBB, eg: red: 0xFF0000 */
    AX_U8 nAlpha;             /* RW; range: (0, 255]; 0: transparent, 255: opaque */
} AX_IVPS_RGN_LINE_T;

typedef enum
{
    AX_IVPS_MASK_INNER_FILL = 0,
    AX_IVPS_MASK_OUTTER_FILL = 1,
    AX_IVPS_MASK_INNER_CLR = 2,
    AX_IVPS_MASK_OUTTER_CLR = 3,
    AX_IVPS_MASK_MODE_BUTT
} AX_IVPS_MASK_MODE_E;

typedef struct
{
    union
    {
        AX_IVPS_RECT_T tRect;       /* rectangle info */
        AX_IVPS_POINT_T tPTs[AX_IVPS_MAX_POLYGON_POINT_NUM];  /* RW; polygon fixed-point coordinates */
    };
    AX_U8 nPointNum;                /* RW; range: [3, 10] [AX_IVPS_MIN_POLYGON_POINT_NUM, AX_IVPS_MAX_POLYGON_POINT_NUM]*/
    AX_U32 nLineWidth;              /* RW; range: [0, 16] , if 0, fill the rect */
    IVPS_RGB nColor;                /* RW; range: [0, 0xffffff]; color RGB888; 0xRRGGBB */
    AX_U8 nAlpha;                   /* RW; range: (0, 255]; 0: transparent, 255: opaque */
    AX_BOOL bSolid;                 /* if AX_TRUE, fill the rect with the nColor */
    AX_IVPS_MASK_MODE_E eMaskMode;
    AX_IVPS_CORNER_RECT_ATTR_T tCornerRect;  /* if CornerRect is Enable, then LineWidth should be non-zero */
} AX_IVPS_RGN_POLYGON_T;

typedef struct
{
    AX_IVPS_RECT_T tRect;
    AX_IVPS_MOSAIC_BLK_SIZE_E eBklSize;
} AX_IVPS_RGN_MOSAIC_T;

typedef AX_OSD_BMP_ATTR_T AX_IVPS_RGN_OSD_T;

typedef union
{
    AX_IVPS_RGN_LINE_T tLine;
    AX_IVPS_RGN_POLYGON_T tPolygon;
    AX_IVPS_RGN_MOSAIC_T tMosaic;
    AX_IVPS_RGN_OSD_T tOSD;
} AX_IVPS_RGN_DISP_U;

typedef struct
{
    AX_BOOL bShow;
    AX_IVPS_RGN_TYPE_E eType;
    AX_IVPS_RGN_DISP_U uDisp;
} AX_IVPS_RGN_DISP_T;

typedef struct
{
    AX_U32 nNum;
    AX_IVPS_RGN_CHN_ATTR_T tChnAttr;
    AX_IVPS_RGN_DISP_T arrDisp[AX_IVPS_REGION_MAX_DISP_NUM];
} AX_IVPS_RGN_DISP_GROUP_T;

typedef struct
{
    AX_U16 nThick;
    AX_U16 nAlpha;
    AX_U32 nColor;
    AX_BOOL bSolid;  /* if AX_TRUE, fill the rect with the nColor, Reserved */
    AX_BOOL bAbsCoo; /* is Absolute Coordinate */
    AX_IVPS_CORNER_RECT_ATTR_T tCornerRect; /* if corner rect is Enable, then nThick should be non-zero */
} AX_IVPS_GDI_ATTR_T;

typedef struct
{
    AX_U64 nPhyAddr;
    AX_VOID *pVirAddr;
    AX_U32 nUVOffset; /* Pixels of Y and UV offset. If YUV420 format, nUVOffset = u64PhyAddr[1] - u64PhyAddr[0] */
    AX_U32 nStride;
    AX_U16 nW;
    AX_U16 nH;
    AX_IMG_FORMAT_E eFormat;
} AX_IVPS_RGN_CANVAS_INFO_T;

#define AX_IVPS_MAX_PALETTE_TABLE_NUM 256

typedef struct _AX_IVPS_ARGB {
    AX_U8 nAlpha;
    AX_U8 nRed;
    AX_U8 nGreen;
    AX_U8 nBlue;
} AX_IVPS_ARGB_T;

typedef struct _AX_IVPS_PALETTE_TABLE {
    AX_IVPS_ARGB_T atElement[AX_IVPS_MAX_PALETTE_TABLE_NUM];
} AX_IVPS_PALETTE_TABLE_T;


/***************************************************************************************************************/
/*                                                   PIPELINE                                                  */
/***************************************************************************************************************/
typedef struct
{
    AX_U16 nDstWidth;
    AX_U16 nDstHeight;
} AX_IVPS_VPP_CASCADE_CFG_T;

typedef struct
{
    AX_BOOL bCascadePrevChn;
    AX_BOOL bMirror;
    AX_BOOL bFlip;
    AX_U8 nCascadeNum;
    AX_IVPS_VPP_CASCADE_CFG_T tCfg[AX_IVPS_VPP_MAX_CASCADE_NUM];
} AX_IVPS_VPP_CFG_T;

typedef struct
{
    AX_BOOL bMirror;
    AX_BOOL bFlip;
} AX_IVPS_VGP_CFG_T;

/*
 * AX_IVPS_ENGINE_SCL: scaler module. This module must be placed on the channel and cannot exist independently.
 *                     There must be a previous module(GDC or VPP) with online link relationship between them.
 */
typedef enum
{
    AX_IVPS_ENGINE_SCL = 0,
    AX_IVPS_ENGINE_TDP,
    AX_IVPS_ENGINE_GDC,
    AX_IVPS_ENGINE_VPP,
    AX_IVPS_ENGINE_VGP,
    AX_IVPS_ENGINE_VO,
    AX_IVPS_ENGINE_BUTT
} AX_IVPS_ENGINE_E;

/*
 * AX_IVPS_PIPELINE_DEFAULT is the mainstream pipeline that we currently support. The mode is as follows:
 *                             |--> Pipe1Filter0 Pipe1Filter1 (chn 0)
 *                             |--> Pipe2Filter0 Pipe2Filter1 (chn 1)
 * Pipe0Filter0 Pipe0Filter1-->|--> Pipe3Filter0 Pipe3Filter1 (chn 2)
 *                             |--> Pipe4Filter0 Pipe4Filter1 (chn 3)
 *                             |--> Pipe5Filter0 Pipe5Filter1 (chn 4)
 */
typedef enum
{
    AX_IVPS_PIPELINE_DEFAULT = 0,
    AX_IVPS_PIPELINE_BUTT
} AX_IVPS_PIPELINE_E;

typedef struct
{
    AX_BOOL bEngage;
    AX_IVPS_ENGINE_E eEngine;

    AX_FRAME_RATE_CTRL_T tFRC;
    AX_BOOL bCrop;
    AX_IVPS_RECT_T tCropRect;

    AX_U16 nDstPicWidth;
    AX_U16 nDstPicHeight;
    AX_U16 nDstPicStride;
    AX_IMG_FORMAT_E eDstPicFormat;
    AX_FRAME_COMPRESS_INFO_T tCompressInfo;

    AX_BOOL bInplace;
    AX_IVPS_ASPECT_RATIO_T tAspectRatio;
    AX_IVPS_SCL_TYPE_E eSclType;

    union /* engine specific config data */
    {
        AX_IVPS_GDC_CFG_T tGdcCfg;
        AX_IVPS_VGP_CFG_T tVgpCfg;
        AX_IVPS_VPP_CFG_T tVppCfg;
    };
} AX_IVPS_FILTER_T;

/*
    * AX_IVPS_PIPELINE_ATTR_T
    * Dynamic attribute for the group.
    * @nOutChnNum: [1~AX_IVPS_MAX_OUTCHN_NUM] set according to output channel number.
    * @nInDebugFifoDepth: [0~1024] used by ax_ivps_dump.bin.
    * @nOutFifoDepth: [0~4] If user want to get frame from channel, set it to [1~4].
    * Otherwise, set it to 0.
    */
typedef struct
{
    AX_U8 nOutChnNum;
    AX_U16 nInDebugFifoDepth;
    AX_U8 nOutFifoDepth[AX_IVPS_MAX_OUTCHN_NUM];
    AX_IVPS_FILTER_T tFilter[AX_IVPS_MAX_OUTCHN_NUM + 1][AX_IVPS_MAX_FILTER_NUM_PER_OUTCHN];
    AX_IVPS_FRC_MODE_E eFRCMode;
} AX_IVPS_PIPELINE_ATTR_T;

typedef struct
{
    AX_FRAME_RATE_CTRL_T tFRC;
    AX_U16 nDstPicWidth;
    AX_U16 nDstPicHeight;
    AX_U16 nDstPicStride;
    AX_IMG_FORMAT_E eDstPicFormat;
    AX_IVPS_ASPECT_RATIO_T tAspectRatio;
    AX_U8 nOutFifoDepth;
} AX_IVPS_CHN_ATTR_T;

typedef struct
{
    AX_U8 nInFifoDepth;
    AX_IVPS_PIPELINE_E ePipeline;
} AX_IVPS_GRP_ATTR_T;

#endif
