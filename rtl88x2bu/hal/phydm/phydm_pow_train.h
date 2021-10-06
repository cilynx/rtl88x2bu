/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/

#ifndef __PHYDM_POW_TRAIN_H__
#define __PHYDM_POW_TRAIN_H__

#define POW_TRAIN_VERSION "1.0" /* @2017.07.0141  Dino, Add phydm_pow_train.h*/

/****************************************************************
 * 1 ============================================================
 * 1  Definition
 * 1 ============================================================
 ***************************************************************/

#ifdef PHYDM_POWER_TRAINING_SUPPORT
/****************************************************************
 * 1 ============================================================
 * 1  structure
 * 1 ============================================================
 ***************************************************************/

struct phydm_pow_train_stuc {
	u8 pt_state;
	u32 pow_train_score;
};

/****************************************************************
 * 1 ============================================================
 * 1  enumeration
 * 1 ============================================================
 ***************************************************************/

enum pow_train_state {
	DYNAMIC_POW_TRAIN = 0,
	ENABLE_POW_TRAIN = 1,
	DISABLE_POW_TRAIN = 2
};

enum power_training_score {
	DISABLE_PT_SCORE = 0,
	KEEP_PRE_PT_SCORE = 1,
	ENABLE_PT_SCORE = 2
};

/****************************************************************
 * 1 ============================================================
 * 1  function prototype
 * 1 ============================================================
 ***************************************************************/

void phydm_update_power_training_state(
	void *dm_void);

void phydm_pow_train_debug(
	void *dm_void,
	char input[][16],
	u32 *_used,
	char *output,
	u32 *_out_len);

#endif
#endif
