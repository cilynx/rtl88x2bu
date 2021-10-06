/******************************************************************************
 *
 * Copyright(c) 2016 - 2017 Realtek Corporation.
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

#if (BT_SUPPORT == 1 && COEX_SUPPORT == 1)

#if (RTL8822B_SUPPORT == 1)

/* *******************************************
 * The following is for 8822B 1ANT BT Co-exist definition
 * ********************************************/
#define BT_INFO_8822B_1ANT_B_FTP	BIT(7)
#define BT_INFO_8822B_1ANT_B_A2DP	BIT(6)
#define BT_INFO_8822B_1ANT_B_HID	BIT(5)
#define BT_INFO_8822B_1ANT_B_SCO_BUSY	BIT(4)
#define BT_INFO_8822B_1ANT_B_ACL_BUSY	BIT(3)
#define BT_INFO_8822B_1ANT_B_INQ_PAGE	BIT(2)
#define BT_INFO_8822B_1ANT_B_SCO_ESCO	BIT(1)
#define BT_INFO_8822B_1ANT_B_CONNECTION	BIT(0)

#define BTC_RSSI_COEX_THRESH_TOL_8822B_1ANT	2

#define BT_8822B_1ANT_WIFI_NOISY_THRESH	150 /* max: 255 */
#define BT_8822B_1ANT_DEFAULT_ISOLATION	15 /*  unit: dB */

enum bt_8822b_1ant_signal_state {
	BT_8822B_1ANT_GNT_SET_TO_LOW	= 0x0,
	BT_8822B_1ANT_GNT_SET_TO_HIGH	= 0x1,
	BT_8822B_1ANT_GNT_SET_BY_HW	= 0x2,
	BT_8822B_1ANT_GNT_SET_MAX
};

enum bt_8822b_1ant_path_ctrl_owner {
	BT_8822B_1ANT_PCO_BTSIDE	= 0x0,
	BT_8822B_1ANT_PCO_WLSIDE	= 0x1,
	BT_8822B_1ANT_PCO_MAX
};

enum bt_8822b_1ant_gnt_ctrl_type {
	BT_8822B_1ANT_GNT_CTRL_BY_PTA	= 0x0,
	BT_8822B_1ANT_GNT_CTRL_BY_SW	= 0x1,
	BT_8822B_1ANT_GNT_CTRL_MAX
};

enum bt_8822b_1ant_gnt_ctrl_block {
	BT_8822B_1ANT_GNT_BLOCK_RFC_BB	= 0x0,
	BT_8822B_1ANT_GNT_BLOCK_RFC	= 0x1,
	BT_8822B_1ANT_GNT_BLOCK_BB	= 0x2,
	BT_8822B_1ANT_GNT_BLOCK_MAX
};

enum bt_8822b_1ant_lte_coex_table_type {
	BT_8822B_1ANT_CTT_WL_VS_LTE	= 0x0,
	BT_8822B_1ANT_CTT_BT_VS_LTE	= 0x1,
	BT_8822B_1ANT_CTT_MAX
};

enum bt_8822b_1ant_lte_break_table_type {
	BT_8822B_1ANT_LBTT_WL_BREAK_LTE	= 0x0,
	BT_8822B_1ANT_LBTT_BT_BREAK_LTE	= 0x1,
	BT_8822B_1ANT_LBTT_LTE_BREAK_WL	= 0x2,
	BT_8822B_1ANT_LBTT_LTE_BREAK_BT	= 0x3,
	BT_8822B_1ANT_LBTT_MAX
};

enum bt_info_src_8822b_1ant {
	BT_8822B_1ANT_INFO_SRC_WIFI_FW	= 0x0,
	BT_8822B_1ANT_INFO_SRC_BT_RSP	= 0x1,
	BT_8822B_1ANT_INFO_SRC_BT_ACT	= 0x2,
	BT_8822B_1ANT_INFO_SRC_BT_IQK	= 0x3,
	BT_8822B_1ANT_INFO_SRC_BT_SCBD	= 0x4,
	BT_8822B_1ANT_INFO_SRC_MAX
};

