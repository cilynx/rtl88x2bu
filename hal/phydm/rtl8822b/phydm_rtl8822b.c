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
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8822B_SUPPORT == 1)
void phydm_dynamic_switch_htstf_mumimo_8822b(struct dm_struct *dm)
{
	u8 rssi_l2h = 40, rssi_h2l = 35;

	/*@if Pin > -60dBm, enable HT-STF gain controller, otherwise, if rssi < -65dBm, disable the controller*/

	if (dm->rssi_min >= rssi_l2h)
		odm_set_bb_reg(dm, R_0x8d8, BIT(17), 0x1);
	else if (dm->rssi_min < rssi_h2l)
		odm_set_bb_reg(dm, R_0x8d8, BIT(17), 0x0);
}

void phydm_dynamic_parameters_ota(struct dm_struct *dm)
{
	u8 rssi_l2h = 40, rssi_h2l = 35;

	/* PD TH modify due to enlarge MF windows size */
	odm_set_bb_reg(dm, 0x838, 0xf0, 0x6);

	if ((*dm->channel <= 14) && (*dm->band_width == CHANNEL_WIDTH_20)) {
		if (dm->rssi_min >= rssi_l2h) {
			/*@if (dm->bhtstfdisabled == false)*/
			odm_set_bb_reg(dm, R_0x8d8, BIT(17), 0x1);

			odm_set_bb_reg(dm, R_0x98c, 0x7fc0000, 0x0);
			odm_set_bb_reg(dm, R_0x818, 0x7000000, 0x1);
			odm_set_bb_reg(dm, R_0xc04, BIT(18), 0x0);
			odm_set_bb_reg(dm, R_0xe04, BIT(18), 0x0);
			if (dm->p_advance_ota & PHYDM_HP_OTA_SETTING_A) {
				odm_set_bb_reg(dm, R_0x19d8, MASKDWORD, 0x444);
				odm_set_bb_reg(dm, R_0x19d4, MASKDWORD, 0x4444aaaa);
			} else if (dm->p_advance_ota & PHYDM_HP_OTA_SETTING_B) {
				odm_set_bb_reg(dm, R_0x19d8, MASKDWORD, 0x444);
				odm_set_bb_reg(dm, R_0x19d4, MASKDWORD, 0x444444aa);
			}
		} else if (dm->rssi_min < rssi_h2l) {
			/*@if (dm->bhtstfdisabled == true)*/
			odm_set_bb_reg(dm, R_0x8d8, BIT(17), 0x0);

			odm_set_bb_reg(dm, R_0x98c, MASKDWORD, 0x43440000);
			odm_set_bb_reg(dm, R_0x818, 0x7000000, 0x4);
			odm_set_bb_reg(dm, R_0xc04, (BIT(18) | BIT(21)), 0x0);
			odm_set_bb_reg(dm, R_0xe04, (BIT(18) | BIT(21)), 0x0);
			odm_set_bb_reg(dm, R_0x19d8, MASKDWORD, 0xaaa);
			odm_set_bb_reg(dm, R_0x19d4, MASKDWORD, 0xaaaaaaaa);
		}
	} else {
#if 0
		//odm_set_bb_reg(dm, R_0x8d8, BIT(17), 0x0);
#endif
		odm_set_bb_reg(dm, R_0x98c, MASKDWORD, 0x43440000);
		odm_set_bb_reg(dm, R_0x818, 0x7000000, 0x4);
		odm_set_bb_reg(dm, R_0xc04, (BIT(18) | BIT(21)), 0x0);
		odm_set_bb_reg(dm, R_0xe04, (BIT(18) | BIT(21)), 0x0);
		odm_set_bb_reg(dm, R_0x19d8, MASKDWORD, 0xaaa);
		odm_set_bb_reg(dm, R_0x19d4, MASKDWORD, 0xaaaaaaaa);
	}
}

