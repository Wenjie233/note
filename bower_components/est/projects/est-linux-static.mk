#
#   est-linux-static.mk -- Makefile to build Embethis EST for linux
#

NAME                  := est
VERSION               := 0.6.3
PROFILE               ?= static
ARCH                  ?= $(shell uname -m | sed 's/i.86/x86/;s/x86_64/x64/;s/arm.*/arm/;s/mips.*/mips/')
CC_ARCH               ?= $(shell echo $(ARCH) | sed 's/x86/i686/;s/x64/x86_64/')
OS                    ?= linux
CC                    ?= gcc
CONFIG                ?= $(OS)-$(ARCH)-$(PROFILE)
BUILD                 ?= build/$(CONFIG)
LBIN                  ?= $(BUILD)/bin
PATH                  := $(LBIN):$(PATH)

ME_COM_COMPILER       ?= 1
ME_COM_EST            ?= 1
ME_COM_LIB            ?= 1
ME_COM_OPENSSL        ?= 0
ME_COM_OSDEP          ?= 1
ME_COM_SSL            ?= 1
ME_COM_VXWORKS        ?= 0
ME_COM_WINSDK         ?= 1

ME_COM_OPENSSL_PATH   ?= "/usr"

ifeq ($(ME_COM_LIB),1)
    ME_COM_COMPILER := 1
endif
ifeq ($(ME_COM_OPENSSL),1)
    ME_COM_SSL := 1
endif

CFLAGS                += -fPIC -w
DFLAGS                += -D_REENTRANT -DPIC $(patsubst %,-D%,$(filter ME_%,$(MAKEFLAGS))) -DME_COM_COMPILER=$(ME_COM_COMPILER) -DME_COM_EST=$(ME_COM_EST) -DME_COM_LIB=$(ME_COM_LIB) -DME_COM_OPENSSL=$(ME_COM_OPENSSL) -DME_COM_OSDEP=$(ME_COM_OSDEP) -DME_COM_SSL=$(ME_COM_SSL) -DME_COM_VXWORKS=$(ME_COM_VXWORKS) -DME_COM_WINSDK=$(ME_COM_WINSDK) 
IFLAGS                += "-I$(BUILD)/inc"
LDFLAGS               += '-rdynamic' '-Wl,--enable-new-dtags' '-Wl,-rpath,$$ORIGIN/'
LIBPATHS              += -L$(BUILD)/bin
LIBS                  += -lrt -ldl -lpthread -lm

DEBUG                 ?= debug
CFLAGS-debug          ?= -g
DFLAGS-debug          ?= -DME_DEBUG
LDFLAGS-debug         ?= -g
DFLAGS-release        ?= 
CFLAGS-release        ?= -O2
LDFLAGS-release       ?= 
CFLAGS                += $(CFLAGS-$(DEBUG))
DFLAGS                += $(DFLAGS-$(DEBUG))
LDFLAGS               += $(LDFLAGS-$(DEBUG))

ME_ROOT_PREFIX        ?= 
ME_BASE_PREFIX        ?= $(ME_ROOT_PREFIX)/usr/local
ME_DATA_PREFIX        ?= $(ME_ROOT_PREFIX)/
ME_STATE_PREFIX       ?= $(ME_ROOT_PREFIX)/var
ME_APP_PREFIX         ?= $(ME_BASE_PREFIX)/lib/$(NAME)
ME_VAPP_PREFIX        ?= $(ME_APP_PREFIX)/$(VERSION)
ME_BIN_PREFIX         ?= $(ME_ROOT_PREFIX)/usr/local/bin
ME_INC_PREFIX         ?= $(ME_ROOT_PREFIX)/usr/local/include
ME_LIB_PREFIX         ?= $(ME_ROOT_PREFIX)/usr/local/lib
ME_MAN_PREFIX         ?= $(ME_ROOT_PREFIX)/usr/local/share/man
ME_SBIN_PREFIX        ?= $(ME_ROOT_PREFIX)/usr/local/sbin
ME_ETC_PREFIX         ?= $(ME_ROOT_PREFIX)/etc/$(NAME)
ME_WEB_PREFIX         ?= $(ME_ROOT_PREFIX)/var/www/$(NAME)
ME_LOG_PREFIX         ?= $(ME_ROOT_PREFIX)/var/log/$(NAME)
ME_SPOOL_PREFIX       ?= $(ME_ROOT_PREFIX)/var/spool/$(NAME)
ME_CACHE_PREFIX       ?= $(ME_ROOT_PREFIX)/var/spool/$(NAME)/cache
ME_SRC_PREFIX         ?= $(ME_ROOT_PREFIX)$(NAME)-$(VERSION)