enum bt_8822b_1ant_bt_status {
	BT_8822B_1ANT_BSTATUS_NCON_IDLE		= 0x0,
	BT_8822B_1ANT_BSTATUS_CON_IDLE		= 0x1,
	BT_8822B_1ANT_BSTATUS_INQ_PAGE		= 0x2,
	BT_8822B_1ANT_BSTATUS_ACL_BUSY		= 0x3,
	BT_8822B_1ANT_BSTATUS_SCO_BUSY		= 0x4,
	BT_8822B_1ANT_BSTATUS_ACL_SCO_BUSY	= 0x5,
	BT_8822B_1ANT_BSTATUS_MAX
};

enum bt_8822b_1ant_wifi_status {
	BT_8822B_1ANT_WSTATUS_NCON_IDLE		= 0x0,
	BT_8822B_1ANT_WSTATUS_NCON_SCAN		= 0x1,
	BT_8822B_1ANT_WSTATUS_CON_SCAN		= 0x2,
	BT_8822B_1ANT_WSTATUS_CON_SPECPKT	= 0x3,
	BT_8822B_1ANT_WSTATUS_CON_IDLE		= 0x4,
	BT_8822B_1ANT_WSTATUS_CON_BUSY		= 0x5,
	BT_8822B_1ANT_WSTATUS_MAX
};

enum bt_8822b_1ant_coex_algo {
	BT_8822B_1ANT_COEX_UNDEFINED		= 0x0,
	BT_8822B_1ANT_COEX_SCO			= 0x1,
	BT_8822B_1ANT_COEX_HID			= 0x2,
	BT_8822B_1ANT_COEX_A2DP			= 0x3,
	BT_8822B_1ANT_COEX_A2DP_PANHS		= 0x4,
	BT_8822B_1ANT_COEX_PAN			= 0x5,
	BT_8822B_1ANT_COEX_PANHS		= 0x6,
	BT_8822B_1ANT_COEX_PAN_A2DP		= 0x7,
	BT_8822B_1ANT_COEX_PAN_HID		= 0x8,
	BT_8822B_1ANT_COEX_HID_A2DP_PAN		= 0x9,
	BT_8822B_1ANT_COEX_HID_A2DP		= 0xa,
	BT_8822B_1ANT_COEX_NOPROFILEBUSY	= 0xb,
	BT_8822B_1ANT_COEX_A2DPSINK		= 0xc,
	BT_8822B_1ANT_COEX_MAX
};

enum bt_8822b_1ant_ext_ant_switch_type {
	BT_8822B_1ANT_SWITCH_USE_SPDT	= 0x0,
	BT_8822B_1ANT_SWITCH_USE_SP3T	= 0x1,
	BT_8822B_1ANT_SWITCH_MAX
};

enum bt_8822b_1ant_ext_ant_switch_ctrl_type {
	BT_8822B_1ANT_CTRL_BY_BBSW	= 0x0,
	BT_8822B_1ANT_CTRL_BY_PTA	= 0x1,
	BT_8822B_1ANT_CTRL_BY_ANTDIV	= 0x2,
	BT_8822B_1ANT_CTRL_BY_MAC	= 0x3,
	BT_8822B_1ANT_CTRL_BY_BT	= 0x4,
	BT_8822B_1ANT_CTRL_BY_FW	= 0x5,
	BT_8822B_1ANT_CTRL_MAX
};

enum bt_8822b_1ant_ext_ant_switch_pos_type {
	BT_8822B_1ANT_TO_BT		= 0x0,
	BT_8822B_1ANT_TO_WLG		= 0x1,
	BT_8822B_1ANT_TO_WLA		= 0x2,
	BT_8822B_1ANT_TO_NOCARE		= 0x3,
	BT_8822B_1ANT_TO_S0WLG_S1BT	= 0x4,
	BT_8822B_1ANT_TO_MAX
};

