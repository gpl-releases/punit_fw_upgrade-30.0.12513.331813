#
#  This file is provided under a dual BSD/GPLv2 license.  When using or
#  redistributing this file, you may do so under either license.
#
#  GPL LICENSE SUMMARY
#
#  Copyright(c) 2008-2012 Intel Corporation. All rights reserved.
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of version 2 of the GNU General Public License as
#  published by the Free Software Foundation.
#
#  This program is distributed in the hope that it will be useful, but
#  WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#  The full GNU General Public License is included in this distribution
#  in the file called LICENSE.GPL.
#
#  Contact Information:
#    Intel Corporation
#    2200 Mission College Blvd.
#    Santa Clara, CA  97052
#
#
#  BSD LICENSE
#
#  Copyright(c) 2008-2012 Intel Corporation. All rights reserved.
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in
#      the documentation and/or other materials provided with the
#      distribution.
#    * Neither the name of Intel Corporation nor the names of its
#      contributors may be used to endorse or promote products derived
#      from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
#  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
#  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
#  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
#  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
#  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
#  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

ifndef COM_VER1
COM_VER1=9
COM_VER2=9
COM_VER3=9
COM_VER4=9
COM_VER=<Unofficial Build>
VERSION_NUMBER=<Unofficial Build>
endif

ONLY_KERNEL_MODE=1

export PWD := $(shell pwd)
ifndef PUNIT_FW_UPGRADE_BASE
export PUNIT_FW_UPGRADE_BASE := $(PWD)
endif

BASE_DIR=$(PUNIT_FW_UPGRADE_BASE)

EXTAR_CFLAGS += -I./
EXTRA_CFLAGS += -I$(BASE_DIR)/include/
EXTRA_CFLAGS += -I$(BUILD_DEST)/usr/include/
EXTRA_CFLAGS += -I$(BUILD_DEST)/include/
EXTRA_CFLAGS += -I$(BUILD_DEST)/include/linux/
EXTRA_CFLAGS += -DLINUX -DCOM_VER1=$(COM_VER1) -DCOM_VER2=$(COM_VER2) -DCOM_VER3=$(COM_VER3) -DCOM_VER4=$(COM_VER4) -DCOM_VER="\"$(COM_VER)\""
EXTRA_CFLAGS += -DLINUX_DRIVER -fno-common

CFLAGS_PP = -E -C -P 

MOD_NAME=punit_fw_upgrade_drv

ifneq ($(KERNELRELEASE),)	
punit_fw_upgrade_drv-objs := punit_fw_upgrade.o \
                        punit_sys_info.o \
                        _ce2600_upgrade_ops.o \
                        _ce5300_upgrade_ops.o
obj-m := punit_fw_upgrade_drv.o
empty :=
space := $(empty) $(empty)
KERNEL_VER_NUM:=$(lastword $(subst .,$(space),$(KERNEL_VER)))
ifeq ($(KERNEL_VER_NUM),23)
    $(shell cat $(BUILD_DEST)/kernel/module.symvers/* > ./Module.symvers)
else
    KBUILD_EXTRA_SYMBOLS := $(wildcard $(BUILD_DEST)/kernel/module.symvers/*)
endif
else
KERNELDIR ?= $(KERNEL_BUILD_DIR)
PWD       := $(shell pwd)

module: 
	@echo ">>> Build P-Unit FW upgrade kernel module"
	@-rm $(BUILD_DEST)/kernel/module.symvers/$(MODULE_NAME) 2> /dev/null
	$(MAKE) -C $(KERNEL_BUILD_DIR) SUBDIRS=$(PWD) modules
	@if [ ! -d $(BUILD_DEST)/kernel/module.symvers/ ]; then \
		mkdir $(BUILD_DEST)/kernel/module.symvers/ ; \
	fi
	@cp Module.symvers $(BUILD_DEST)/kernel/module.symvers/$(MODULE_NAME)


.PHONY: all  clean module
all: module

clean :
	rm -rf *.o *~ core .depend .*.cmd *.ko *.mod.c .tmp_versions *.symvers *.order .version

endif