ifeq ($(ME_COM_EST),1)
    TARGETS           += $(BUILD)/bin/libest.so
endif

unexport CDPATH

ifndef SHOW
.SILENT:
endif

all build compile: prep $(TARGETS)

.PHONY: prep

prep:
	@echo "      [Info] Use "make SHOW=1" to trace executed commands."
	@if [ "$(CONFIG)" = "" ] ; then echo WARNING: CONFIG not set ; exit 255 ; fi
	@if [ "$(ME_APP_PREFIX)" = "" ] ; then echo WARNING: ME_APP_PREFIX not set ; exit 255 ; fi
	@[ ! -x $(BUILD)/bin ] && mkdir -p $(BUILD)/bin; true
	@[ ! -x $(BUILD)/inc ] && mkdir -p $(BUILD)/inc; true
	@[ ! -x $(BUILD)/obj ] && mkdir -p $(BUILD)/obj; true
	@[ ! -f $(BUILD)/inc/me.h ] && cp projects/est-linux-static-me.h $(BUILD)/inc/me.h ; true
	@if ! diff $(BUILD)/inc/me.h projects/est-linux-static-me.h >/dev/null ; then\
		cp projects/est-linux-static-me.h $(BUILD)/inc/me.h  ; \
	fi; true
	@if [ -f "$(BUILD)/.makeflags" ] ; then \
		if [ "$(MAKEFLAGS)" != "`cat $(BUILD)/.makeflags`" ] ; then \
			echo "   [Warning] Make flags have changed since the last build: "`cat $(BUILD)/.makeflags`"" ; \
		fi ; \
	fi
	@echo "$(MAKEFLAGS)" >$(BUILD)/.makeflags

clean:
	rm -f "$(BUILD)/obj/aes.o"
	rm -f "$(BUILD)/obj/arc4.o"
	rm -f "$(BUILD)/obj/base64.o"
	rm -f "$(BUILD)/obj/bignum.o"
	rm -f "$(BUILD)/obj/camellia.o"
	rm -f "$(BUILD)/obj/certs.o"
	rm -f "$(BUILD)/obj/debug.o"
	rm -f "$(BUILD)/obj/des.o"
	rm -f "$(BUILD)/obj/dhm.o"
	rm -f "$(BUILD)/obj/havege.o"
	rm -f "$(BUILD)/obj/md2.o"
	rm -f "$(BUILD)/obj/md4.o"
	rm -f "$(BUILD)/obj/md5.o"
	rm -f "$(BUILD)/obj/net.o"
	rm -f "$(BUILD)/obj/padlock.o"
	rm -f "$(BUILD)/obj/rsa.o"
	rm -f "$(BUILD)/obj/sha1.o"
	rm -f "$(BUILD)/obj/sha2.o"
	rm -f "$(BUILD)/obj/sha4.o"
	rm -f "$(BUILD)/obj/ssl_cli.o"
	rm -f "$(BUILD)/obj/ssl_srv.o"
	rm -f "$(BUILD)/obj/ssl_tls.o"
	rm -f "$(BUILD)/obj/timing.o"
	rm -f "$(BUILD)/obj/x509parse.o"
	rm -f "$(BUILD)/obj/xtea.o"
	rm -f "$(BUILD)/bin/libest.so"

clobber: clean
	rm -fr ./$(BUILD)

#
#   aes.h
#
DEPS_1 += src/aes.h

