/*
#
#  This file is provided under a GPLv2 license.  When using or
#  redistributing this file, you may do so under either license.
#
#  GPL LICENSE SUMMARY
#
#  Copyright(c) 2010-2012 Intel Corporation. All rights reserved.
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
#  intel.com
#  Intel Corporation
#  2200 Mission College Blvd.
#  Santa Clara, CA  95052
#  USA
#  (408) 765-8080
#
#
#*/
 

/*------------------------------------------------------------------------------
 * File Name:punit_fw_upgrade.h
 *------------------------------------------------------------------------------
 */

#ifndef _PUNIT_FW_UPGRADE_H
#define _PUNIT_FW_UPGRADE_H
#include <linux/pci.h>
#include <linux/firmware.h>
#include <linux/kobject.h>
#include "iosf.h"
#include "pal.h"


#define PUNIT_MEM_SIZE (64*1024)
#define PUNIT_MEM_ALIGN (64*1024)

typedef enum {
	RET_OK = 0x0,
	RET_INVALID_PARAM,
	RET_NOT_SUPPORTED_PLATFORM,
	RET_NOT_SUPPORTED_OPS,
	RET_NOMEM_ERR,
	RET_IOSF_ERR,
	RET_PAL_ERR,
	RET_FW_REQ_ERR,
	RET_FW_BIN_ERR,
	RET_OTHER_ERR
}punit_ret_result_t;

typedef struct platform_info {
        pal_soc_info_t soc_info;
        iosf_handle handle;
        uint32_t fw_version;
        punit_ret_result_t (*_get_running_fw_version)(struct platform_info *platform_info);
        punit_ret_result_t (*_is_need_upgrade)(struct firmware **pfw_entry,  struct platform_info *platform_info);
        punit_ret_result_t (*_upgrade_firmware)(struct firmware **pfw_entry, struct platform_info *platform_info);
}platform_info_t;
 
typedef struct fw_info {
        struct kobject fw_info_kobj;
        struct firmware *fw_entry;
        void* fw_mem_addr;
        uint32_t fw_version;
        bool flash_need_upgrade;
	void *platform_priv;
}fw_info_t;

punit_ret_result_t _upgrade_firmware(struct firmware **pfw_entry, platform_info_t *platform_info);
punit_ret_result_t _is_need_upgrade(struct firmware **pfw_entry, platform_info_t *platform_info);
punit_ret_result_t _get_running_fw_version(platform_info_t *platform_info);

#endif /* _PUNIT_FW_UPGRADE_H */
