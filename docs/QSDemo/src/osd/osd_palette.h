/**************************************************************************************************
 *
 * Copyright (c) 2019-2025 Axera Semiconductor Co., Ltd. All Rights Reserved.
 *
 * This source file is the property of Axera Semiconductor Co., Ltd. and
 * may not be copied or distributed in any isomorphic form without the prior
 * written consent of Axera Semiconductor Co., Ltd.
 *
 **************************************************************************************************/

#ifndef __OSDPALETTE_H__
#define __OSDPALETTE_H__

#include "ax_ivps_type.h"

const static AX_IVPS_PALETTE_TABLE_T g_tOsdPalette = {
    .atElement = {
        // a    r    g    b
        {  0,   0,   0,   0},  // index 0, a=0 transparent
        {255, 255, 255, 255},  // index 1, reserved
        {255,   0,   0,   0},  // index 2, reserved
        // common color, total 7
        {255,   0,   0,   0},  // index 3, black
        {255, 255, 255, 255},  // index 4, white
        {255, 255,   0,   0},  // index 5, red
        {255,   0, 255,   0},  // index 6, green
        {255,   0,   0, 255},  // index 7, blue
        {255, 255, 255,   0},  // index 8, yellow
        {255, 255,   0, 255},  // index 9, purple
        // logo color
        {255, 128, 128, 128},  // index 10
        {255, 200, 200, 200},  // index 11
        {255, 202, 202, 202},  // index 12
        {255, 205, 205, 205},  // index 13
        {255, 234, 234, 234},  // index 14
        {255, 255, 255, 255},  // index 15
    }
};

#define PALETTE_FONT_COLOR_INDEX  (3)
#define PALETTE_FONT_COLOR_COUNT  (7)

AX_U8 GetColorIndex(AX_U32 nColor) {
    if (nColor == 0x0) {
        return 0;  // transparent
    }
    for (AX_U8 i = 0; i < PALETTE_FONT_COLOR_COUNT; i++) {
        AX_IVPS_ARGB_T tColor = g_tOsdPalette.atElement[i + PALETTE_FONT_COLOR_INDEX];
        AX_U32 nCURColor = ((AX_U32)tColor.nAlpha) << 24 | ((AX_U32)tColor.nRed) << 16 | ((AX_U32)tColor.nGreen) << 8 | tColor.nBlue;
        if (nColor == nCURColor) {
            return i + PALETTE_FONT_COLOR_INDEX;
        }
    }
    return 0; // transparent
}


#endif // __OSDPALETTE_H__