enum bt_8822b_1ant_phase {
	BT_8822B_1ANT_PHASE_INIT		= 0x0,
	BT_8822B_1ANT_PHASE_WONLY		= 0x1,
	BT_8822B_1ANT_PHASE_WOFF		= 0x2,
	BT_8822B_1ANT_PHASE_2G			= 0x3,
	BT_8822B_1ANT_PHASE_5G			= 0x4,
	BT_8822B_1ANT_PHASE_BTMP		= 0x5,
	BT_8822B_1ANT_PHASE_POWERON		= 0x6,
	BT_8822B_1ANT_PHASE_2G_WL		= 0x7,
	BT_8822B_1ANT_PHASE_2G_BT		= 0x8,
	BT_8822B_1ANT_PHASE_MCC			= 0x9,
	BT_8822B_1ANT_PHASE_2G_WLBT		= 0xa, /* GNT_BT/GNT_BT PTA */
	BT_8822B_1ANT_PHASE_2G_FREERUN		= 0xb, /* GNT_BT/GNT_BT SW Hi*/
	BT_8822B_1ANT_PHASE_MAX
};

/*ADD SCOREBOARD TO FIX BT LPS 32K ISSUE WHILE WL BUSY*/
enum bt_8822b_1ant_scoreboard {
	BT_8822B_1ANT_SCBD_ACTIVE		= BIT(0),
	BT_8822B_1ANT_SCBD_ONOFF		= BIT(1),
	BT_8822B_1ANT_SCBD_SCAN			= BIT(2),
	BT_8822B_1ANT_SCBD_UNDERTEST		= BIT(3),
	BT_8822B_1ANT_SCBD_RXGAIN		= BIT(4),
	BT_8822B_1ANT_SCBD_WLBUSY		= BIT(6),
	BT_8822B_1ANT_SCBD_EXTFEM		= BIT(8),
	BT_8822B_1ANT_SCBD_TDMA			= BIT(9),
	BT_8822B_1ANT_SCBD_CQDDR		= BIT(10),
	BT_8822B_1ANT_SCBD_ALL			= 0xffff
};

enum bt_8822b_1ant_RUNREASON {
	BT_8822B_1ANT_RSN_2GSCANSTART	= 0x0,
	BT_8822B_1ANT_RSN_5GSCANSTART	= 0x1,
	BT_8822B_1ANT_RSN_SCANFINISH	= 0x2,
	BT_8822B_1ANT_RSN_2GSWITCHBAND	= 0x3,
	BT_8822B_1ANT_RSN_5GSWITCHBAND	= 0x4,
	BT_8822B_1ANT_RSN_2GCONSTART	= 0x5,
	BT_8822B_1ANT_RSN_5GCONSTART	= 0x6,
	BT_8822B_1ANT_RSN_2GCONFINISH	= 0x7,
	BT_8822B_1ANT_RSN_5GCONFINISH	= 0x8,
	BT_8822B_1ANT_RSN_2GMEDIA	= 0x9,
	BT_8822B_1ANT_RSN_5GMEDIA	= 0xa,
	BT_8822B_1ANT_RSN_MEDIADISCON	= 0xb,
	BT_8822B_1ANT_RSN_2GSPECIALPKT	= 0xc,
	BT_8822B_1ANT_RSN_5GSPECIALPKT	= 0xd,
	BT_8822B_1ANT_RSN_BTINFO	= 0xe,
	BT_8822B_1ANT_RSN_PERIODICAL	= 0xf,
	BT_8822B_1ANT_RSN_PNP		= 0x10,
	BT_8822B_1ANT_RSN_LPS		= 0x11,
	BT_8822B_1ANT_RSN_MAX
};

enum bt_8822b_1ant_WL_LINK_MODE {
	BT_8822B_1ANT_WLINK_2G1PORT	= 0x0,
	BT_8822B_1ANT_WLINK_2GMPORT	= 0x1,
	BT_8822B_1ANT_WLINK_25GMPORT	= 0x2,
	BT_8822B_1ANT_WLINK_5G		= 0x3,
	BT_8822B_1ANT_WLINK_2GGO	= 0x4,
	BT_8822B_1ANT_WLINK_2GGC	= 0x5,
	BT_8822B_1ANT_WLINK_BTMR	= 0x6,
	BT_8822B_1ANT_WLINK_MAX
};