$(BUILD)/inc/aes.h: $(DEPS_1)
	@echo '      [Copy] $(BUILD)/inc/aes.h'
	mkdir -p "$(BUILD)/inc"
	cp src/aes.h $(BUILD)/inc/aes.h

#
#   arc4.h
#
DEPS_2 += src/arc4.h

$(BUILD)/inc/arc4.h: $(DEPS_2)
	@echo '      [Copy] $(BUILD)/inc/arc4.h'
	mkdir -p "$(BUILD)/inc"
	cp src/arc4.h $(BUILD)/inc/arc4.h

#
#   base64.h
#
DEPS_3 += src/base64.h

$(BUILD)/inc/base64.h: $(DEPS_3)
	@echo '      [Copy] $(BUILD)/inc/base64.h'
	mkdir -p "$(BUILD)/inc"
	cp src/base64.h $(BUILD)/inc/base64.h

#
#   bignum.h
#
DEPS_4 += src/bignum.h

$(BUILD)/inc/bignum.h: $(DEPS_4)
	@echo '      [Copy] $(BUILD)/inc/bignum.h'
	mkdir -p "$(BUILD)/inc"
	cp src/bignum.h $(BUILD)/inc/bignum.h

#
#   bn_mul.h
#
DEPS_5 += src/bn_mul.h

$(BUILD)/inc/bn_mul.h: $(DEPS_5)
	@echo '      [Copy] $(BUILD)/inc/bn_mul.h'
	mkdir -p "$(BUILD)/inc"
	cp src/bn_mul.h $(BUILD)/inc/bn_mul.h

#
#   camellia.h
#
DEPS_6 += src/camellia.h

$(BUILD)/inc/camellia.h: $(DEPS_6)
	@echo '      [Copy] $(BUILD)/inc/camellia.h'
	mkdir -p "$(BUILD)/inc"
	cp src/camellia.h $(BUILD)/inc/camellia.h

#
#   certs.h
#
DEPS_7 += src/certs.h

$(BUILD)/inc/certs.h: $(DEPS_7)
	@echo '      [Copy] $(BUILD)/inc/certs.h'
	mkdir -p "$(BUILD)/inc"
	cp src/certs.h $(BUILD)/inc/certs.h

#
#   debug.h
#
DEPS_8 += src/debug.h

$(BUILD)/inc/debug.h: $(DEPS_8)
	@echo '      [Copy] $(BUILD)/inc/debug.h'
	mkdir -p "$(BUILD)/inc"
	cp src/debug.h $(BUILD)/inc/debug.h

#
#   des.h
#
DEPS_9 += src/des.h

$(BUILD)/inc/des.h: $(DEPS_9)
	@echo '      [Copy] $(BUILD)/inc/des.h'
	mkdir -p "$(BUILD)/inc"
	cp src/des.h $(BUILD)/inc/des.h

#
#   dhm.h
#
DEPS_10 += src/dhm.h

$(BUILD)/inc/dhm.h: $(DEPS_10)
	@echo '      [Copy] $(BUILD)/inc/dhm.h'
	mkdir -p "$(BUILD)/inc"
	cp src/dhm.h $(BUILD)/inc/dhm.h

#
#   me.h
#

$(BUILD)/inc/me.h: $(DEPS_11)

#
#   osdep.h
#
DEPS_12 += src/osdep/osdep.h
DEPS_12 += $(BUILD)/inc/me.h

$(BUILD)/inc/osdep.h: $(DEPS_12)
	@echo '      [Copy] $(BUILD)/inc/osdep.h'
	mkdir -p "$(BUILD)/inc"
	cp src/osdep/osdep.h $(BUILD)/inc/osdep.h

#
#   net.h
#
DEPS_13 += src/net.h

$(BUILD)/inc/net.h: $(DEPS_13)
	@echo '      [Copy] $(BUILD)/inc/net.h'
	mkdir -p "$(BUILD)/inc"
	cp src/net.h $(BUILD)/inc/net.h

#
#   rsa.h
#
DEPS_14 += src/rsa.h

