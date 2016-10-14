#                                                                                                    
#  Copyright (C) Firmwave Ltd., All rights reserved.                                                 
#  Licensed under the MIT license. See LICENSE file in the project root for full license information.
#                                                                                                    

# Toolchain executables
CROSS_COMPILE = /opt/arm-gnu-toolchain-5.3.1.487-linux.any.x86_64/bin/arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJDUMP = $(CROSS_COMPILE)objdump
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE = $(CROSS_COMPILE)size
NM = $(CROSS_COMPILE)nm

# Include files
INCLUDES = \
    -I../ \
    -I../../../../c-utility/inc \
    -I../../../../iothub_client/inc \
    -I../../../../uamqp/inc \
    -I../../../../c-utility/adapters \
    -I../../../../parson \
    -I../../../../certs \
    -I/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/inc \
    -I/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/common \
    -I/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/demo/common/atmel/boards/same70_xplained \
    -I/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto \
    -I/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl \
    -I/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Source/include \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1 \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/sam/utils/cmsis/same70/include \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/thirdparty/CMSIS/Include \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/sam/utils/cmsis/same70/source/templates \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/sam/utils/fpu \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/sam/utils \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/sam/utils/preprocessor \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/sam/utils/header_files \
    -I/opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/common/utils \

# Global defines
CFLAGS += \
    -DBOARD=SAME70_XPLAINED \
    -DMBED_BUILD_TIMESTAMP \
    -DTRACE_LEVEL=4 \
    -DUSE_SAME70_XPLAINED \
    -D_WINSOCK_H \
    -D__SAME70Q21__ \
    -Dprintf=iprintf \

# Global C flags
CFLAGS += -std=gnu99 -O0 -g \
-march=armv7e-m -mthumb -mfloat-abi=softfp  -mfpu=fpv5-sp-d16 \
-ffunction-sections \
-Wall -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int \
-Wmain -Wparentheses \
-Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs \
-Wuninitialized -Wunknown-pragmas -Wfloat-equal \
-Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings \
-Waggregate-return -Wstrict-prototypes \
-Wmissing-prototypes -Wmissing-declarations \
-Wformat -Wmissing-format-attribute -Wno-deprecated-declarations \
-Wredundant-decls -Wnested-externs \
-Wunreachable-code

# Global linker flags
LDFLAGS += -march=armv7e-m -mthumb -mfloat-abi=softfp  -mfpu=fpv5-sp-d16 \
-Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler \
-Wl,--unresolved-symbols=report-all -Wl,--warn-common \
-Wl,--defsym=malloc_getpagesize_P=0x80 \
-L/opt/CycloneTCP_SSL_Crypto_Open_1_7_4/linker \
-T"same70_flash.ld" \
-Wl,--start-group -lgcc -lc -lm -Wl,--end-group --specs=nosys.specs

# Global libraries

OBJECTS = $(patsubst %.c, obj/%.o, $(SOURCES))
THIS_MAKEFILE := $(lastword $(MAKEFILE_LIST))
CFLAGS += $(INCLUDES)