struct coex_dm_8822b_1ant {
	/* hw setting */
	u32	cur_ant_pos_type;
	/* fw mechanism */
	boolean cur_ignore_wlan_act;

	u8	cur_ps_tdma;
	u8	ps_tdma_para[5];
	boolean auto_tdma_adjust;
	boolean cur_ps_tdma_on;

	boolean cur_bt_auto_report;
	u8	cur_lps;
	u8	cur_rpwm;

	u8	cur_bt_pwr_lvl;
	u8	cur_wl_pwr_lvl;

	/* sw mechanism */

	boolean cur_low_penalty_ra;

	u32	cur_val0x6c0;
	u32	cur_val0x6c4;
	u32	cur_val0x6c8;
	u8	cur_val0x6cc;

	/* algorithm related */
	u8	cur_algorithm;
	u8	bt_status;
	u8	wifi_chnl_info[3];

	u32	arp_cnt;

	u32	cur_switch_status;

	boolean cur_agc_table_en;
	u32	setting_tdma;
};

struct coex_sta_8822b_1ant {
	boolean bt_disabled;
	boolean bt_link_exist;
	boolean sco_exist;
	boolean a2dp_exist;
	boolean hid_exist;
	boolean pan_exist;
	boolean msft_mr_exist;
	boolean bt_a2dp_active;
	u8	num_of_profile;

	boolean under_lps;
	boolean under_ips;
	u32	specific_pkt_period_cnt;
	u32	high_priority_tx;
	u32	high_priority_rx;
	u32	low_priority_tx;
	u32	low_priority_rx;
	boolean is_hi_pri_rx_overhead;
	s8	bt_rssi;
	u8	pre_bt_rssi_state;
	u8	pre_wifi_rssi_state[4];
	u8	bt_info_c2h[BT_8822B_1ANT_INFO_SRC_MAX][BTC_BTINFO_LENGTH_MAX];
	u32	bt_info_c2h_cnt[BT_8822B_1ANT_INFO_SRC_MAX];
	boolean bt_whck_test;
	boolean c2h_bt_inquiry_page;
	boolean c2h_bt_remote_name_req;
	boolean c2h_bt_page; /* Add for win8.1 page out issue */
	boolean	wifi_high_pri_task1;
	boolean	wifi_high_pri_task2;

	u8	bt_info_lb2;
	u8	bt_info_lb3;
	u8	bt_info_hb0;
	u8	bt_info_hb1;
	u8	bt_info_hb2;
	u8	bt_info_hb3;

	u32	pop_event_cnt;
	u8	scan_ap_num;
	u8	bt_retry_cnt;

	u32	crc_ok_cck;
	u32	crc_ok_11g;
	u32	crc_ok_11n;
	u32	crc_ok_11n_vht;

	u32	crc_err_cck;
	u32	crc_err_11g;
	u32	crc_err_11n;
	u32	crc_err_11n_vht;

	boolean cck_lock;
	boolean cck_lock_ever;
	boolean cck_lock_warn;

	u8	coex_table_type;

	boolean force_lps_ctrl;

	boolean concurrent_rx_mode_on;

	u16	score_board;
	u8	isolation_btween_wb; /* 0~ 50 */

	u8	a2dp_bit_pool;
	u8	kt_ver;
	boolean acl_busy;
	boolean bt_create_connection;

	u32	bt_coex_supported_feature;
	u32	bt_coex_supported_version;

	u8	bt_ble_scan_type;
	u32	bt_ble_scan_para[3];

	boolean run_time_state;
	boolean freeze_coexrun_by_btinfo;

	boolean is_A2DP_3M;
	boolean voice_over_HOGP;
	boolean bt_418_hid_exist;
	boolean bt_ble_hid_exist;
	u8	forbidden_slot;
	u8	hid_busy_num;
	u8	hid_pair_cnt;