static void
_set_tx_a_cali_value(
	struct dm_struct *dm,
	enum rf_path rf_path,
	u8 offset,
	u8 tx_a_bias_offset)
{
	u32 modi_tx_a_value = 0;
	u8 tmp1_byte = 0;
	boolean is_minus = false;
	u8 comp_value = 0;

	switch (offset) {
	case 0x0:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X10124);
		break;
	case 0x1:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X10524);
		break;
	case 0x2:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X10924);
		break;
	case 0x3:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X10D24);
		break;
	case 0x4:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X30164);
		break;
	case 0x5:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X30564);
		break;
	case 0x6:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X30964);
		break;
	case 0x7:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X30D64);
		break;
	case 0x8:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X50195);
		break;
	case 0x9:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X50595);
		break;
	case 0xa:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X50995);
		break;
	case 0xb:
		odm_set_rf_reg(dm, rf_path, RF_0x18, 0xFFFFF, 0X50D95);
		break;
	default:
		PHYDM_DBG(dm, ODM_COMP_API, "Invalid TxA band offset...\n");
		return;
	}

	/* @Get TxA value */
	modi_tx_a_value = odm_get_rf_reg(dm, rf_path, RF_0x61, 0xFFFFF);
	tmp1_byte = (u8)modi_tx_a_value & (BIT(3) | BIT(2) | BIT(1) | BIT(0));

	/* @check how much need to calibration */
	switch (tx_a_bias_offset) {
	case 0xF6:
		is_minus = true;
		comp_value = 3;
		break;

	case 0xF4:
		is_minus = true;
		comp_value = 2;
		break;

	case 0xF2:
		is_minus = true;
		comp_value = 1;
		break;

	case 0xF3:
		is_minus = false;
		comp_value = 1;
		break;

	case 0xF5:
		is_minus = false;
		comp_value = 2;
		break;

	case 0xF7:
		is_minus = false;
		comp_value = 3;
		break;

	case 0xF9:
		is_minus = false;
		comp_value = 4;
		break;

	/* @do nothing case */
	case 0xF0:
	default:
		PHYDM_DBG(dm, ODM_COMP_API,
			  "No need to do TxA bias current calibration\n");
		return;
	}

	/* @calc correct value to calibrate */
	if (is_minus) {
		if (tmp1_byte >= comp_value)
			tmp1_byte -= comp_value;
#if 0
			//modi_tx_a_value += tmp1_byte;
#endif
		else
			tmp1_byte = 0;
	} else {
		tmp1_byte += comp_value;
		if (tmp1_byte >= 7)
			tmp1_byte = 7;
	}

	/* Write back to RF reg */
	odm_set_rf_reg(dm, rf_path, RF_0x30, 0xFFFF, (offset << 12 | (modi_tx_a_value & 0xFF0) | tmp1_byte));
}

static void
_txa_bias_cali_4_each_path(
	struct dm_struct *dm,
	u8 rf_path,
	u8 efuse_value)
{
	/* switch on set TxA bias */
	odm_set_rf_reg(dm, rf_path, RF_0xef, 0xFFFFF, 0x200);

	/* Set 12 sets of TxA value */
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x0, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x1, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x2, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x3, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x4, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x5, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x6, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x7, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x8, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0x9, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0xa, efuse_value);
	_set_tx_a_cali_value(dm, (enum rf_path)rf_path, 0xb, efuse_value);

	/* switch off set TxA bias */
	odm_set_rf_reg(dm, rf_path, RF_0xef, 0xFFFFF, 0x0);
}

/* @for 8822B PCIE D-cut patch only */
/* Normal driver and MP driver need this patch */

void phydm_txcurrentcalibration(struct dm_struct *dm)
{
	u8 efuse0x3D8, efuse0x3D7;
	u32 orig_rf0x18_path_a = 0, orig_rf0x18_path_b = 0;

	if (!(dm->support_ic_type & ODM_RTL8822B))
		return;

	PHYDM_DBG(dm, ODM_COMP_API,
		  "8822b 5g tx current calibration 0x3d7=0x%X 0x3d8=0x%X\n",
		  dm->efuse0x3d7, dm->efuse0x3d8);

	/* save original 0x18 value */
	orig_rf0x18_path_a = odm_get_rf_reg(dm, RF_PATH_A, RF_0x18, 0xFFFFF);
	orig_rf0x18_path_b = odm_get_rf_reg(dm, RF_PATH_B, RF_0x18, 0xFFFFF);

	/* @define efuse content */
	efuse0x3D8 = dm->efuse0x3d8;
	efuse0x3D7 = dm->efuse0x3d7;

	/* @check efuse content to judge whether need to calibration or not */
	if (efuse0x3D7 == 0xFF) {
		PHYDM_DBG(dm, ODM_COMP_API,
			  "efuse content 0x3D7 == 0xFF, No need to do TxA cali\n");
		return;
	}

	/* write RF register for calibration */
	_txa_bias_cali_4_each_path(dm, RF_PATH_A, efuse0x3D7);
	_txa_bias_cali_4_each_path(dm, RF_PATH_B, efuse0x3D8);

	/* restore original 0x18 value */
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x18, 0xFFFFF, orig_rf0x18_path_a);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0x18, 0xFFFFF, orig_rf0x18_path_b);
}