$(BUILD)/inc/rsa.h: $(DEPS_14)
	@echo '      [Copy] $(BUILD)/inc/rsa.h'
	mkdir -p "$(BUILD)/inc"
	cp src/rsa.h $(BUILD)/inc/rsa.h

#
#   md5.h
#
DEPS_15 += src/md5.h

$(BUILD)/inc/md5.h: $(DEPS_15)
	@echo '      [Copy] $(BUILD)/inc/md5.h'
	mkdir -p "$(BUILD)/inc"
	cp src/md5.h $(BUILD)/inc/md5.h

#
#   sha1.h
#
DEPS_16 += src/sha1.h

$(BUILD)/inc/sha1.h: $(DEPS_16)
	@echo '      [Copy] $(BUILD)/inc/sha1.h'
	mkdir -p "$(BUILD)/inc"
	cp src/sha1.h $(BUILD)/inc/sha1.h

#
#   x509.h
#
DEPS_17 += src/x509.h

$(BUILD)/inc/x509.h: $(DEPS_17)
	@echo '      [Copy] $(BUILD)/inc/x509.h'
	mkdir -p "$(BUILD)/inc"
	cp src/x509.h $(BUILD)/inc/x509.h

#
#   ssl.h
#
DEPS_18 += src/ssl.h

$(BUILD)/inc/ssl.h: $(DEPS_18)
	@echo '      [Copy] $(BUILD)/inc/ssl.h'
	mkdir -p "$(BUILD)/inc"
	cp src/ssl.h $(BUILD)/inc/ssl.h

#
#   havege.h
#
DEPS_19 += src/havege.h

$(BUILD)/inc/havege.h: $(DEPS_19)
	@echo '      [Copy] $(BUILD)/inc/havege.h'
	mkdir -p "$(BUILD)/inc"
	cp src/havege.h $(BUILD)/inc/havege.h

#
#   md2.h
#
DEPS_20 += src/md2.h

$(BUILD)/inc/md2.h: $(DEPS_20)
	@echo '      [Copy] $(BUILD)/inc/md2.h'
	mkdir -p "$(BUILD)/inc"
	cp src/md2.h $(BUILD)/inc/md2.h

#
#   md4.h
#
DEPS_21 += src/md4.h

$(BUILD)/inc/md4.h: $(DEPS_21)
	@echo '      [Copy] $(BUILD)/inc/md4.h'
	mkdir -p "$(BUILD)/inc"
	cp src/md4.h $(BUILD)/inc/md4.h

#
#   padlock.h
#
DEPS_22 += src/padlock.h

$(BUILD)/inc/padlock.h: $(DEPS_22)
	@echo '      [Copy] $(BUILD)/inc/padlock.h'
	mkdir -p "$(BUILD)/inc"
	cp src/padlock.h $(BUILD)/inc/padlock.h

#
#   sha2.h
#
DEPS_23 += src/sha2.h

$(BUILD)/inc/sha2.h: $(DEPS_23)
	@echo '      [Copy] $(BUILD)/inc/sha2.h'
	mkdir -p "$(BUILD)/inc"
	cp src/sha2.h $(BUILD)/inc/sha2.h

#
#   sha4.h
#
DEPS_24 += src/sha4.h

$(BUILD)/inc/sha4.h: $(DEPS_24)
	@echo '      [Copy] $(BUILD)/inc/sha4.h'
	mkdir -p "$(BUILD)/inc"
	cp src/sha4.h $(BUILD)/inc/sha4.h

#
#   timing.h
#
DEPS_25 += src/timing.h

$(BUILD)/inc/timing.h: $(DEPS_25)
	@echo '      [Copy] $(BUILD)/inc/timing.h'
	mkdir -p "$(BUILD)/inc"
	cp src/timing.h $(BUILD)/inc/timing.h

#
#   xtea.h
#
DEPS_26 += src/xtea.h

$(BUILD)/inc/xtea.h: $(DEPS_26)
	@echo '      [Copy] $(BUILD)/inc/xtea.h'
	mkdir -p "$(BUILD)/inc"
	cp src/xtea.h $(BUILD)/inc/xtea.h