	u32	cnt_remote_name_req;
	u32	cnt_setup_link;
	u32	cnt_reinit;
	u32	cnt_ign_wlan_act;
	u32	cnt_page;
	u32	cnt_role_switch;
	u32	cnt_wl_fw_notify;

	u16	bt_reg_vendor_ac;
	u16	bt_reg_vendor_ae;

	boolean is_setup_link;
	u8	wl_noisy_level;
	u32	gnt_error_cnt;
	u8	bt_afh_map[10];
	u8	bt_relink_downcount;
	boolean is_tdma_btautoslot;

	u8	switch_band_notify_to;

	boolean is_bt_multi_link;
	boolean is_bt_a2dp_sink;

	boolean is_set_ps_state_fail;
	u8	cnt_set_ps_state_fail;

	u8	wl_fw_dbg_info[10];
	u8	wl_rx_rate;
	u8	wl_tx_rate;
	u8	wl_rts_rx_rate;
	u8	wl_center_channel;
	u8	wl_tx_macid;
	u8	wl_tx_retry_ratio;

	boolean is_2g_freerun;

	u16	score_board_WB;
	boolean is_hid_rcu;
	u8	bt_a2dp_vendor_id;
	u32	bt_a2dp_device_name;
	u32	bt_a2dp_flush_time;
	boolean is_ble_scan_en;

	boolean is_bt_opp_exist;
	boolean gl_wifi_busy;

	boolean is_mimo_ps;
	u8	connect_ap_period_cnt;
	boolean is_bt_reenable;
	u8	cnt_bt_reenable;
	boolean is_wifi_linkscan_process;
	u8	wl_coex_mode;

	u8	wl_pnp_wakeup_downcnt;
	u32	coex_run_cnt;
	boolean is_no_wl_5ms_extend;

	u16	wl_0x42a_backup;
	u32	wl_0x430_backup;
	u32	wl_0x434_backup;
	u8	wl_0x455_backup;

	boolean wl_tx_limit_en;
	boolean wl_ampdu_limit_en;
	boolean	wl_rxagg_limit_en;
	u8	wl_rxagg_size;
	u8	coex_run_reason;

	u8	tdma_timer_base;
	boolean wl_slot_toggle;
	boolean wl_slot_toggle_change; /* if toggle to no-toggle */
};

struct rfe_type_8822b_1ant {
	u8	rfe_module_type;
	boolean ext_switch_exist;
	u8	ext_switch_type;
	/*  iF 0: ANTSW(rfe_sel9)=0, ANTSWB(rfe_sel8)=1 =>  Ant to BT/5G */
	u8	ext_switch_polarity;
};

struct wifi_link_info_8822b_1ant {
	u8	num_of_active_port;
	u32	port_connect_status;
	boolean is_all_under_5g;
	boolean is_mcc_25g;
	boolean is_p2p_connected;
	boolean is_connected;
};

/* *******************************************
 * The following is interface which will notify coex module.
 * ********************************************/
void ex_halbtc8822b1ant_power_on_setting(struct btc_coexist *btc);
void ex_halbtc8822b1ant_pre_load_firmware(struct btc_coexist *btc);
void ex_halbtc8822b1ant_init_hw_config(struct btc_coexist *btc,
				       boolean wifi_only);
void ex_halbtc8822b1ant_init_coex_dm(struct btc_coexist *btc);
void ex_halbtc8822b1ant_ips_notify(struct btc_coexist *btc, u8 type);
void ex_halbtc8822b1ant_lps_notify(struct btc_coexist *btc, u8 type);
void ex_halbtc8822b1ant_scan_notify(struct btc_coexist *btc, u8 type);
void ex_halbtc8822b1ant_scan_notify_without_bt(struct btc_coexist *btc,
					       u8 type);
void ex_halbtc8822b1ant_switchband_notify(struct btc_coexist *btc,
					  u8 type);
void ex_halbtc8822b1ant_switchband_notify_without_bt(struct btc_coexist *btc,
						     u8 type);
void ex_halbtc8822b1ant_connect_notify(struct btc_coexist *btc, u8 type);
void ex_halbtc8822b1ant_media_status_notify(struct btc_coexist *btc,
					    u8 type);
