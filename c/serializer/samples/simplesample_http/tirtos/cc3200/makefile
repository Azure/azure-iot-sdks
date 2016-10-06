#
#  Copyright (c) 2015, Texas Instruments Incorporated
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

TREE_ROOT = ../../../../..
include $(TREE_ROOT)/build_all/tirtos/products.mak
include makedefs

CONFIG = main

CFLAGS :=  -I../.. -I../../../../inc/ -I../../../../../c-utility/inc/ -I../../../../../c-utility/inc/azure_c_shared_utility -I../../../../../iothub_client/inc -g --cmd_file=./$(CONFIG)/compiler.opt $(CFLAGS)

LIBS =  $(TREE_ROOT)/c-utility/build_all/tirtos/lib/common_sl.aem4 \
    $(TREE_ROOT)/iothub_client/build/tirtos/lib/iotclient_sl.aem4 \
    $(TREE_ROOT)/serializer/build/tirtos/lib/serializer_sl.aem4

VPATH = ../..

.PRECIOUS: %/compiler.opt %/linker.cmd

%/compiler.opt %/linker.cmd : %.cfg
	@$(CONFIGURO) -c $(CODEGEN_INSTALLATION_DIR) -t $(TARGET) -p $(PLATFORM) -r release $<

%.o : %.c

%.o : %.c $(CONFIG)/compiler.opt
	@$(CC) $(CFLAGS) -c $<

all: simplesample_http.bin

simplesample_http.out: main.o simplesample_http.o rom_pin_mux_config.o netwifi.o UARTUtils.o CC3200_LAUNCHXL.o network_common.o CC3200_LAUNCHXL.cmd $(CONFIG)/linker.cmd $(LIBS)
	@echo building $@ ..
	@$(LD) -o $@ $^ -x -m $@.map $(LFLAGS)

simplesample_http.bin: simplesample_http.out
	@$(binconv) $^ $@

clean:
	@echo cleaning ..
	@ $(call remove, *.o *.out *.map *.bin)
	@ $(RMDIR) $(CONFIG)