void phydm_1rcca_setting(struct dm_struct *dm, boolean enable_1rcca)
{
	u32 reg_32;

	reg_32 = odm_get_bb_reg(dm, R_0xa04, 0x0f000000);

	/* @Enable or disable 1RCCA setting accrodding to the control from driver */
	if (enable_1rcca) {
		if (reg_32 == 0x0)
			odm_set_bb_reg(dm, R_0x808, MASKBYTE0, 0x13); /* @CCK path-a */
		else if (reg_32 == 0x5)
			odm_set_bb_reg(dm, R_0x808, MASKBYTE0, 0x23); /* @CCK path-b */
	} else {
		odm_set_bb_reg(dm, R_0x808, MASKBYTE0, 0x33); /* @disable 1RCCA */
		odm_set_bb_reg(dm, R_0xa04, 0x0f000000, 0x0); /* @CCK default is at path-a */
	}
}

void phydm_dynamic_select_cck_path_8822b(struct dm_struct *dm)
{
	struct phydm_fa_struct *fa_cnt = (struct phydm_fa_struct *)phydm_get_structure(dm, PHYDM_FALSEALMCNT);
	struct drp_rtl8822b_struct *drp_8822b = &dm->phydm_rtl8822b;

	if (dm->ap_total_num > 10) {
		if (drp_8822b->path_judge & BIT(2))
			odm_set_bb_reg(dm, R_0xa04, 0x0f000000, 0x0); /*@fix CCK Path A if AP nums > 10*/
		return;
	}

	if (drp_8822b->path_judge & BIT(2))
		return;

	PHYDM_DBG(dm, ODM_PHY_CONFIG,
		  "phydm 8822b cck rx path selection start\n");

	if (drp_8822b->path_judge & BB_PATH_A) {
		drp_8822b->path_a_cck_fa = (u16)fa_cnt->cnt_cck_fail;
		drp_8822b->path_judge &= ~BB_PATH_A;
		odm_set_bb_reg(dm, R_0xa04, 0x0f000000, 0x5); /*@change to path B collect CCKFA*/
	} else if (drp_8822b->path_judge & BB_PATH_B) {
		drp_8822b->path_b_cck_fa = (u16)fa_cnt->cnt_cck_fail;
		drp_8822b->path_judge &= ~BB_PATH_B;

		if (drp_8822b->path_a_cck_fa <= drp_8822b->path_b_cck_fa)
			odm_set_bb_reg(dm, R_0xa04, 0x0f000000, 0x0); /*@FA A<=B choose A*/
		else
			odm_set_bb_reg(dm, R_0xa04, 0x0f000000, 0x5); /*@FA B>A choose B*/

		drp_8822b->path_judge |= BIT(2); /*@it means we have already choosed cck rx path*/
	}

	PHYDM_DBG(dm, ODM_PHY_CONFIG, "path_a_fa = %d, path_b_fa = %d\n",
		  drp_8822b->path_a_cck_fa, drp_8822b->path_b_cck_fa);
}

void phydm_somlrxhp_setting(struct dm_struct *dm, boolean switch_soml)
{
	if (switch_soml) {
		odm_set_bb_reg(dm, R_0x19a8, MASKDWORD, 0xd90a0000);
		/* @Following are RxHP settings for T2R as always low, workaround for OTA test, required to classify */
		odm_set_bb_reg(dm, R_0xc04, (BIT(21) | BIT(18)), 0x0);
		odm_set_bb_reg(dm, R_0xe04, (BIT(21) | BIT(18)), 0x0);
	} else {
		odm_set_bb_reg(dm, R_0x19a8, MASKDWORD, 0x090a0000);
		odm_set_bb_reg(dm, R_0xc04, (BIT(21) | BIT(18)), 0x0);
		odm_set_bb_reg(dm, R_0xe04, (BIT(21) | BIT(18)), 0x0);
	}

	/* @Dynamic RxHP setting with SoML on/off apply on all RFE type */
	if (!switch_soml && (dm->rfe_type == 1 || dm->rfe_type == 6 || dm->rfe_type == 7 || dm->rfe_type == 9)) {
		odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108000);
		odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x0);
	}

	if (*dm->channel <= 14) {
		if (switch_soml && (!(dm->rfe_type == 3 || dm->rfe_type == 5 || dm->rfe_type == 8 || dm->rfe_type == 17))) {
			odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108000);
			odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x0);
		}
	} else if (*dm->channel > 35) {
		if (switch_soml) {
			odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108000);
			odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x0);
		}
	}
	/* @for 8822B RXHP H2L, since always L will cause DFS FRD */
	if (dm->is_dfs_band) {
			odm_set_bb_reg(dm, 0x8d8, MASKDWORD, 0x29035612);
			odm_set_bb_reg(dm, 0x8cc, MASKDWORD, 0x08108492);
	}
