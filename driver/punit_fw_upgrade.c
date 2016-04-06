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
 * File Name: punit_fw_upgrade.c
 *------------------------------------------------------------------------------
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/slab.h>

#include <linux/firmware.h>
#include <linux/pci.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/platform_device.h>

#include <asm/cacheflush.h>

#include "pal.h"
#include "iosf.h"

#include "punit_fw_upgrade.h"
#include "_ce5300_upgrade_ops.h"
#include "_ce2600_upgrade_ops.h"
#include "punit_sys_info.h"

static char *fw_name="/lib/firmware/punit/ce2600_8051_firmware.bin";
module_param(fw_name, charp, S_IRUGO);

static int time_out=0x10000;
module_param(time_out, int, S_IRUGO);

static platform_info_t _platform_info;
static fw_info_t _fw_info;

punit_ret_result_t _get_running_fw_version(platform_info_t *platform_info)
{
	if(iosf_read32(platform_info->handle, 0x04,0x87, &platform_info->fw_version) != IOSF_OK)
		return RET_IOSF_ERR;
	return RET_OK;
}

punit_ret_result_t _upgrade_firmware(struct firmware **pfw_entry, platform_info_t *platform_info)
{
	uint32_t val, fw_addr;
	uint32_t local_time_out = time_out;
    	struct firmware *fw = *pfw_entry;
	fw_info_t *fw_info = container_of(pfw_entry, fw_info_t, fw_entry);
    	iosf_handle handle = platform_info->handle;

	/*
	* P-Unit needs PUNIT_MEM_SIZE (64KB) physically continued and 64KB aligned memory 	
	*/
	fw_info->fw_mem_addr = (unsigned int)kmalloc(2*PUNIT_MEM_SIZE, GFP_KERNEL);
	if (fw_info->fw_mem_addr == 0)
		return RET_NOMEM_ERR;
    	fw_addr = (uint32_t)fw_info->fw_mem_addr&~(PUNIT_MEM_ALIGN-1);

	memcpy((void *)fw_addr, fw->data, fw->size);
	/*
	* flush the firmware image to DRAM is neeed.
	* otherwise P-Unit can't get the the correct firmware image
	*/
	clflush_cache_range((void *)fw_addr, PUNIT_MEM_SIZE);

	if(iosf_msg_data(handle,0x04,0x79,(uint32_t)virt_to_phys((void*)fw_addr)) != IOSF_OK)
		goto IOSF_ERROR;
	if(iosf_modify(handle, 0x04, 0x71, (1<<24), (1<<24)) != IOSF_OK)
		goto IOSF_ERROR;
 
    	/* wait for 8051 to acknowledge it has started */
    	do{
		if(iosf_read32(handle, 0x04, 0x71, &val) != IOSF_OK)
			goto IOSF_ERROR;	
		local_time_out--;
	}while((val&(1<<30)) == 0 || local_time_out == 0);

    	/* clear this bit so that we could upgrade the fw more than one time */
    	if(iosf_modify(handle, 0x04,0x71,(0x01<<30),0x00) != IOSF_OK)
		goto IOSF_ERROR;
	/*
	*For CE5300/CE2600, the DRAM is not needed once it's copied to P-Unit internal RAM, this is different from CE4100/CE4200 
	*We could free this memory region safely.
	*/
	kfree((void*)fw_info->fw_mem_addr);
    	return RET_OK;

IOSF_ERROR:
	printk(KERN_ERR "Failed to perform IOSF operations.\n");
	kfree((void*)fw_info->fw_mem_addr);
	return RET_IOSF_ERR;
}

static punit_ret_result_t verify_firmware_binary(struct firmware *fw)
{
	if(fw->size > PUNIT_MEM_SIZE){
		printk(KERN_ERR "verify th firmware binary failed, please check it %s\n", fw_name);
		return RET_FW_BIN_ERR;
	}
	return RET_OK;
}

static punit_ret_result_t punit_request_firmware(fw_info_t *fw_info)
{
	int err;
	uint32_t fw_version;
	struct firmware *fw;
	struct platform_device *pdev;
	pdev = platform_device_register_simple("punit_fw_upgrade", 0, NULL, 0);
        if (IS_ERR(pdev)) {
                printk(KERN_ERR "Failed to register device for \"%s\"\n",fw_name);
                return RET_OTHER_ERR;
        }
	err = request_firmware((const struct firmware**)&fw_info->fw_entry, fw_name, &pdev->dev);
	platform_device_unregister(pdev);
        if (err) {
                printk(KERN_ERR "Failed to request firmware \"%s\" err %d\n",fw_name, err);
                return RET_FW_REQ_ERR;
        }
	fw = fw_info->fw_entry;
	if(verify_firmware_binary(fw) != RET_OK)
		return RET_FW_BIN_ERR; 
	fw_version = *(uint32_t*)(&fw->data[fw->size - 4]);
	fw_info->fw_version = ((fw_version & 0xFF) << 24)
                        | ((fw_version & 0xFF00) << 8)
                        | ((fw_version >> 8) & 0xFF00)
                        | (fw_version >> 24);
        return RET_OK;				
}

