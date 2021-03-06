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
 * File Name:_ce5300_upgrade_ops.h
 *------------------------------------------------------------------------------
 */


#ifndef __CE5300_UPGRADE_OPS_H
#define __CE5300_UPGRADE_OPS_H
#include "punit_fw_upgrade.h"
punit_ret_result_t _ce5300_get_running_fw_version( platform_info_t *platform_info);
punit_ret_result_t _ce5300_is_need_upgrade(struct firmware **pfw_entry, platform_info_t *platform_info);
punit_ret_result_t _ce5300_upgrade_firmware(struct firmware **pfw_entry, platform_info_t *platform_info);
#endif /*__CE5300_UPGRADE_OPS_H*/