#if 0
	if (!(dm->rfe_type == 1 || dm->rfe_type == 6 || dm->rfe_type == 7 || dm->rfe_type == 9)) {
		if (*dm->channel <= 14) {
			/* TFBGA iFEM SoML on/off with RxHP always high-to-low */
			if (switch_soml == true && (!(dm->rfe_type == 3 || dm->rfe_type == 5))) {
				if (switch_soml == true) {
				odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108000);
				odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x0);
				odm_set_bb_reg(dm, R_0xc04, (BIT(21) | (BIT(18))), 0x0);
				odm_set_bb_reg(dm, R_0xe04, (BIT(21) | (BIT(18))), 0x0);
				} else {
				odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108492);
				odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x1);
				}
			}
		} else if (*dm->channel > 35) {
			if (switch_soml == true) {
				odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108000);
				odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x0);
				odm_set_bb_reg(dm, R_0xc04, (BIT(21) | (BIT(18))), 0x0);
				odm_set_bb_reg(dm, R_0xe04, (BIT(21) | (BIT(18))), 0x0);
			} else {
				odm_set_bb_reg(dm, R_0x8cc, MASKDWORD, 0x08108492);
				odm_set_bb_reg(dm, R_0x8d8, BIT(27), 0x1);
			}
		}
		PHYDM_DBG(dm, ODM_COMP_API,
			  "Dynamic RxHP control with SoML is enable !!\n");
	}
#endif
}

void phydm_config_tx2path_8822b(struct dm_struct *dm,
				enum wireless_set wireless_mode,
				boolean is_tx2_path)
{
	if (wireless_mode == WIRELESS_CCK) {
		if (is_tx2_path)
			odm_set_bb_reg(dm, R_0xa04, 0xf0000000, 0xc);
		else
			odm_set_bb_reg(dm, R_0xa04, 0xf0000000, 0x8);
	} else {
		if (is_tx2_path)
			odm_set_bb_reg(dm, R_0x93c, 0xf00000, 0x3);
		else
			odm_set_bb_reg(dm, R_0x93c, 0xf00000, 0x1);
	}
}