punit_ret_result_t _is_need_upgrade(struct firmware **pfw_entry, platform_info_t *platform_info)
{

	fw_info_t *fw_info = container_of(pfw_entry, fw_info_t, fw_entry);
	platform_info->_get_running_fw_version(platform_info);
 	if ((platform_info->fw_version&0xffffff) >= (fw_info->fw_version & 0xffffff) ){
		fw_info->flash_need_upgrade = false;
	}else{
		fw_info->flash_need_upgrade = true;
	}	
	return RET_OK;
}

static punit_ret_result_t register_fw_load_ops(platform_info_t *platform_info)
{
	pal_soc_info_t *soc_info = &platform_info->soc_info;
	switch (soc_info->name){
		case SOC_NAME_CE2600:
			platform_info->_get_running_fw_version = _ce2600_get_running_fw_version;
			platform_info->_is_need_upgrade = _ce2600_is_need_upgrade;
			platform_info->_upgrade_firmware = _ce2600_upgrade_firmware;
		break;
		case SOC_NAME_CE5300:
			platform_info->_get_running_fw_version = _ce5300_get_running_fw_version;
			platform_info->_is_need_upgrade = _ce5300_is_need_upgrade;
			platform_info->_upgrade_firmware = _ce5300_upgrade_firmware;
		break;
		default:
			return RET_NOT_SUPPORTED_PLATFORM;
		break;
	}
	return RET_OK;
}

static int __init punit_load_fw_init(void)
{
	int retval;
	memset(&_fw_info, 0, sizeof(fw_info_t));
	memset(&_platform_info, 0, sizeof(platform_info_t));
	_fw_info.platform_priv = (void*)&_platform_info;

	if (pal_get_soc_info(&_platform_info.soc_info)!= PAL_SUCCESS){
		printk(KERN_ERR "Failed to get soc info\n ");
		goto ERR_AND_EXIT;
	}
	if (register_fw_load_ops(&_platform_info) != RET_OK){
		printk (KERN_ERR "Not support soc\n");
		goto ERR_AND_EXIT;
	}
	if(iosf_open(0, &_platform_info.handle)!= IOSF_OK ){
		printk(KERN_ERR "Failed to open iosf device\n");
		goto ERR_AND_EXIT;
	}
	if (punit_request_firmware(&_fw_info) != RET_OK){
		printk(KERN_ERR "Failed to request firmware\n");
		goto ERR_AND_EXIT;
	}
	if(_platform_info._get_running_fw_version(&_platform_info) != RET_OK){
		printk(KERN_ERR "Failed to get running firmware version\n");
		goto ERR_AND_EXIT;
	}
	if(_platform_info._is_need_upgrade(&_fw_info.fw_entry, &_platform_info) == RET_OK){
		if(_fw_info.flash_need_upgrade == false ){
			printk(KERN_INFO "Firmware NOT upgraded: running one is the latest.\n");
	 	}else{
			if(_platform_info._upgrade_firmware(&_fw_info.fw_entry, &_platform_info) == RET_OK){
				printk(KERN_INFO "Firmware upgrade SUCCESSFULLY.\n");
			}else{
				printk(KERN_ERR "Firmware upgrade FAILED.\n");
				goto ERR_AND_EXIT;
			}
		}
	}
	retval = punit_create_sys_info(&_fw_info);
	return retval;

ERR_AND_EXIT:
	return -EINVAL;
	
}

static void __exit punit_load_fw_exit(void)
{
	punit_destroy_sys_info(&_fw_info);
	iosf_close(_platform_info.handle);
}
 
module_init (punit_load_fw_init);
module_exit (punit_load_fw_exit);
 
/* module identification */
MODULE_AUTHOR("Intel Corporation, (C) 2012 - All Rights Reserved");
MODULE_DESCRIPTION("Kernel Module to Upgrade P-Unit Firmware");
MODULE_SUPPORTED_DEVICE("Intel Media Processors");
MODULE_LICENSE("GPL"); /* Inform kernel that module is GPL only. */