SOURCES_iothub_client_sample_http = \
    iothub_client_sample_http.c \
    c-utility/src/base64.c \
    c-utility/src/buffer.c \
    c-utility/src/consolelogger.c \
    c-utility/src/constbuffer.c \
    c-utility/src/constmap.c \
    c-utility/src/crt_abstractions.c \
    c-utility/src/doublylinkedlist.c \
    c-utility/src/gb_stdio.c \
    c-utility/src/gb_time.c \
    c-utility/src/gballoc.c \
    c-utility/src/hmac.c \
    c-utility/src/hmacsha256.c \
    c-utility/src/httpapiex.c \
    c-utility/src/httpapiexsas.c \
    c-utility/src/httpheaders.c \
    c-utility/src/map.c \
    c-utility/src/optionhandler.c \
    c-utility/src/sastoken.c \
    c-utility/src/sha1.c \
    c-utility/src/sha224.c \
    c-utility/src/sha384-512.c \
    c-utility/src/singlylinkedlist.c \
    c-utility/src/string_tokenizer.c \
    c-utility/src/strings.c \
    c-utility/src/tlsio_cyclonessl.c \
    c-utility/src/tlsio_cyclonessl_socket.c \
    c-utility/src/urlencode.c \
    c-utility/src/usha.c \
    c-utility/src/vector.c \
    c-utility/src/xio.c \
    c-utility/src/xlogging.c \
    certs/certs.c \
    iothub_client/src/blob.c \
    iothub_client/src/iothub_client.c \
    iothub_client/src/iothub_client_ll.c \
    iothub_client/src/iothub_client_ll_uploadtoblob.c \
    iothub_client/src/iothub_message.c \
    iothub_client/src/iothubtransport.c \
    iothub_client/src/iothubtransporthttp.c \
    iothub_client/src/uamqp_messaging.c \
    iothub_client/src/version.c \
    parson/parson.c \
    uamqp/src/amqp_definitions.c \
    uamqp/src/amqp_frame_codec.c \
    uamqp/src/amqp_management.c \
    uamqp/src/amqpalloc.c \
    uamqp/src/amqpvalue.c \
    uamqp/src/amqpvalue_to_string.c \
    uamqp/src/cbs.c \
    uamqp/src/connection.c \
    uamqp/src/frame_codec.c \
    uamqp/src/header_detect_io.c \
    uamqp/src/link.c \
    uamqp/src/message.c \
    uamqp/src/message_receiver.c \
    uamqp/src/message_sender.c \
    uamqp/src/messaging.c \
    uamqp/src/sasl_anonymous.c \
    uamqp/src/sasl_frame_codec.c \
    uamqp/src/sasl_mechanism.c \
    uamqp/src/sasl_mssbcbs.c \
    uamqp/src/sasl_plain.c \
    uamqp/src/saslclientio.c \
    uamqp/src/session.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/aes.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/aria.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/asn1.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/base64.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/camellia.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/chacha.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/chacha20_poly1305.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_cbc.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_ccm.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_cfb.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_ctr.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_ecb.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_gcm.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/cipher_mode_ofb.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/des.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/des3.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/dh.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/dsa.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/ec.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/ec_curves.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/ecdh.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/ecdsa.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/hmac.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/idea.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/keccak.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/md2.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/md4.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/md5.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/mpi.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/oid.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/pem.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/pkcs5.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/poly1305.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/rc4.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/rc6.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/ripemd128.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/ripemd160.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/rsa.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/seed.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha1.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha224.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha256.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha384.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha3_224.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha3_256.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha3_384.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha3_512.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha512.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha512_224.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/sha512_256.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/tiger.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/whirlpool.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/x509.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_crypto/yarrow.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/ssl_common.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_cache.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_cipher_suites.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_client.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_common.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_io.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_misc.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_ssl/tls_record.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/ethernet.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/ip.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/net.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/net_mem.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/nic.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/ping.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/raw_socket.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/socket.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/tcp.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/tcp_fsm.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/tcp_misc.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/tcp_timer.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/core/udp.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dhcp/dhcp_client.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dhcp/dhcp_common.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dhcp/dhcp_debug.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dns/dns_cache.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dns/dns_client.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dns/dns_common.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/dns/dns_debug.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv4/arp.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv4/auto_ip.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv4/icmp.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv4/igmp.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv4/ipv4.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv4/ipv4_frag.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/icmpv6.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ipv6.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ipv6_frag.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ipv6_misc.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ipv6_pmtu.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ipv6_routing.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/mld.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ndp.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ndp_cache.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ndp_misc.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/ndp_router_adv.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/ipv6/slaac.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/netbios/nbns_client.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/netbios/nbns_common.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/netbios/nbns_responder.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/src/debug.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/src/ext_int_driver.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/src/rtc.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/src/startup_same70.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/src/syscalls.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/src/system_same70.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Demo/CORTEX_M7_SAME70_Xplained_AtmelStudio/src/ASF/common/utils/interrupt/interrupt_sam_nvic.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/croutine.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/event_groups.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/list.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/queue.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/tasks.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/timers.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1/port.c \
    c-utility/adapters/platform_freertos.c \
    c-utility/adapters/tickcounter_freertos.c \
    c-utility/adapters/agenttime.c \
    c-utility/adapters/threadapi_freertos.c \
    c-utility/adapters/httpapi_compact.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/common/os_port_freertos.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/common/endian.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/common/str.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/common/date_time.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/demo/common/atmel/boards/same70_xplained/sdram.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/drivers/ksz8081.c \
    /opt/CycloneTCP_SSL_Crypto_Open_1_7_4/cyclone_tcp/drivers/same70_eth.c \
    /opt/FreeRTOSv9.0.0/FreeRTOS/Source/portable/MemMang/heap_3.c \

OBJECTS_iothub_client_sample_http = $(patsubst %.c, obj/%.o, $(SOURCES_iothub_client_sample_http))

VPATH += \
    ../ \
    ../../../../ \

V ?= 0
Q_0 := @
Q_1 := 
Q = $(Q_$(V))

.SUFFIXES:
.PHONY: all clean

define loginfo
      $(Q)echo "[INFO] $1"
endef

define logerror
      $(Q)echo "[ERR ] $1"
endef

all: build

build: iothub_client_sample_http

iothub_client_sample_http: $(OBJECTS_iothub_client_sample_http) $(THIS_MAKEFILE)
	$(call loginfo,"Linking exec" $@)
	$(Q)$(CC) $(LDFLAGS) -Wl,-M=bin/$@.map $(CFLAGS) $(OBJECTS_iothub_client_sample_http) $(LIBS) -o bin/$@.elf
	$(Q)$(NM) bin/$@.elf >bin/$@.elf.txt
	$(Q)$(OBJCOPY) -O binary bin/$@.elf bin/$@.bin
	$(Q)$(SIZE) $(OBJECTS_iothub_client_sample_http) bin/$@.elf

$(OBJECTS): | bin obj

bin:
	$(Q)mkdir -p $@

obj:
	$(Q)mkdir -p $@

obj/%.o: %.c $(THIS_MAKEFILE)
	$(call loginfo,"Compiling file" $<)
	$(Q)mkdir -p $(dir $@)
	$(Q)$(CC) -MT $@ -MMD -MP -MF obj/$*.Td $(CFLAGS) -c $< -o $@
	$(Q)mv -f obj/$*.Td obj/$*.d

clean:
	$(Q)rm -R bin
	$(Q)rm -R obj

obj/%.d: ;
.PRECIOUS: obj/%.d

-include $(patsubst %,obj/%.d,$(basename $(SOURCES)))