#ifdef DYN_ANT_WEIGHTING_SUPPORT
void phydm_dynamic_ant_weighting_8822b(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 rssi_l2h = 43, rssi_h2l = 37;
	u8 reg_8;

	if (dm->is_disable_dym_ant_weighting)
		return;
#ifdef CONFIG_MCC_DM
	if (dm->is_stop_dym_ant_weighting)
		return;
#endif
	if (*dm->channel <= 14) {
		if (dm->rssi_min >= rssi_l2h) {
			odm_set_bb_reg(dm, R_0x98c, 0x7fc0000, 0x0);

			/*@equal weighting*/
			reg_8 = (u8)odm_get_bb_reg(dm, R_0xf94, BIT(0) | BIT(1) | BIT(2));
			PHYDM_DBG(dm, ODM_COMP_API,
				  "Equal weighting ,rssi_min = %d\n, 0xf94[2:0] = 0x%x\n",
				  dm->rssi_min, reg_8);
		} else if (dm->rssi_min <= rssi_h2l) {
			odm_set_bb_reg(dm, R_0x98c, MASKDWORD, 0x43440000);

			/*@fix sec_min_wgt = 1/2*/
			reg_8 = (u8)odm_get_bb_reg(dm, R_0xf94, BIT(0) | BIT(1) | BIT(2));
			PHYDM_DBG(dm, ODM_COMP_API,
				  "AGC weighting ,rssi_min = %d\n, 0xf94[2:0] = 0x%x\n",
				  dm->rssi_min, reg_8);
		}
	} else {
		odm_set_bb_reg(dm, R_0x98c, MASKDWORD, 0x43440000);

		reg_8 = (u8)odm_get_bb_reg(dm, R_0xf94, BIT(0) | BIT(1) | BIT(2));
		PHYDM_DBG(dm, ODM_COMP_API,
			  "AGC weighting ,rssi_min = %d\n, 0xf94[2:0] = 0x%x\n",
			  dm->rssi_min, reg_8);
		/*@fix sec_min_wgt = 1/2*/
	}
}
#endif
#ifdef CONFIG_MCC_DM
#ifdef DYN_ANT_WEIGHTING_SUPPORT
void phydm_set_weighting_cmn(struct dm_struct *dm)
{
	PHYDM_DBG(dm, DBG_COMP_MCC, "phydm_set_weighting_cmn\n");
	odm_set_bb_reg(dm, 0xc04, (BIT(18)|BIT(21)), 0x0);
	odm_set_bb_reg(dm, 0xe04, (BIT(18)|BIT(21)), 0x0);
}
void phydm_set_weighting_mcc(u8 b_equal_weighting, void *dm_void, u8 port)
{
	/*u8 reg_8;*/
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm ->mcc_dm;
	u8	val_0x98e, val_0x98f, val_0x81b;
	PHYDM_DBG(dm , DBG_COMP_MCC, "ant_weighting_mcc ,port = %d\n", port);
	if (b_equal_weighting) {
		val_0x98e = (u8)(odm_get_bb_reg(dm , 0x98c, 0x00ff0000)>>16) & 0xc0;
		val_0x98f = (u8)(odm_get_bb_reg(dm , 0x98c, 0xff000000)>>24) & 0x7f;
		val_0x81b = (u8)(odm_get_bb_reg(dm , 0x818, 0xff000000)>>24) & 0xfd;
		PHYDM_DBG(dm , DBG_COMP_MCC, "Equal weighting ,rssi_min = %d\n",
			dm ->rssi_min);
		/*equal weighting*/
		/*
		odm_set_bb_reg(p_dm_odm, 0x98c, 0x7fc0000, 0x0);
		odm_set_bb_reg(p_dm_odm, 0x818, BIT(26), 0x0);
		reg_8 = odm_get_bb_reg(p_dm_odm, 0xf94, BIT(0)|BIT(1)|BIT(2));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_COMMON, ODM_DBG_LOUD, ("Equal weighting ,rssi_min = %d\n, 0xf94[2:0] = 0x%x\n", p_dm_odm->rssi_min, reg_8));*/
	}
	else
	{
		val_0x98e = 0x44;
		val_0x98f = 0x43;
		val_0x81b = (u8)(odm_get_bb_reg(dm , 0x818, 0xff000000)>>24) | BIT(2);
		PHYDM_DBG(dm , DBG_COMP_MCC, "AGC weighting ,rssi_min = %d\n",
			dm ->rssi_min);
		/*fix sec_min_wgt = 1/2*/
		/*
		odm_set_bb_reg(p_dm_odm, 0x98c, MASKDWORD, 0x43440000);
		odm_set_bb_reg(p_dm_odm, 0x818, BIT(26), 0x1);
		reg_8 = odm_get_bb_reg(p_dm_odm, 0xf94, BIT(0)|BIT(1)|BIT(2));
		ODM_RT_TRACE(p_dm_odm, ODM_COMP_COMMON, ODM_DBG_LOUD, ("AGC weighting ,rssi_min = %d\n, 0xf94[2:0] = 0x%x\n", p_dm_odm->rssi_min, reg_8));*/
	}
	mcc_dm->mcc_reg_id[2] = 0x2;
	mcc_dm->mcc_dm_reg[2] = 0x98e;
	mcc_dm->mcc_dm_val[2][port] = val_0x98e;

	mcc_dm->mcc_reg_id[3] = 0x3;
	mcc_dm->mcc_dm_reg[3] = 0x98f;
	mcc_dm->mcc_dm_val[3][port] = val_0x98f;

	mcc_dm->mcc_reg_id[4] = 0x4;
	mcc_dm->mcc_dm_reg[4] = 0x81b;
	mcc_dm->mcc_dm_val[4][port] = val_0x81b;
	
}
void phydm_dyn_ant_dec_mcc(u8 port, u8 rssi_in, void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 rssi_l2h = 43, rssi_h2l = 37;
	if (rssi_in == 0xff) {
		phydm_set_weighting_mcc(FALSE, dm, port);
	}
	else if (rssi_in >= rssi_l2h) {
		phydm_set_weighting_mcc(TRUE, dm, port);
	}
	else if (rssi_in <= rssi_h2l) {
		phydm_set_weighting_mcc(FALSE, dm, port);
	}
}

