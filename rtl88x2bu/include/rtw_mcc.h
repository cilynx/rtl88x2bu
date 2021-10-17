/******************************************************************************
 *
 * Copyright(c) 2015 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/
#ifdef CONFIG_MCC_MODE

#ifndef _RTW_MCC_H_
#define _RTW_MCC_H_

#include <drv_types.h> /* PADAPTER */

#define MCC_STATUS_PROCESS_MCC_START_SETTING BIT0
#define MCC_STATUS_PROCESS_MCC_STOP_SETTING BIT1
#define MCC_STATUS_NEED_MCC BIT2
#define MCC_STATUS_DOING_MCC BIT3


#define MCC_SWCH_FW_EARLY_TIME 10 /* ms */
#define MCC_EXPIRE_TIME 50 /* ms */
#define MCC_TOLERANCE_TIME 2 /* 2*2 = 4s */
#define MCC_UPDATE_PARAMETER_THRESHOLD 5 /* ms */

#define MCC_ROLE_STA_GC_MGMT_QUEUE_MACID 0
#define MCC_ROLE_SOFTAP_GO_MGMT_QUEUE_MACID 1

/* Lower for stop, Higher for start */
#define MCC_SETCMD_STATUS_STOP_DISCONNECT 0x0
#define MCC_SETCMD_STATUS_STOP_SCAN_START 0x1
#define MCC_SETCMD_STATUS_START_CONNECT 0x80
#define MCC_SETCMD_STATUS_START_SCAN_DONE 0x81

/*
* depenad platform or customer requirement(TP unit:Mbps),
* must be provided by PM or sales or product document
* too large value means not to limit tx bytes (current for ap mode)
* NOTE: following values ref from test results
*/
#define MCC_AP_BW20_TARGET_TX_TP (300)
#define MCC_AP_BW40_TARGET_TX_TP (300)
#define MCC_AP_BW80_TARGET_TX_TP (300)
#define MCC_STA_BW20_TARGET_TX_TP (35)
#define MCC_STA_BW40_TARGET_TX_TP (70)
#define MCC_STA_BW80_TARGET_TX_TP (140)
#define MCC_SINGLE_TX_CRITERIA 5 /* Mbps */

#define MAX_MCC_NUM 2
#define DBG_MCC_REG_NUM 4
#define DBG_MCC_RF_REG_NUM 1

#define MCC_STOP(adapter) (adapter->mcc_adapterpriv.mcc_tx_stop)
#define MCC_EN(adapter) (adapter_to_dvobj(adapter)->mcc_objpriv.en_mcc)
#define adapter_to_mccobjpriv(adapter) (&(adapter_to_dvobj(adapter)->mcc_objpriv))
#define SET_MCC_EN_FLAG(adapter, flag)\
	do { \
		adapter_to_dvobj(adapter)->mcc_objpriv.en_mcc = (flag); \
	} while (0)
#define SET_MCC_DURATION(adapter, val)\
	do { \
		adapter_to_dvobj(adapter)->mcc_objpriv.duration = (val); \
	} while (0)
#define SET_MCC_RUNTIME_DURATION(adapter, flag)\
	do { \
		adapter_to_dvobj(adapter)->mcc_objpriv.enable_runtime_duration = (flag); \
	} while (0)

#define SET_MCC_PHYDM_OFFLOAD(adapter, flag)\
	do { \
		adapter_to_dvobj(adapter)->mcc_objpriv.mcc_phydm_offload = (flag); \
	} while (0)

#ifdef CONFIG_MCC_PHYDM_OFFLOAD
enum mcc_cfg_phydm_ops {
	MCC_CFG_PHYDM_OFFLOAD = 0,
	MCC_CFG_PHYDM_RF_CH,
	MCC_CFG_PHYDM_ADD_CLIENT,
	MCC_CFG_PHYDM_REMOVE_CLIENT,
	MCC_CFG_PHYDM_START,
	MCC_CFG_PHYDM_STOP,
	MCC_CFG_PHYDM_DUMP,
	MCC_CFG_PHYDM_MAX,
};
#endif

enum rtw_mcc_cmd_id {
	MCC_CMD_WK_CID = 0,
	MCC_SET_DURATION_WK_CID,
	MCC_GET_DBG_REG_WK_CID,
	#ifdef CONFIG_MCC_PHYDM_OFFLOAD
	MCC_SET_PHYDM_OFFLOAD_WK_CID,
	#endif
};

/* Represent Channel Tx Null setting */
enum mcc_channel_tx_null {
	MCC_ENABLE_TX_NULL = 0,
	MCC_DISABLE_TX_NULL = 1,
};

