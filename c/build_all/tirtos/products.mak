#
#  ======== products.mak ========
#

##################### All platforms ####################

XDCTOOLS_INSTALLATION_DIR ?= /path/to/xdctools_installation

##################### TI-RTOS ####################

TIRTOS_INSTALLATION_DIR ?= /path/to/tirtos_installation
BIOS_INSTALLATION_DIR   ?= $(TIRTOS_INSTALLATION_DIR)/products/bios_6_42_02_29
UIA_INSTALLATION_DIR    ?= $(TIRTOS_INSTALLATION_DIR)/products/uia_2_00_02_39
NS_INSTALLATION_DIR     ?= /path/to/ns_installation

##################### TI-RTOS (CC3200) ####################

CC3200SDK_VERSION = 1.1.0
CCWARE_INSTALLATION_DIR    ?= $(TIRTOS_INSTALLATION_DIR)/products/CC3200_driverlib_1.1.0
CC3200SDK_INSTALLATION_DIR ?= /path/to/cc3200-sdk

# Leave assignment empty to disable this toolchain
ti.targets.arm.elf.M4  ?= /path/to/TI_ARM_compiler