void phydm_dynamic_ant_weighting_mcc_8822b(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8	i;
	phydm_set_weighting_cmn(dm);
	for (i = 0; i <= 1; i++)
		phydm_dyn_ant_dec_mcc(i, mcc_dm->mcc_rssi[i], dm);
}
#endif /*#ifdef DYN_ANT_WEIGHTING_SUPPORT*/

void phydm_mcc_init (void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8	i;
	/*PHYDM_DBG(dm, DBG_COMP_MCC, ("MCC init\n"));*/
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC init\n");
	for (i = 0; i < MCC_DM_REG_NUM; i++ ) {
		mcc_dm->mcc_reg_id[i] = 0xff;
		mcc_dm->mcc_dm_reg[i] = 0;
		mcc_dm->mcc_dm_val[i][0] = 0;
		mcc_dm->mcc_dm_val[i][1] = 0;
	}
	for (i = 0; i < NUM_STA; i++ ) {
		mcc_dm->sta_macid[0][i] = 0xff;
		mcc_dm->sta_macid[1][i] = 0xff;
	}
	/* Function init */
	dm->is_stop_dym_ant_weighting = 0;
}

u8 phydm_check(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	struct cmn_sta_info			*p_entry = NULL;
	u8	shift = 0;
	u8	i = 0;
	u8	j = 0;
	u8	rssi_tmp_min[2] = {0xff, 0xff};
	u8	sta_num = 8;
	u8 mcc_macid = 0;
	for (i = 0;i <= 1;i++) {
		for (j = 0;j < sta_num;j++) {
			if (mcc_dm->sta_macid[i][j] != 0xff) {
				mcc_macid = mcc_dm->sta_macid[i][j];
				p_entry = dm->phydm_sta_info[mcc_macid];
				if (p_entry == NULL) {
					PHYDM_DBG(dm, DBG_COMP_MCC, "Pentry == NULL(mac=%d)\n", 
						mcc_dm->sta_macid[i][j]);
					return _FAIL;
				}
				PHYDM_DBG(dm, DBG_COMP_MCC, "undecorated_smoothed_pwdb=%d\n", 
					p_entry->rssi_stat.rssi);
				if (p_entry->rssi_stat.rssi < rssi_tmp_min[i])
					rssi_tmp_min[i] = p_entry->rssi_stat.rssi;
			}
		}
	}
	mcc_dm->mcc_rssi[0] = (u8)rssi_tmp_min[0];
	mcc_dm->mcc_rssi[1] = (u8)rssi_tmp_min[1];
	return _SUCCESS;
}

void phydm_mcc_h2ccmd_rst(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8 i;
	u8 regid;
	u8 h2c_mcc[H2C_MAX_LENGTH];

	/* RST MCC */
	for (i = 0; i < H2C_MAX_LENGTH; i++ ) {
		h2c_mcc[i] = 0xff;
	}
	h2c_mcc[0] = 0x00;
	odm_fill_h2c_cmd(dm, PHYDM_H2C_MCC, H2C_MAX_LENGTH, h2c_mcc);
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC H2C RST\n");
}