/* Represent C2H Report setting */
enum mcc_c2h_report {
	MCC_C2H_REPORT_DISABLE = 0,
	MCC_C2H_REPORT_FAIL_STATUS = 1,
	MCC_C2H_REPORT_ALL_STATUS = 2,
};

/* Represent Channel Scan */
enum mcc_channel_scan {
	MCC_CHIDX = 0,
	MCC_SCANCH_RSVD_LOC = 1,
};

/* Represent FW status report of channel switch */
enum mcc_status_rpt {
	MCC_RPT_SUCCESS = 0,
	MCC_RPT_TXNULL_FAIL = 1,
	MCC_RPT_STOPMCC = 2,
	MCC_RPT_READY = 3,
	MCC_RPT_SWICH_CHANNEL_NOTIFY = 7,
	MCC_RPT_UPDATE_NOA_START_TIME = 8,
	MCC_RPT_TSF = 9,
	MCC_RPT_MAX,
};

enum mcc_role {
	MCC_ROLE_STA = 0,
	MCC_ROLE_AP = 1,
	MCC_ROLE_GC = 2,
	MCC_ROLE_GO = 3,
	MCC_ROLE_MAX,
};

struct mcc_iqk_backup {
	u16 TX_X;
	u16 TX_Y;
	u16 RX_X;
	u16 RX_Y;
};

enum mcc_duration_setting {
	MCC_DURATION_MAPPING = 0,
	MCC_DURATION_DIRECET = 1,
};

enum mcc_sched_mode {
	MCC_FAIR_SCHEDULE = 0,
	MCC_FAVOR_STA = 1,
	MCC_FAVOR_P2P = 2,
};

/*  mcc data for adapter */
struct mcc_adapter_priv {
	u8 order;		/* FW document, softap/AP must be 0 */
	enum mcc_role role;			/* MCC role(AP,STA,GO,GC) */
	u8 mcc_duration; /* channel stay period, UNIT:1TU */

	/* flow control */
	u8 mcc_tx_stop;				/* check if tp stop or not */
	u8 mcc_tp_limit;				/* check if tp limit or not */
	u32 mcc_target_tx_bytes_to_port;		/* customer require  */
	u32 mcc_tx_bytes_to_port;	/* already tx to tx fifo (write port) */

	/* data from kernel to check if enqueue data or netif stop queue */
	u32 mcc_tp;
	u64 mcc_tx_bytes_from_kernel;
	u64 mcc_last_tx_bytes_from_kernel;

	/* Backup IQK value for MCC */
	struct mcc_iqk_backup mcc_iqk_arr[MAX_RF_PATH];

	/* mgmt queue macid to avoid RA issue */
	u8 mgmt_queue_macid;

	/* set macid bitmap to let fw know which macid should be tx pause */
	/* all interface share total 16 macid */
	u16 mcc_macid_bitmap;

	/* use for NoA start time (unit: mircoseconds) */
	u32 noa_start_time;

	u8 p2p_go_noa_ie[MAX_P2P_IE_LEN];
	u32 p2p_go_noa_ie_len;
	u64 tsf;
#ifdef CONFIG_TDLS
	u8 backup_tdls_en;
#endif /* CONFIG_TDLS */

	u8 null_early;
	u8 null_rty_num;
};

struct mcc_obj_priv {
	u8 en_mcc; /* enable MCC or not */
	u8 duration; /* store duration(%) from registry, for primary adapter */
	u8 interval;
	u8 start_time;
	u8 mcc_c2h_status;
	u8 cur_mcc_success_cnt; /* used for check mcc switch channel success */
	u8 prev_mcc_success_cnt; /* used for check mcc switch channel success */
	u8 mcc_tolerance_time; /* used for detect mcc switch channel success */
	u8 mcc_loc_rsvd_paga[MAX_MCC_NUM];  /* mcc rsvd page */
	u8 mcc_status; /* mcc status stop or start .... */
	u8 policy_index;
	u8 mcc_stop_threshold;
	u8 current_order;
	u8 last_tsfdiff;
	systime mcc_launch_time; /* mcc launch time, used for starting detect mcc switch channel success */
	_mutex mcc_mutex;
	_lock mcc_lock;
	PADAPTER iface[MAX_MCC_NUM]; /* by order, use for mcc parameter cmd */
	struct submit_ctx mcc_sctx;
	struct submit_ctx mcc_tsf_req_sctx;
	_mutex mcc_tsf_req_mutex;
	u8 mcc_tsf_req_sctx_order; /* record current order for mcc_tsf_req_sctx */
#ifdef CONFIG_MCC_MODE_V2
	u8 mcc_iqk_value_rsvd_page[3];
#endif /* CONFIG_MCC_MODE_V2 */
	u8 mcc_pwr_idx_rsvd_page[MAX_MCC_NUM];
	u8 enable_runtime_duration;
	/* for LG */
	u8 mchan_sched_mode;

