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

export BUILD_ROOT ?= $(PWD)/..
export BUILD_DEST  ?= $(BUILD_ROOT)/i686-linux-elf

export PWD := $(shell pwd)
ifndef PUNIT_FW_UPGRADE_BASE
export PUNIT_FW_UPGRADE_BASE := $(PWD)
endif

include Makefile.inc

COMPONENT_NAME=punit_fw_upgrade

SUBDIRS = driver 

.PHONY:all
all: bld readme_copy

.PHONY:bld
bld:
	@echo ">>>Building P-Unit FW upgrade Module"
	@$(foreach SUBDIR, $(SUBDIRS), ($(MAKE) -C $(SUBDIR) -f Makefile)&&) exit 0;

.PHONY: readme_copy  
readme_copy:
	if [ -e README_pal.txt ]; then cp -f README_pal.txt $(BUILD_DEST); fi 

.PHONY: debug
debug: dbld readme_copy

.PHONY:dbld
dbld:
	@echo ">>>Building P-Unit FW upgrade DEBUG Module"
	@$(foreach SUBDIR, $(SUBDIRS), ($(MAKE) -C $(SUBDIR) -f Makefile -e DEBUG=1)&&) exit 0; 
	
.PHONY:clean
clean:
	@echo ">>>Cleaning P-Unit FW upgrade Module"
	@$(foreach SUBDIR, $(SUBDIRS), ($(MAKE) -C $(SUBDIR) -f Makefile clean) &&) exit 0;


.PHONY:install
install: install_dev install_target
	@echo ">>>Installed development and target files"

.PHONY:install_dev
install_dev: all
	@echo ">>>Copying all development files to $(BUILD_DEST)"
	mkdir -p $(BUILD_DEST)/lib/modules
	cp -f driver/punit_fw_upgrade_drv.ko $(BUILD_DEST)/lib/modules
	
.PHONY:install_target
install_target: all
	@echo ">>>Copying all target files to $(FSROOT)"
	mkdir -p $(FSROOT)/lib/modules
	cp -f driver/punit_fw_upgrade_drv.ko $(FSROOT)/lib/modules 
	if [ -e init_punit_fw_upgrade ]; then mkdir -p $(FSROOT)/etc/init.d/; cp -f init_punit_fw_upgrade $(FSROOT)/etc/init.d/init_punit_fw_upgrade; fi
	if [ -e bin/ce2600_8051_firmware.bin ]; then mkdir -p $(FSROOT)/lib/firmware/punit/; cp -f bin/ce2600_8051_firmware.bin $(FSROOT)/lib/firmware/punit/ce2600_8051_firmware.bin; fi

.PHONY: test
test:
	@echo ">>>Do nothing"

.PHONY: uninstall
uninstall: uninstall_dev uninstall_target
	@echo ">>>Uninstalled development and target files"

.PHONY: uninstall_dev
uninstall_dev:
	@echo ">>>Removing all development files from $(BUILD_DEST)"
	rm -f $(BUILD_DEST)/lib/modules/punit_fw_upgrade_drv.ko
	
.PHONY: uninstall_target
uninstall_target:
	@echo ">>>Removing all target files from $(FSROOT)"
	rm -f $(FSROOT)/lib/modules/punit_fw_upgrade_drv.ko 
	rm -f $(FSROOT)/lib/firmware/punit/ce2600_8051_firmware.bin 

