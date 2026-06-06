CUR_PATH                     := $(shell pwd)
HOME_PATH                     = $(abspath $(CUR_PATH)/../../..)

include $(HOME_PATH)/build/color.mk
include $(HOME_PATH)/build/config.mak

OUT_PATH                     := $(MSP_OUT_PATH)
SRC_PATH                     := $(CUR_PATH)/src
BASE_PATH                    := $(HOME_PATH)/msp/component
SRC_RELATIVE_PATH            := $(subst $(HOME_PATH)/,,$(CUR_PATH))
TARGET_OUT_PATH              := $(OBJS_OUT_PATH)/$(SRC_RELATIVE_PATH)

################################################################################
# build
################################################################################

QSDEMO_AUDIO_SUPPORT         := yes
QSDEMO_AUDIO_AAC_SUPPORT     := yes
QSDEMO_MP4ENC_SUPPORT        := no
QSDEMO_CROSS_PROCESS_SUPPORT := no
QSDEMO_TCM_SUPPORT           := yes

.PHONY : all clean install
.NOTPARALLEL: clean install
all: app
	@echo -e $(GREEN)"\nBuild $@ success!!\n"  $(DONE)

clean:
	$(VERB) $(MAKE) -f Makefile.dynamic clean
	$(VERB) $(MAKE) -f Makefile.static clean
	@echo -e $(GREEN)"\nClean $@ success!!\n"  $(DONE)

install:
	@echo -e $(GREEN)"Install dynamic ..."  $(DONE)
	$(VERB) $(MAKE) -f Makefile.dynamic install
	@echo -e $(GREEN)"Install static ..."  $(DONE)
	$(VERB) $(MAKE) -f Makefile.static install

	$(VERB) $(MKDIR) $(ROOTFS_TARGET_PATH)/data/qsdemo
	$(VERB) $(CP) $(SRC_PATH)/res $(ROOTFS_TARGET_PATH)/data/qsdemo -R

ifeq ($(AX_BOOT_OPTIMIZATION_SUPPORT),TRUE)
	@sed -i "2s/qsdemo/qsdemo_s/" $(ROOTFS_TARGET_PATH)/data/qsdemo/res/run.sh
endif

	@$(MKDIR) $(PRJ_BUILD_OUT_PATH)/images/debug
	@$(CP) $(TARGET_OUT_PATH)/qsdemo_debug $(PRJ_BUILD_OUT_PATH)/images/debug/qsdemo
	@$(CP) $(TARGET_OUT_PATH)/qsdemo_s_debug $(PRJ_BUILD_OUT_PATH)/images/debug/qsdemo_s

app:
	@echo -e $(GREEN)"Build dynamic $@ ..."  $(DONE)
	$(VERB) $(MAKE) -B -f Makefile.dynamic
	@echo -e $(GREEN)"Build static $@ ..."  $(DONE)
	$(VERB) $(MAKE) -B -f Makefile.static

export QSDEMO_AUDIO_SUPPORT
export QSDEMO_AUDIO_AAC_SUPPORT
export QSDEMO_MP4ENC_SUPPORT
export QSDEMO_CROSS_PROCESS_SUPPORT
export QSDEMO_TCM_SUPPORT