#
#   est.h
#
DEPS_27 += src/est.h
DEPS_27 += $(BUILD)/inc/me.h
DEPS_27 += $(BUILD)/inc/osdep.h
DEPS_27 += $(BUILD)/inc/bignum.h
DEPS_27 += $(BUILD)/inc/net.h
DEPS_27 += $(BUILD)/inc/dhm.h
DEPS_27 += $(BUILD)/inc/rsa.h
DEPS_27 += $(BUILD)/inc/md5.h
DEPS_27 += $(BUILD)/inc/sha1.h
DEPS_27 += $(BUILD)/inc/x509.h
DEPS_27 += $(BUILD)/inc/ssl.h
DEPS_27 += $(BUILD)/inc/aes.h
DEPS_27 += $(BUILD)/inc/arc4.h
DEPS_27 += $(BUILD)/inc/base64.h
DEPS_27 += $(BUILD)/inc/bn_mul.h
DEPS_27 += $(BUILD)/inc/camellia.h
DEPS_27 += $(BUILD)/inc/certs.h
DEPS_27 += $(BUILD)/inc/debug.h
DEPS_27 += $(BUILD)/inc/des.h
DEPS_27 += $(BUILD)/inc/havege.h
DEPS_27 += $(BUILD)/inc/md2.h
DEPS_27 += $(BUILD)/inc/md4.h
DEPS_27 += $(BUILD)/inc/padlock.h
DEPS_27 += $(BUILD)/inc/sha2.h
DEPS_27 += $(BUILD)/inc/sha4.h
DEPS_27 += $(BUILD)/inc/timing.h
DEPS_27 += $(BUILD)/inc/xtea.h

$(BUILD)/inc/est.h: $(DEPS_27)
	@echo '      [Copy] $(BUILD)/inc/est.h'
	mkdir -p "$(BUILD)/inc"
	cp src/est.h $(BUILD)/inc/est.h

#
#   openssl.h
#
DEPS_28 += src/openssl.h

$(BUILD)/inc/openssl.h: $(DEPS_28)
	@echo '      [Copy] $(BUILD)/inc/openssl.h'
	mkdir -p "$(BUILD)/inc"
	cp src/openssl.h $(BUILD)/inc/openssl.h

#
#   aes.o
#
DEPS_29 += $(BUILD)/inc/est.h

$(BUILD)/obj/aes.o: \
    src/aes.c $(DEPS_29)
	@echo '   [Compile] $(BUILD)/obj/aes.o'
	$(CC) -c -o $(BUILD)/obj/aes.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/aes.c

#
#   arc4.o
#
DEPS_30 += $(BUILD)/inc/est.h

$(BUILD)/obj/arc4.o: \
    src/arc4.c $(DEPS_30)
	@echo '   [Compile] $(BUILD)/obj/arc4.o'
	$(CC) -c -o $(BUILD)/obj/arc4.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/arc4.c

#
#   base64.o
#
DEPS_31 += $(BUILD)/inc/est.h

$(BUILD)/obj/base64.o: \
    src/base64.c $(DEPS_31)
	@echo '   [Compile] $(BUILD)/obj/base64.o'
	$(CC) -c -o $(BUILD)/obj/base64.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/base64.c

#
#   bignum.o
#
DEPS_32 += $(BUILD)/inc/est.h

$(BUILD)/obj/bignum.o: \
    src/bignum.c $(DEPS_32)
	@echo '   [Compile] $(BUILD)/obj/bignum.o'
	$(CC) -c -o $(BUILD)/obj/bignum.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/bignum.c

#
#   camellia.o
#
DEPS_33 += $(BUILD)/inc/est.h

$(BUILD)/obj/camellia.o: \
    src/camellia.c $(DEPS_33)
	@echo '   [Compile] $(BUILD)/obj/camellia.o'
	$(CC) -c -o $(BUILD)/obj/camellia.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/camellia.c

#
#   certs.o
#
DEPS_34 += $(BUILD)/inc/est.h

