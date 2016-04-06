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
 * File Name: punit_sys_info.c
 *------------------------------------------------------------------------------
 */

#include <linux/module.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>

#include "punit_fw_upgrade.h"

ssize_t fw_version_show(struct kobject *kobject, struct kobj_attribute *attr,char *buf);
ssize_t flash_need_upgrade_show(struct kobject *kobject, struct kobj_attribute *attr,char *buf);

static struct kobj_attribute fw_version_attribute =
        __ATTR(fw_version, 0444, fw_version_show, NULL);
static struct kobj_attribute flash_need_upgrade_attribute =
        __ATTR(flash_need_upgrade, 0444, flash_need_upgrade_show, NULL);

void fw_info_kobj_release(struct kobject *kobject); 
ssize_t fw_info_common_show(struct kobject *kobject, struct attribute *attr,char *buf); 
  
static struct attribute *fw_info_attrs[] = { 
        &fw_version_attribute.attr, 
        &flash_need_upgrade_attribute.attr,
	NULL, 
}; 
  
struct sysfs_ops fw_info_kobj_sysops = 
{ 
        .show = fw_info_common_show, 
}; 
  
struct kobj_type ktype = 
{ 
        .release = fw_info_kobj_release, 
        .sysfs_ops=&fw_info_kobj_sysops, 
        .default_attrs=fw_info_attrs, 
}; 
  
void fw_info_kobj_release(struct kobject *kobject) 
{ 
	pr_debug("kobject: (%p): %s\n", kobject, __FUNCTION__);
} 

ssize_t fw_version_show(struct kobject *kobject, struct kobj_attribute *attr,char *buf)
{
	uint32_t fw_version;
	fw_info_t *fw_info=container_of(kobject, fw_info_t, fw_info_kobj);
	platform_info_t *platform_info = (platform_info_t*)fw_info->platform_priv;
	if(platform_info->_get_running_fw_version == NULL)
		return sprintf(buf,"Can't get 8051 FW version\n");	
	platform_info->_get_running_fw_version(platform_info);
	fw_version = platform_info->fw_version;
	return sprintf(buf, "Running 8051 FW version:%02X-%d.%d.%d (0x%x) \n",((fw_version>>24)&0xFF),((fw_version>>16)&0xFF),((fw_version>>8)&0xFF),(fw_version&0xFF), fw_version);	
}
  
ssize_t flash_need_upgrade_show(struct kobject *kobject, struct kobj_attribute *attr,char *buf)
{
	fw_info_t *fw_info=container_of(kobject, fw_info_t, fw_info_kobj);
	if(fw_info->flash_need_upgrade == true)
		return sprintf(buf,"Y\n");	
	else
		return sprintf(buf,"N\n");	
}

ssize_t fw_info_common_show(struct kobject *kobject, struct attribute *attr,char *buf) 
{ 
	ssize_t ret = -EIO;
	struct kobj_attribute *kobj_attr = container_of(attr, struct kobj_attribute, attr);
	if(kobj_attr->show)
		ret = kobj_attr->show(kobject, kobj_attr, buf);
	return ret;
} 

int punit_create_sys_info(fw_info_t *fw_info)
{
	int retval = 0;
	retval = kobject_init_and_add(&fw_info->fw_info_kobj, &ktype, &(THIS_MODULE->mkobj.kobj), "fw_info");
	if (retval != 0)
		printk(KERN_ERR "Kobject create failed: (%s)\n", __FUNCTION__);
	return retval;
}

void punit_destroy_sys_info(fw_info_t *fw_info)
{
	kobject_del(&fw_info->fw_info_kobj);
}