void phydm_mcc_h2ccmd(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8 i;
	u8 regid;
	u8 h2c_mcc[H2C_MAX_LENGTH];

	if (mcc_dm->mcc_rf_channel[0] == 0xff && mcc_dm->mcc_rf_channel[1] == 0xff) {
		PHYDM_DBG(dm, DBG_COMP_MCC, "MCC channel Error\n");	
		return;
	}
	/* Set Channel number */
	for (i = 0; i < H2C_MAX_LENGTH; i++ ) {
		h2c_mcc[i] = 0xff;
	}
	h2c_mcc[0] = 0xe0;
	h2c_mcc[1] = (u8)(mcc_dm->mcc_rf_channel[0]);
	h2c_mcc[2] = (u8)(mcc_dm->mcc_rf_channel[0] >> 8);
	h2c_mcc[3] = (u8)(mcc_dm->mcc_rf_channel[1]);
	h2c_mcc[4] = (u8)(mcc_dm->mcc_rf_channel[1] >> 8);
	h2c_mcc[5] = 0xff;
	h2c_mcc[6] = 0xff;
	odm_fill_h2c_cmd(dm, PHYDM_H2C_MCC, H2C_MAX_LENGTH, h2c_mcc);
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC H2C SetCH: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
					h2c_mcc[0], h2c_mcc[1], h2c_mcc[2], h2c_mcc[3], h2c_mcc[4],
					h2c_mcc[5], h2c_mcc[6]);

	/* Set Reg and value*/
	for (i = 0; i < H2C_MAX_LENGTH; i++ ) {
		h2c_mcc[i] = 0xff;
	}

	for (i = 0; i < MCC_DM_REG_NUM; i++ ) {
		regid = mcc_dm->mcc_reg_id[i];
		if (regid!=0xff) {
			h2c_mcc[0] = 0xa0 | (regid & 0x1f);
			h2c_mcc[1] = (u8)(mcc_dm->mcc_dm_reg[i]);
			h2c_mcc[2] = (u8)(mcc_dm->mcc_dm_reg[i] >> 8);
			h2c_mcc[3] = mcc_dm->mcc_dm_val[i][0];
			h2c_mcc[4] = mcc_dm->mcc_dm_val[i][1];
			h2c_mcc[5] = 0xff;
			h2c_mcc[6] = 0xff;
			odm_fill_h2c_cmd(dm, PHYDM_H2C_MCC, H2C_MAX_LENGTH, h2c_mcc);
			PHYDM_DBG(dm, DBG_COMP_MCC, "MCC H2C: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
					h2c_mcc[0], h2c_mcc[1], h2c_mcc[2], h2c_mcc[3], h2c_mcc[4],
					h2c_mcc[5], h2c_mcc[6]);
		}
	}
	
	
}

void phydm_mcc_ctrl(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC status: %x\n", mcc_dm->mcc_status);
	/*MCC stage no change*/
	if (mcc_dm->mcc_status == mcc_dm->mcc_pre_status)
		return;
	/*Not in MCC stage*/
	if (mcc_dm->mcc_status == 0)
	{
		/* Enable normal Ant-weighting */
		dm->is_stop_dym_ant_weighting = 0;
		/* Enable normal DIG */
		odm_pause_dig(dm, PHYDM_RESUME, PHYDM_PAUSE_LEVEL_1, 0x20);
	}
	else
	{
		/* Disable normal Ant-weighting */
		dm->is_stop_dym_ant_weighting = 1;
		/* Enable normal DIG */
		odm_pause_dig(dm, PHYDM_PAUSE_NO_SET, PHYDM_PAUSE_LEVEL_1, 0x20);
	}
	if (mcc_dm->mcc_status == 0 && mcc_dm->mcc_pre_status != 0)
		phydm_mcc_init(dm);
	mcc_dm->mcc_pre_status = mcc_dm->mcc_status;
	}

void phydm_fill_mcccmd( void *dm_void, u8 regid, u16 reg_add,	u8 val0,	u8 val1)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	mcc_dm->mcc_reg_id[regid] = regid;
	mcc_dm->mcc_dm_reg[regid] = reg_add;
	mcc_dm->mcc_dm_val[regid][0] = val0;
	mcc_dm->mcc_dm_val[regid][1] = val1;
}

void phydm_mcc_switch(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	s8 ret;
	phydm_mcc_ctrl(dm);
	if (mcc_dm->mcc_status == 0) {/*Not in MCC stage*/
		phydm_mcc_h2ccmd_rst(dm);
		return;
	}
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC switch\n");
	ret = phydm_check(dm);
	if (ret == _FAIL) {
		PHYDM_DBG(dm, DBG_COMP_MCC, "MCC check fail\n");
		return;
	}
	/* Set IGI*/
	phydm_mcc_igi_cal(dm);
	
	/* Set Antenna Gain*/
#if (RTL8822B_SUPPORT == 1)
	phydm_dynamic_ant_weighting_mcc_8822b(dm);
#endif
	/* Set H2C Cmd*/
	phydm_mcc_h2ccmd(dm);

}

#endif