$(BUILD)/obj/certs.o: \
    src/certs.c $(DEPS_34)
	@echo '   [Compile] $(BUILD)/obj/certs.o'
	$(CC) -c -o $(BUILD)/obj/certs.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/certs.c

#
#   debug.o
#
DEPS_35 += $(BUILD)/inc/est.h

$(BUILD)/obj/debug.o: \
    src/debug.c $(DEPS_35)
	@echo '   [Compile] $(BUILD)/obj/debug.o'
	$(CC) -c -o $(BUILD)/obj/debug.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/debug.c

#
#   des.o
#
DEPS_36 += $(BUILD)/inc/est.h

$(BUILD)/obj/des.o: \
    src/des.c $(DEPS_36)
	@echo '   [Compile] $(BUILD)/obj/des.o'
	$(CC) -c -o $(BUILD)/obj/des.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/des.c

#
#   dhm.o
#
DEPS_37 += $(BUILD)/inc/est.h

$(BUILD)/obj/dhm.o: \
    src/dhm.c $(DEPS_37)
	@echo '   [Compile] $(BUILD)/obj/dhm.o'
	$(CC) -c -o $(BUILD)/obj/dhm.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/dhm.c

#
#   havege.o
#
DEPS_38 += $(BUILD)/inc/est.h

$(BUILD)/obj/havege.o: \
    src/havege.c $(DEPS_38)
	@echo '   [Compile] $(BUILD)/obj/havege.o'
	$(CC) -c -o $(BUILD)/obj/havege.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/havege.c

#
#   md2.o
#
DEPS_39 += $(BUILD)/inc/est.h

$(BUILD)/obj/md2.o: \
    src/md2.c $(DEPS_39)
	@echo '   [Compile] $(BUILD)/obj/md2.o'
	$(CC) -c -o $(BUILD)/obj/md2.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/md2.c

#
#   md4.o
#
DEPS_40 += $(BUILD)/inc/est.h

$(BUILD)/obj/md4.o: \
    src/md4.c $(DEPS_40)
	@echo '   [Compile] $(BUILD)/obj/md4.o'
	$(CC) -c -o $(BUILD)/obj/md4.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/md4.c

#
#   md5.o
#
DEPS_41 += $(BUILD)/inc/est.h

$(BUILD)/obj/md5.o: \
    src/md5.c $(DEPS_41)
	@echo '   [Compile] $(BUILD)/obj/md5.o'
	$(CC) -c -o $(BUILD)/obj/md5.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/md5.c

#
#   net.o
#
DEPS_42 += $(BUILD)/inc/est.h

$(BUILD)/obj/net.o: \
    src/net.c $(DEPS_42)
	@echo '   [Compile] $(BUILD)/obj/net.o'
	$(CC) -c -o $(BUILD)/obj/net.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/net.c

#
#   padlock.o
#
DEPS_43 += $(BUILD)/inc/est.h

$(BUILD)/obj/padlock.o: \
    src/padlock.c $(DEPS_43)
	@echo '   [Compile] $(BUILD)/obj/padlock.o'
	$(CC) -c -o $(BUILD)/obj/padlock.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/padlock.c

#
#   rsa.o
#
DEPS_44 += $(BUILD)/inc/est.h

$(BUILD)/obj/rsa.o: \
    src/rsa.c $(DEPS_44)
	@echo '   [Compile] $(BUILD)/obj/rsa.o'
	$(CC) -c -o $(BUILD)/obj/rsa.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/rsa.c

#
#   sha1.o
#
DEPS_45 += $(BUILD)/inc/est.h

$(BUILD)/obj/sha1.o: \
    src/sha1.c $(DEPS_45)
	@echo '   [Compile] $(BUILD)/obj/sha1.o'
	$(CC) -c -o $(BUILD)/obj/sha1.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/sha1.c

#
#   sha2.o
#
DEPS_46 += $(BUILD)/inc/est.h

$(BUILD)/obj/sha2.o: \
    src/sha2.c $(DEPS_46)
	@echo '   [Compile] $(BUILD)/obj/sha2.o'
	$(CC) -c -o $(BUILD)/obj/sha2.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/sha2.c

