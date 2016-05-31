#
#  ======== products.mak ========
#

##################### All platforms ####################

XDCTOOLS_INSTALLATION_DIR ?= c:\\ti\\xdctools_3_31_01_33_core

##################### TI-RTOS ####################

TIRTOS_INSTALLATION_DIR ?= c:\\ti/tirtos_simplelink_2_14_01_20
BIOS_INSTALLATION_DIR   ?= $(TIRTOS_INSTALLATION_DIR)/products/bios_6_42_02_29
UIA_INSTALLATION_DIR    ?= $(TIRTOS_INSTALLATION_DIR)/products/uia_2_00_02_39
NS_INSTALLATION_DIR     ?= c:\\ti/ns_1_10_00_00_eng

##################### TI-RTOS (CC3200) ####################

CC3200SDK_VERSION = 1.1.0
CCWARE_INSTALLATION_DIR    ?= $(TIRTOS_INSTALLATION_DIR)/products/CC3200_driverlib_1.1.0
CC3200SDK_INSTALLATION_DIR ?= c:\\ti/CC3200SDK_1.1.0/cc3200-sdk

# Leave assignment empty to disable this toolchain
ti.targets.arm.elf.M4  ?= c:\\ti/ti-cgt-arm_5.2.5
