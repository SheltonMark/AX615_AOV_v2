/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef _AX_BUFFER_TOOL_H_
#define _AX_BUFFER_TOOL_H_

#include "ax_base_type.h"
#include "ax_global_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* End of #ifdef __cplusplus */

#define AX_COMM_ALIGN(value, n) (((value) + (n) - 1) & ~((n) - 1))


/*
 * eImageFormat: Image Format, only support raw
 * pstCompressInfo: Compression mode and compression level
 */
AX_U32 AX_VIN_GetPixelBitDepth(AX_IMG_FORMAT_E eImageFormat, AX_FRAME_COMPRESS_INFO_T *pstCompressInfo);

/*
 * uHeight: Image height in pixels
 * uWidthStride: Image width in pixels
 * eImageFormat: Image Format, only support raw
 * pstCompressInfo: Compression mode and compression level
 * uAlignSize: Aligned size in bytes, If input is 0, align by 128 bytes
 * ebufMode: buffer mode
 */
AX_U32 AX_VIN_CalImgSize(AX_U32 uHeight, AX_U32 uWidthStride, AX_IMG_FORMAT_E eImageFormat,
        AX_FRAME_COMPRESS_INFO_T *pstCompressInfo, AX_U32 uAlignSize, AX_U32 ubufMode);

/*
 * uHeight: Image height in pixels
 * uWidthStride: Image width in pixels
 * eImageFormat: Image Format, only support raw
 * pstCompressInfo: Compression mode and compression level
 * uAlignSize: Aligned size in bytes, If input is 0, align by 128 bytes
 */
AX_U32 AX_VIN_GetImgBufferSize(AX_U32 uHeight, AX_U32 uWidthStride, AX_IMG_FORMAT_E eImageFormat,
        AX_FRAME_COMPRESS_INFO_T *pstCompressInfo, AX_U32 uAlignSize);

/*
 * uHeight: Image height in pixels
 * uWidthStride: Image width in pixels
 * eImageFormat: Image Format, only support raw
 * pstCompressInfo: Compression mode and compression level
 * uAlignSize: Aligned size in bytes, If input is 0, align by 128 bytes
 * ebufMode: buffer mode
 */
AX_U32 AX_VIN_GetBufferSize(AX_U32 uHeight, AX_U32 uWidthStride, AX_IMG_FORMAT_E eImageFormat,
        AX_FRAME_COMPRESS_INFO_T *pstCompressInfo, AX_U32 uAlignSize, AX_U32 ubufMode);

/*
 * uHeight: Image height in pixels
 * uWidthStride: Image width in bytes
 * eImageFormat: Image Format, only support Yuv
 * pstCompressInfo: Compression mode and compression level
 * uAlignSize: Aligned size in bytes
 */
AX_U32 AX_VIN_GetYuvImgBufferSize(AX_U32 uHeight, AX_U32 uWidthStride, AX_IMG_FORMAT_E eImageFormat,
        AX_FRAME_COMPRESS_INFO_T *pstCompressInfo, AX_U32 uAlignSize);

/*
 * uWidthPixel: Image width in pixels
 * eImageFormat: Image Format
 * uAlignSizeByte: Aligned size in bytes
 * return uStrideByte: Image width in bytes
 */
AX_U32 AX_VIN_GetImgStrideByte(AX_U32 uWidthPixel, AX_IMG_FORMAT_E eImageFormat, AX_U32 uAlignSizeByte);

AX_U32 AX_VDEC_GetPicBufferSize(AX_U32 uWidth, AX_U32 uHeight, AX_PAYLOAD_TYPE_E enType);

AX_U32 AX_JDEC_GetYuvBufferSize(AX_U32 uWidth, AX_U32 uHeight, AX_IMG_FORMAT_E eOutputFormat);

#ifdef __cplusplus
}
#endif /* End of #ifdef __cplusplus */

#endif //_AX_BUFFER_TOOL_H_