	_mutex mcc_dbg_reg_mutex;
	u32 dbg_reg[DBG_MCC_REG_NUM];
	u32 dbg_reg_val[DBG_MCC_REG_NUM];
	u32 dbg_rf_reg[DBG_MCC_RF_REG_NUM];
	u32 dbg_rf_reg_val[DBG_MCC_RF_REG_NUM][MAX_RF_PATH];
	u8 mcc_phydm_offload;
};

/* backup IQK val */
void rtw_hal_mcc_restore_iqk_val(PADAPTER padapter);

/* check mcc status */
u8 rtw_hal_check_mcc_status(PADAPTER padapter, u8 mcc_status);

/* set mcc status */
void rtw_hal_set_mcc_status(PADAPTER padapter, u8 mcc_status);

/* clear mcc status */
void rtw_hal_clear_mcc_status(PADAPTER padapter, u8 mcc_status);

/* dl mcc rsvd page */
u8 rtw_hal_dl_mcc_fw_rsvd_page(_adapter *adapter, u8 *pframe, u16 *index
	, u8 tx_desc, u32 page_size, u8 *total_page_num, RSVDPAGE_LOC *rsvd_page_loc, u8 *page_num);

/* handle C2H */
void rtw_hal_mcc_c2h_handler(PADAPTER padapter, u8 buflen, u8 *tmpBuf);

/* switch channel successfully or not */
void rtw_hal_mcc_sw_status_check(PADAPTER padapter);

/* change some scan flags under site survey */
u8 rtw_hal_mcc_change_scan_flag(PADAPTER padapter, u8 *ch, u8 *bw, u8 *offset);

/* record data kernel TX to driver to check MCC concurrent TX  */
void rtw_hal_mcc_calc_tx_bytes_from_kernel(PADAPTER padapter, u32 len);

/* record data to port to let driver do flow ctrl  */
void rtw_hal_mcc_calc_tx_bytes_to_port(PADAPTER padapter, u32 len);

/* check stop write port or not  */
u8 rtw_hal_mcc_stop_tx_bytes_to_port(PADAPTER padapter);

u8 rtw_hal_set_mcc_setting_scan_start(PADAPTER padapter);

u8 rtw_hal_set_mcc_setting_scan_complete(PADAPTER padapter);

u8 rtw_hal_set_mcc_setting_start_bss_network(PADAPTER padapter, u8 chbw_grouped);

u8 rtw_hal_set_mcc_setting_disconnect(PADAPTER padapter);

u8 rtw_hal_set_mcc_setting_join_done_chk_ch(PADAPTER padapter);

u8 rtw_hal_set_mcc_setting_chk_start_clnt_join(PADAPTER padapter, u8 *ch, u8 *bw, u8 *offset, u8 chbw_allow);

void rtw_hal_dump_mcc_info(void *sel, struct dvobj_priv *dvobj);

void update_mcc_mgntframe_attrib(_adapter *padapter, struct pkt_attrib *pattrib);

u8 rtw_hal_mcc_link_status_chk(_adapter *padapter, const char *msg);

void rtw_hal_mcc_issue_null_data(_adapter *padapter, u8 chbw_allow, u8 ps_mode);

u8 *rtw_hal_mcc_append_go_p2p_ie(PADAPTER padapter, u8 *pframe, u32 *len);

void rtw_hal_dump_mcc_policy_table(void *sel);

void rtw_hal_mcc_update_macid_bitmap(PADAPTER padapter, int mac_id, u8 add);

void rtw_hal_mcc_process_noa(PADAPTER padapter);

void rtw_hal_mcc_parameter_init(PADAPTER padapter);

u8 rtw_mcc_cmd_hdl(PADAPTER adapter, u8 type, const u8 *val);

u8 rtw_set_mcc_duration_cmd(_adapter *adapter, u8 type, u8 val);
#ifdef CONFIG_MCC_PHYDM_OFFLOAD
u8 rtw_set_mcc_phydm_offload_enable_cmd(PADAPTER adapter, u8 enable, u8 enqueue);
#endif /* CONFIG_MCC_PHYDM_OFFLOAD */
#endif /* _RTW_MCC_H_ */
#endif /* CONFIG_MCC_MODE */
