#!/bin/bash
# Rebuild libmedia and libmedia_demo for board testing
# Usage: ./rebuild_and_test.sh

set -e  # Exit on error

echo "=========================================="
echo "Rebuilding libmedia and libmedia_demo"
echo "=========================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Step 1: Clean and rebuild libmedia
echo -e "${YELLOW}[1/4] Cleaning libmedia...${NC}"
cd libmedia/Build
make clean

echo -e "${YELLOW}[2/4] Building libmedia...${NC}"
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ libmedia build failed!${NC}"
    exit 1
fi
echo -e "${GREEN}✓ libmedia built successfully${NC}"

# Step 2: Rebuild libmedia_demo
cd ../../test/libmedia_demo
echo -e "${YELLOW}[3/4] Cleaning libmedia_demo...${NC}"
make clean

echo -e "${YELLOW}[4/4] Building libmedia_demo...${NC}"
make -j$(nproc)
if [ $? -ne 0 ]; then
    echo -e "${RED}✗ libmedia_demo build failed!${NC}"
    exit 1
fi
echo -e "${GREEN}✓ libmedia_demo built successfully${NC}"

# Summary
echo ""
echo "=========================================="
echo -e "${GREEN}Build completed successfully!${NC}"
echo "=========================================="
echo ""
echo "Next steps:"
echo "  1. Copy test/libmedia_demo/libmedia_demo to board's /tmp/"
echo "  2. Copy test/libmedia_demo/run_libmedia_demo.sh to board's /tmp/"
echo "  3. On board, run: cd /tmp && ./run_libmedia_demo.sh -x 3 -o /tmp/libmedia_demo.h265"
echo ""
echo "Expected output:"
echo "  [AxVinAdapter] Pre-StartPipe diagnostics: ..."
echo "  [AxVinAdapter] AX_VIN_StartPipe OK          ← Should succeed now!"
echo "  [Pipeline] VIN Enable OK"
echo "  [Pipeline] Starting ISP (AX_ISP_Start)"
echo ""