#
#   sha4.o
#
DEPS_47 += $(BUILD)/inc/est.h

$(BUILD)/obj/sha4.o: \
    src/sha4.c $(DEPS_47)
	@echo '   [Compile] $(BUILD)/obj/sha4.o'
	$(CC) -c -o $(BUILD)/obj/sha4.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/sha4.c

#
#   ssl_cli.o
#
DEPS_48 += $(BUILD)/inc/est.h

$(BUILD)/obj/ssl_cli.o: \
    src/ssl_cli.c $(DEPS_48)
	@echo '   [Compile] $(BUILD)/obj/ssl_cli.o'
	$(CC) -c -o $(BUILD)/obj/ssl_cli.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/ssl_cli.c

#
#   ssl_srv.o
#
DEPS_49 += $(BUILD)/inc/est.h

$(BUILD)/obj/ssl_srv.o: \
    src/ssl_srv.c $(DEPS_49)
	@echo '   [Compile] $(BUILD)/obj/ssl_srv.o'
	$(CC) -c -o $(BUILD)/obj/ssl_srv.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/ssl_srv.c

#
#   ssl_tls.o
#
DEPS_50 += $(BUILD)/inc/est.h

$(BUILD)/obj/ssl_tls.o: \
    src/ssl_tls.c $(DEPS_50)
	@echo '   [Compile] $(BUILD)/obj/ssl_tls.o'
	$(CC) -c -o $(BUILD)/obj/ssl_tls.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/ssl_tls.c

#
#   timing.o
#
DEPS_51 += $(BUILD)/inc/est.h

$(BUILD)/obj/timing.o: \
    src/timing.c $(DEPS_51)
	@echo '   [Compile] $(BUILD)/obj/timing.o'
	$(CC) -c -o $(BUILD)/obj/timing.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/timing.c

#
#   x509parse.o
#
DEPS_52 += $(BUILD)/inc/est.h

$(BUILD)/obj/x509parse.o: \
    src/x509parse.c $(DEPS_52)
	@echo '   [Compile] $(BUILD)/obj/x509parse.o'
	$(CC) -c -o $(BUILD)/obj/x509parse.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/x509parse.c

#
#   xtea.o
#
DEPS_53 += $(BUILD)/inc/est.h

$(BUILD)/obj/xtea.o: \
    src/xtea.c $(DEPS_53)
	@echo '   [Compile] $(BUILD)/obj/xtea.o'
	$(CC) -c -o $(BUILD)/obj/xtea.o $(CFLAGS) $(DFLAGS) $(IFLAGS) src/xtea.c