#ifdef CONFIG_DYNAMIC_BYPASS
void
phydm_pw_sat_8822b(
	struct dm_struct *dm,
	u8 rssi_value
)
{
	u8 ret=2;

	if (dm->rfe_type != 2)
		return;

	/* check */
	ret = odm_get_bb_reg(dm, 0xcb0, BIT(9));

	/* apply on eFEM type*/
	if (rssi_value >= 75) {
		if (*dm->p_channel > 35) {
			if (ret == 0) {
				PHYDM_DBG(dm, ODM_PHY_CONFIG,
				  ("Already in bypass mode setting\n"));
				return;
			}
			odm_set_bb_reg(dm, 0x8cc, MASKDWORD, 0x8108000);
			odm_set_bb_reg(dm, 0x8d8, BIT(27), 0x0);
			odm_set_bb_reg(dm, 0x840, BIT(12), 0x1);
			odm_set_bb_reg(dm, 0x810, (BIT(24)|BIT(25)), 0x2);
			odm_set_bb_reg(dm, 0x814, BIT(0), 0x1);
			odm_set_bb_reg(dm, 0x844, BIT(24), 0x1);

			odm_set_bb_reg(dm, 0x830, MASKDWORD, 0x79a0eaaa);
			odm_set_bb_reg(dm, 0xe58, BIT(20), 0x1);
				
			odm_set_bb_reg(dm, 0xcb0, (MASKBYTE2 | MASKLWORD),
					0x177717);
			odm_set_bb_reg(dm, 0xeb0, (MASKBYTE2 | MASKLWORD),
					0x177717);
			odm_set_bb_reg(dm, 0xcb4, MASKBYTE1, 0x77);
			odm_set_bb_reg(dm, 0xeb4, MASKBYTE1, 0x77);

			PHYDM_DBG(dm, ODM_PHY_CONFIG,
				 ("External-fem turn off !!\n"));

			phydm_rxagc_switch_8822b(dm, true);
		}
	} else if (rssi_value <= 55) {
		if (*dm->p_channel > 35) {
			if (ret == 1) {
				PHYDM_DBG(dm, ODM_PHY_CONFIG,
				  ("Not in bypass mode setting\n"));
				return;
			}
			odm_set_bb_reg(dm, 0xcb0, (MASKBYTE2 | MASKLWORD),
					0x177517);
			odm_set_bb_reg(dm, 0xeb0, (MASKBYTE2 | MASKLWORD),
					0x177517);
			odm_set_bb_reg(dm, 0xcb4, MASKBYTE1, 0x75);
			odm_set_bb_reg(dm, 0xeb4, MASKBYTE1, 0x75);

			PHYDM_DBG(dm, ODM_PHY_CONFIG,
			("External-fem turn on !!rssi =%d\n", rssi_value));

			phydm_rxagc_switch_8822b(dm, false);
		}
	}
}
#endif

void phydm_hwsetting_8822b(struct dm_struct *dm)
{
	struct drp_rtl8822b_struct *drp_8822b = &dm->phydm_rtl8822b;
	u8 set_result_nbi = PHYDM_SET_NO_NEED;

	if ((dm->p_advance_ota & PHYDM_HP_OTA_SETTING_A) || (dm->p_advance_ota & PHYDM_HP_OTA_SETTING_B)) {
		phydm_dynamic_parameters_ota(dm);
	} else {
		if (!dm->bhtstfdisabled)
			phydm_dynamic_switch_htstf_mumimo_8822b(dm);
		else
			PHYDM_DBG(dm, ODM_PHY_CONFIG,
				  "Default HT-STF gain control setting\n");
	}

	phydm_dynamic_ant_weighting(dm);

	if (dm->p_advance_ota & PHYDM_ASUS_OTA_SETTING) {
		/* PD TH modify due to enlarge MF windows size */
		odm_set_bb_reg(dm, 0x838, 0xf0, 0x6);

		if (dm->rssi_min <= 20)
			phydm_somlrxhp_setting(dm, false);
		else if (dm->rssi_min >= 25)
			phydm_somlrxhp_setting(dm, true);
	}

	if ((dm->p_advance_ota & PHYDM_ASUS_OTA_SETTING_CCK_PATH) || (dm->p_advance_ota & PHYDM_HP_OTA_SETTING_CCK_PATH)) {
		if (dm->is_linked)
			phydm_dynamic_select_cck_path_8822b(dm);
		else
			drp_8822b->path_judge |= ((~BIT(2)) | BB_PATH_A | BB_PATH_B);
	}

	if (dm->p_advance_ota & PHYDM_LENOVO_OTA_SETTING_NBI_CSI) {
		if ((*dm->band_width == CHANNEL_WIDTH_80) && (*dm->channel == 157)) {
			set_result_nbi = phydm_nbi_setting(dm, FUNC_ENABLE, *dm->channel, 80, 5760, PHYDM_DONT_CARE);
			PHYDM_DBG(dm, ODM_PHY_CONFIG, "Enable NBI\n");
		}
	}
}

#endif /* RTL8822B_SUPPORT == 1 */