void ex_halbtc8822b1ant_specific_packet_notify(struct btc_coexist *btc,
					       u8 type);
void ex_halbtc8822b1ant_bt_info_notify(struct btc_coexist *btc,
				       u8 *tmp_buf, u8 length);
void ex_halbtc8822b1ant_wl_fwdbginfo_notify(struct btc_coexist *btc,
					    u8 *tmp_buf, u8 length);
void ex_halbtc8822b1ant_rx_rate_change_notify(struct btc_coexist *btc,
					      BOOLEAN is_data_frame,
					      u8 btc_rate_id);
void ex_halbtc8822b1ant_tx_rate_change_notify(struct btc_coexist *btc,
					      u8 tx_rate,
					      u8 tx_retry_ratio, u8 macid);
void ex_halbtc8822b1ant_rf_status_notify(struct btc_coexist *btc, u8 type);
void ex_halbtc8822b1ant_halt_notify(struct btc_coexist *btc);
void ex_halbtc8822b1ant_pnp_notify(struct btc_coexist *btc, u8 pnp_state);
void ex_halbtc8822b1ant_score_board_status_notify(struct btc_coexist *btc,
						  u8 *tmp_buf, u8 length);
void ex_halbtc8822b1ant_coex_dm_reset(struct btc_coexist *btc);
void ex_halbtc8822b1ant_periodical(struct btc_coexist *btc);
void ex_halbtc8822b1ant_display_simple_coex_info(struct btc_coexist *btc);
void ex_halbtc8822b1ant_display_coex_info(struct btc_coexist *btc);
void ex_halbtc8822b1ant_dbg_control(struct btc_coexist *btc, u8 op_code,
				    u8 op_len, u8 *pdata);

#else
#define ex_halbtc8822b1ant_power_on_setting(btc)
#define ex_halbtc8822b1ant_pre_load_firmware(btc)
#define ex_halbtc8822b1ant_init_hw_config(btc, wifi_only)
#define ex_halbtc8822b1ant_init_coex_dm(btc)
#define ex_halbtc8822b1ant_ips_notify(btc, type)
#define ex_halbtc8822b1ant_lps_notify(btc, type)
#define ex_halbtc8822b1ant_scan_notify(btc, type)
#define ex_halbtc8822b1ant_scan_notify_without_bt(btc, type)
#define ex_halbtc8822b1ant_switchband_notify(btc, type)
#define ex_halbtc8822b1ant_switchband_notify_without_bt(btc, type)
#define ex_halbtc8822b1ant_connect_notify(btc, type)
#define ex_halbtc8822b1ant_media_status_notify(btc, type)
#define ex_halbtc8822b1ant_specific_packet_notify(btc, type)
#define ex_halbtc8822b1ant_bt_info_notify(btc, tmp_buf, length)
#define ex_halbtc8822b1ant_wl_fwdbginfo_notify(btc, tmp_buf, length)
#define ex_halbtc8822b1ant_rx_rate_change_notify(btc, is_data_frame,     \
						 btc_rate_id)
#define ex_halbtc8822b1ant_tx_rate_change_notify(btcoexist, tx_rate,     \
						tx_retry_ratio, macid)
#define ex_halbtc8822b1ant_rf_status_notify(btc, type)
#define ex_halbtc8822b1ant_halt_notify(btc)
#define ex_halbtc8822b1ant_pnp_notify(btc, pnp_state)
#define ex_halbtc8822b1ant_score_board_status_notify(btc, tmp_buf, length)
#define ex_halbtc8822b1ant_coex_dm_reset(btc)
#define ex_halbtc8822b1ant_periodical(btc)
#define ex_halbtc8822b1ant_display_coex_info(btc)
#define ex_halbtc8822b1ant_dbg_control(btc, op_code, op_len, pdata)
#endif
#else

void
ex_halbtc8822b1ant_init_hw_config_without_bt(struct btc_coexist *btc);
void ex_halbtc8822b1ant_switch_band_without_bt(struct btc_coexist *btc,
					       boolean wifi_only_5g);

#endif