ifeq ($(ME_COM_EST),1)
#
#   libest
#
DEPS_54 += $(BUILD)/inc/osdep.h
DEPS_54 += $(BUILD)/inc/aes.h
DEPS_54 += $(BUILD)/inc/arc4.h
DEPS_54 += $(BUILD)/inc/base64.h
DEPS_54 += $(BUILD)/inc/bignum.h
DEPS_54 += $(BUILD)/inc/bn_mul.h
DEPS_54 += $(BUILD)/inc/camellia.h
DEPS_54 += $(BUILD)/inc/certs.h
DEPS_54 += $(BUILD)/inc/debug.h
DEPS_54 += $(BUILD)/inc/des.h
DEPS_54 += $(BUILD)/inc/dhm.h
DEPS_54 += $(BUILD)/inc/est.h
DEPS_54 += $(BUILD)/inc/havege.h
DEPS_54 += $(BUILD)/inc/md2.h
DEPS_54 += $(BUILD)/inc/md4.h
DEPS_54 += $(BUILD)/inc/md5.h
DEPS_54 += $(BUILD)/inc/net.h
DEPS_54 += $(BUILD)/inc/openssl.h
DEPS_54 += $(BUILD)/inc/padlock.h
DEPS_54 += $(BUILD)/inc/rsa.h
DEPS_54 += $(BUILD)/inc/sha1.h
DEPS_54 += $(BUILD)/inc/sha2.h
DEPS_54 += $(BUILD)/inc/sha4.h
DEPS_54 += $(BUILD)/inc/ssl.h
DEPS_54 += $(BUILD)/inc/timing.h
DEPS_54 += $(BUILD)/inc/x509.h
DEPS_54 += $(BUILD)/inc/xtea.h
DEPS_54 += $(BUILD)/obj/aes.o
DEPS_54 += $(BUILD)/obj/arc4.o
DEPS_54 += $(BUILD)/obj/base64.o
DEPS_54 += $(BUILD)/obj/bignum.o
DEPS_54 += $(BUILD)/obj/camellia.o
DEPS_54 += $(BUILD)/obj/certs.o
DEPS_54 += $(BUILD)/obj/debug.o
DEPS_54 += $(BUILD)/obj/des.o
DEPS_54 += $(BUILD)/obj/dhm.o
DEPS_54 += $(BUILD)/obj/havege.o
DEPS_54 += $(BUILD)/obj/md2.o
DEPS_54 += $(BUILD)/obj/md4.o
DEPS_54 += $(BUILD)/obj/md5.o
DEPS_54 += $(BUILD)/obj/net.o
DEPS_54 += $(BUILD)/obj/padlock.o
DEPS_54 += $(BUILD)/obj/rsa.o
DEPS_54 += $(BUILD)/obj/sha1.o
DEPS_54 += $(BUILD)/obj/sha2.o
DEPS_54 += $(BUILD)/obj/sha4.o
DEPS_54 += $(BUILD)/obj/ssl_cli.o
DEPS_54 += $(BUILD)/obj/ssl_srv.o
DEPS_54 += $(BUILD)/obj/ssl_tls.o
DEPS_54 += $(BUILD)/obj/timing.o
DEPS_54 += $(BUILD)/obj/x509parse.o
DEPS_54 += $(BUILD)/obj/xtea.o

$(BUILD)/bin/libest.so: $(DEPS_54)
	@echo '      [Link] $(BUILD)/bin/libest.so'
	$(CC) -shared -o $(BUILD)/bin/libest.so $(LDFLAGS) $(LIBPATHS) "$(BUILD)/obj/aes.o" "$(BUILD)/obj/arc4.o" "$(BUILD)/obj/base64.o" "$(BUILD)/obj/bignum.o" "$(BUILD)/obj/camellia.o" "$(BUILD)/obj/certs.o" "$(BUILD)/obj/debug.o" "$(BUILD)/obj/des.o" "$(BUILD)/obj/dhm.o" "$(BUILD)/obj/havege.o" "$(BUILD)/obj/md2.o" "$(BUILD)/obj/md4.o" "$(BUILD)/obj/md5.o" "$(BUILD)/obj/net.o" "$(BUILD)/obj/padlock.o" "$(BUILD)/obj/rsa.o" "$(BUILD)/obj/sha1.o" "$(BUILD)/obj/sha2.o" "$(BUILD)/obj/sha4.o" "$(BUILD)/obj/ssl_cli.o" "$(BUILD)/obj/ssl_srv.o" "$(BUILD)/obj/ssl_tls.o" "$(BUILD)/obj/timing.o" "$(BUILD)/obj/x509parse.o" "$(BUILD)/obj/xtea.o" $(LIBS) 
endif

#
#   installPrep
#

installPrep: $(DEPS_55)
	if [ "`id -u`" != 0 ] ; \
	then echo "Must run as root. Rerun with "sudo"" ; \
	exit 255 ; \
	fi

#
#   stop
#

stop: $(DEPS_56)

#
#   installBinary
#

installBinary: $(DEPS_57)

#
#   start
#

start: $(DEPS_58)

#
#   install
#
DEPS_59 += installPrep
DEPS_59 += stop
DEPS_59 += installBinary
DEPS_59 += start

install: $(DEPS_59)

#
#   uninstall
#
DEPS_60 += stop

uninstall: $(DEPS_60)

#
#   version
#

version: $(DEPS_61)
	echo 0.6.3

