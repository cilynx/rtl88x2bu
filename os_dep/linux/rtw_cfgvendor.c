/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

#include <drv_types.h>

#ifdef CONFIG_IOCTL_CFG80211


/*
#include <linux/kernel.h>
#include <linux/if_arp.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/netdevice.h>
#include <linux/sched.h>
#include <linux/etherdevice.h>
#include <linux/wireless.h>
#include <linux/ieee80211.h>
#include <linux/wait.h>
#include <net/cfg80211.h>
*/

#include <net/rtnetlink.h>

#ifdef DBG_MEM_ALLOC
extern bool match_mstat_sniff_rules(const enum mstat_f flags, const size_t size);
struct sk_buff *dbg_rtw_cfg80211_vendor_event_alloc(struct wiphy *wiphy, struct wireless_dev *wdev, int len, int event_id, gfp_t gfp
		, const enum mstat_f flags, const char *func, const int line)
{
	struct sk_buff *skb;
	unsigned int truesize = 0;

	skb = cfg80211_vendor_event_alloc(wiphy, wdev, len, event_id, gfp);

	if (skb)
		truesize = skb->truesize;

	if (!skb || truesize < len || match_mstat_sniff_rules(flags, truesize))
		RTW_INFO("DBG_MEM_ALLOC %s:%d %s(%d), skb:%p, truesize=%u\n", func, line, __FUNCTION__, len, skb, truesize);

	rtw_mstat_update(
		flags
		, skb ? MSTAT_ALLOC_SUCCESS : MSTAT_ALLOC_FAIL
		, truesize
	);

	return skb;
}

void dbg_rtw_cfg80211_vendor_event(struct sk_buff *skb, gfp_t gfp
		   , const enum mstat_f flags, const char *func, const int line)
{
	unsigned int truesize = skb->truesize;

	if (match_mstat_sniff_rules(flags, truesize))
		RTW_INFO("DBG_MEM_ALLOC %s:%d %s, truesize=%u\n", func, line, __FUNCTION__, truesize);

	cfg80211_vendor_event(skb, gfp);

	rtw_mstat_update(
		flags
		, MSTAT_FREE
		, truesize
	);
}

struct sk_buff *dbg_rtw_cfg80211_vendor_cmd_alloc_reply_skb(struct wiphy *wiphy, int len
		, const enum mstat_f flags, const char *func, const int line)
{
	struct sk_buff *skb;
	unsigned int truesize = 0;

	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len);

	if (skb)
		truesize = skb->truesize;

	if (!skb || truesize < len || match_mstat_sniff_rules(flags, truesize))
		RTW_INFO("DBG_MEM_ALLOC %s:%d %s(%d), skb:%p, truesize=%u\n", func, line, __FUNCTION__, len, skb, truesize);

	rtw_mstat_update(
		flags
		, skb ? MSTAT_ALLOC_SUCCESS : MSTAT_ALLOC_FAIL
		, truesize
	);

	return skb;
}

int dbg_rtw_cfg80211_vendor_cmd_reply(struct sk_buff *skb
	      , const enum mstat_f flags, const char *func, const int line)
{
	unsigned int truesize = skb->truesize;
	int ret;

	if (match_mstat_sniff_rules(flags, truesize))
		RTW_INFO("DBG_MEM_ALLOC %s:%d %s, truesize=%u\n", func, line, __FUNCTION__, truesize);

	ret = cfg80211_vendor_cmd_reply(skb);

	rtw_mstat_update(
		flags
		, MSTAT_FREE
		, truesize
	);

	return ret;
}

#define rtw_cfg80211_vendor_event_alloc(wiphy, wdev, len, event_id, gfp) \
	dbg_rtw_cfg80211_vendor_event_alloc(wiphy, wdev, len, event_id, gfp, MSTAT_FUNC_CFG_VENDOR | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)

#define rtw_cfg80211_vendor_event(skb, gfp) \
	dbg_rtw_cfg80211_vendor_event(skb, gfp, MSTAT_FUNC_CFG_VENDOR | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)

#define rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len) \
	dbg_rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len, MSTAT_FUNC_CFG_VENDOR | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)

#define rtw_cfg80211_vendor_cmd_reply(skb) \
	dbg_rtw_cfg80211_vendor_cmd_reply(skb, MSTAT_FUNC_CFG_VENDOR | MSTAT_TYPE_SKB, __FUNCTION__, __LINE__)
#else

struct sk_buff *rtw_cfg80211_vendor_event_alloc(
	struct wiphy *wiphy, struct wireless_dev *wdev, int len, int event_id, gfp_t gfp)
{
	struct sk_buff *skb;

	skb = cfg80211_vendor_event_alloc(wiphy, wdev, len, event_id, gfp);
	return skb;
}

#define rtw_cfg80211_vendor_event(skb, gfp) \
	cfg80211_vendor_event(skb, gfp)

#define rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len) \
	cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len)

#define rtw_cfg80211_vendor_cmd_reply(skb) \
	cfg80211_vendor_cmd_reply(skb)
#endif /* DBG_MEM_ALLOC */

/*
 * This API is to be used for asynchronous vendor events. This
 * shouldn't be used in response to a vendor command from its
 * do_it handler context (instead rtw_cfgvendor_send_cmd_reply should
 * be used).
 */
int rtw_cfgvendor_send_async_event(struct wiphy *wiphy,
	   struct net_device *dev, int event_id, const void  *data, int len)
{
	u16 kflags;
	struct sk_buff *skb;

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_event_alloc(wiphy, ndev_to_wdev(dev), len, event_id, kflags);
	if (!skb) {
		RTW_ERR(FUNC_NDEV_FMT" skb alloc failed", FUNC_NDEV_ARG(dev));
		return -ENOMEM;
	}

	/* Push the data to the skb */
	nla_put_nohdr(skb, len, data);

	rtw_cfg80211_vendor_event(skb, kflags);

	return 0;
}

static int rtw_cfgvendor_send_cmd_reply(struct wiphy *wiphy,
			struct net_device *dev, const void  *data, int len)
{
	struct sk_buff *skb;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, len);
	if (unlikely(!skb)) {
		RTW_ERR(FUNC_NDEV_FMT" skb alloc failed", FUNC_NDEV_ARG(dev));
		return -ENOMEM;
	}

	/* Push the data to the skb */
	nla_put_nohdr(skb, len, data);

	return rtw_cfg80211_vendor_cmd_reply(skb);
}

/* Feature enums */
#define WIFI_FEATURE_INFRA              0x0001      // Basic infrastructure mode
#define WIFI_FEATURE_INFRA_5G           0x0002      // Support for 5 GHz Band
#define WIFI_FEATURE_HOTSPOT            0x0004      // Support for GAS/ANQP
#define WIFI_FEATURE_P2P                0x0008      // Wifi-Direct
#define WIFI_FEATURE_SOFT_AP            0x0010      // Soft AP
#define WIFI_FEATURE_GSCAN              0x0020      // Google-Scan APIs
#define WIFI_FEATURE_NAN                0x0040      // Neighbor Awareness Networking
#define WIFI_FEATURE_D2D_RTT            0x0080      // Device-to-device RTT
#define WIFI_FEATURE_D2AP_RTT           0x0100      // Device-to-AP RTT
#define WIFI_FEATURE_BATCH_SCAN         0x0200      // Batched Scan (legacy)
#define WIFI_FEATURE_PNO                0x0400      // Preferred network offload
#define WIFI_FEATURE_ADDITIONAL_STA     0x0800      // Support for two STAs
#define WIFI_FEATURE_TDLS               0x1000      // Tunnel directed link setup
#define WIFI_FEATURE_TDLS_OFFCHANNEL    0x2000      // Support for TDLS off channel
#define WIFI_FEATURE_EPR                0x4000      // Enhanced power reporting
#define WIFI_FEATURE_AP_STA             0x8000      // Support for AP STA Concurrency
#define WIFI_FEATURE_LINK_LAYER_STATS   0x10000     // Link layer stats collection
#define WIFI_FEATURE_LOGGER             0x20000     // WiFi Logger
#define WIFI_FEATURE_HAL_EPNO           0x40000     // WiFi PNO enhanced
#define WIFI_FEATURE_RSSI_MONITOR       0x80000     // RSSI Monitor
#define WIFI_FEATURE_MKEEP_ALIVE        0x100000    // WiFi mkeep_alive
#define WIFI_FEATURE_CONFIG_NDO         0x200000    // ND offload configure
#define WIFI_FEATURE_TX_TRANSMIT_POWER  0x400000    // Capture Tx transmit power levels
#define WIFI_FEATURE_CONTROL_ROAMING    0x800000    // Enable/Disable firmware roaming
#define WIFI_FEATURE_IE_WHITELIST       0x1000000   // Support Probe IE white listing
#define WIFI_FEATURE_SCAN_RAND          0x2000000   // Support MAC & Probe Sequence Number randomization
// Add more features here

#define MAX_FEATURE_SET_CONCURRRENT_GROUPS  3

#include <hal_data.h>
int rtw_dev_get_feature_set(struct net_device *dev)
{
	_adapter *adapter = (_adapter *)rtw_netdev_priv(dev);
	HAL_DATA_TYPE *HalData = GET_HAL_DATA(adapter);
	int feature_set = 0;

	feature_set |= WIFI_FEATURE_INFRA;

#if CONFIG_IEEE80211_BAND_5GHZ
	if (is_supported_5g(adapter_to_regsty(adapter)->wireless_mode))
		feature_set |= WIFI_FEATURE_INFRA_5G;
#endif

	feature_set |= WIFI_FEATURE_P2P;
	feature_set |= WIFI_FEATURE_SOFT_AP;

	feature_set |= WIFI_FEATURE_ADDITIONAL_STA;
#ifdef CONFIG_RTW_CFGVEDNOR_LLSTATS
	feature_set |= WIFI_FEATURE_LINK_LAYER_STATS;
#endif /* CONFIG_RTW_CFGVEDNOR_LLSTATS */

#ifdef CONFIG_RTW_CFGVEDNOR_RSSIMONITOR
        feature_set |= WIFI_FEATURE_RSSI_MONITOR;
#endif

#ifdef CONFIG_RTW_CFGVENDOR_WIFI_LOGGER
	feature_set |= WIFI_FEATURE_LOGGER;
#endif

#ifdef CONFIG_RTW_WIFI_HAL
	feature_set |= WIFI_FEATURE_CONFIG_NDO;
	feature_set |= WIFI_FEATURE_SCAN_RAND;
#endif

	return feature_set;
}

int *rtw_dev_get_feature_set_matrix(struct net_device *dev, int *num)
{
	int feature_set_full, mem_needed;
	int *ret;

	*num = 0;
	mem_needed = sizeof(int) * MAX_FEATURE_SET_CONCURRRENT_GROUPS;
	ret = (int *)rtw_malloc(mem_needed);

	if (!ret) {
		RTW_ERR(FUNC_NDEV_FMT" failed to allocate %d bytes\n"
			, FUNC_NDEV_ARG(dev), mem_needed);
		return ret;
	}

	feature_set_full = rtw_dev_get_feature_set(dev);

	ret[0] = (feature_set_full & WIFI_FEATURE_INFRA) |
		 (feature_set_full & WIFI_FEATURE_INFRA_5G) |
		 (feature_set_full & WIFI_FEATURE_NAN) |
		 (feature_set_full & WIFI_FEATURE_D2D_RTT) |
		 (feature_set_full & WIFI_FEATURE_D2AP_RTT) |
		 (feature_set_full & WIFI_FEATURE_PNO) |
		 (feature_set_full & WIFI_FEATURE_BATCH_SCAN) |
		 (feature_set_full & WIFI_FEATURE_GSCAN) |
		 (feature_set_full & WIFI_FEATURE_HOTSPOT) |
		 (feature_set_full & WIFI_FEATURE_ADDITIONAL_STA) |
		 (feature_set_full & WIFI_FEATURE_EPR);

	ret[1] = (feature_set_full & WIFI_FEATURE_INFRA) |
		 (feature_set_full & WIFI_FEATURE_INFRA_5G) |
		 /* Not yet verified NAN with P2P */
		 /* (feature_set_full & WIFI_FEATURE_NAN) | */
		 (feature_set_full & WIFI_FEATURE_P2P) |
		 (feature_set_full & WIFI_FEATURE_D2AP_RTT) |
		 (feature_set_full & WIFI_FEATURE_D2D_RTT) |
		 (feature_set_full & WIFI_FEATURE_EPR);

	ret[2] = (feature_set_full & WIFI_FEATURE_INFRA) |
		 (feature_set_full & WIFI_FEATURE_INFRA_5G) |
		 (feature_set_full & WIFI_FEATURE_NAN) |
		 (feature_set_full & WIFI_FEATURE_D2D_RTT) |
		 (feature_set_full & WIFI_FEATURE_D2AP_RTT) |
		 (feature_set_full & WIFI_FEATURE_TDLS) |
		 (feature_set_full & WIFI_FEATURE_TDLS_OFFCHANNEL) |
		 (feature_set_full & WIFI_FEATURE_EPR);
	*num = MAX_FEATURE_SET_CONCURRRENT_GROUPS;

	return ret;
}

static int rtw_cfgvendor_get_feature_set(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	int reply;

	reply = rtw_dev_get_feature_set(wdev_to_ndev(wdev));

	err =  rtw_cfgvendor_send_cmd_reply(wiphy, wdev_to_ndev(wdev), &reply, sizeof(int));

	if (unlikely(err))
		RTW_ERR(FUNC_NDEV_FMT" Vendor Command reply failed ret:%d\n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);

	return err;
}

static int rtw_cfgvendor_get_feature_set_matrix(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	struct sk_buff *skb;
	int *reply;
	int num, mem_needed, i;

	reply = rtw_dev_get_feature_set_matrix(wdev_to_ndev(wdev), &num);

	if (!reply) {
		RTW_ERR(FUNC_NDEV_FMT" Could not get feature list matrix\n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)));
		err = -EINVAL;
		return err;
	}

	mem_needed = VENDOR_REPLY_OVERHEAD + (ATTRIBUTE_U32_LEN * num) +
		     ATTRIBUTE_U32_LEN;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_cmd_alloc_reply_skb(wiphy, mem_needed);
	if (unlikely(!skb)) {
		RTW_ERR(FUNC_NDEV_FMT" skb alloc failed", FUNC_NDEV_ARG(wdev_to_ndev(wdev)));
		err = -ENOMEM;
		goto exit;
	}

	nla_put_u32(skb, ANDR_WIFI_ATTRIBUTE_NUM_FEATURE_SET, num);
	for (i = 0; i < num; i++)
		nla_put_u32(skb, ANDR_WIFI_ATTRIBUTE_FEATURE_SET, reply[i]);

	err =  rtw_cfg80211_vendor_cmd_reply(skb);

	if (unlikely(err))
		RTW_ERR(FUNC_NDEV_FMT" Vendor Command reply failed ret:%d\n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);
exit:
	rtw_mfree((u8 *)reply, sizeof(int) * num);
	return err;
}



#ifdef CONFIG_RTW_CFGVEDNOR_LLSTATS
enum {
    LSTATS_SUBCMD_GET_INFO = ANDROID_NL80211_SUBCMD_LSTATS_RANGE_START,
	LSTATS_SUBCMD_SET_INFO,
	LSTATS_SUBCMD_CLEAR_INFO,
};
static void LinkLayerStats(_adapter *padapter)
{
	struct xmit_priv		*pxmitpriv = &(padapter->xmitpriv);
	struct recv_priv		*precvpriv = &(padapter->recvpriv);
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
	u32 ps_time, trx_total_time;
	u64 tx_bytes, rx_bytes, trx_total_bytes = 0;
	u64 tmp = 0;

	RTW_DBG("%s adapter type : %u\n", __func__, padapter->adapter_type);

	tx_bytes = 0;
	rx_bytes = 0;
	ps_time = 0;
	trx_total_time = 0;

	if ( padapter->netif_up == _TRUE ) {

		pwrpriv->on_time = rtw_get_passing_time_ms(pwrpriv->radio_on_start_time);

		if (rtw_mi_check_fwstate(padapter, _FW_LINKED)) {
			if ( pwrpriv->bpower_saving == _TRUE ) {
				pwrpriv->pwr_saving_time += rtw_get_passing_time_ms(pwrpriv->pwr_saving_start_time);
				pwrpriv->pwr_saving_start_time = rtw_get_current_time();
			}
		} else {
#ifdef CONFIG_IPS
			if ( pwrpriv->bpower_saving == _TRUE ) {
				pwrpriv->pwr_saving_time += rtw_get_passing_time_ms(pwrpriv->pwr_saving_start_time);
				pwrpriv->pwr_saving_start_time = rtw_get_current_time();
			}
#else
			pwrpriv->pwr_saving_time = pwrpriv->on_time;
#endif
		}

		ps_time = pwrpriv->pwr_saving_time;

		/* Deviation caused by caculation start time */
		if ( ps_time > pwrpriv->on_time )
			ps_time = pwrpriv->on_time;

		tx_bytes = pdvobjpriv->traffic_stat.last_tx_bytes;
		rx_bytes = pdvobjpriv->traffic_stat.last_rx_bytes;
		trx_total_bytes = tx_bytes + rx_bytes;

		trx_total_time = pwrpriv->on_time - ps_time;

		if ( trx_total_bytes == 0) {
			pwrpriv->tx_time = 0;
			pwrpriv->rx_time = 0;
		} else {

			/* tx_time = (trx_total_time * tx_total_bytes) / trx_total_bytes; */
			/* rx_time = (trx_total_time * rx_total_bytes) / trx_total_bytes; */

			tmp = (tx_bytes * trx_total_time);
			tmp = rtw_division64(tmp, trx_total_bytes);
			pwrpriv->tx_time = tmp;

			tmp = (rx_bytes * trx_total_time);
			tmp = rtw_division64(tmp, trx_total_bytes);
			pwrpriv->rx_time = tmp;

		}

	}
	else {
			pwrpriv->on_time = 0;
			pwrpriv->tx_time = 0;
			pwrpriv->rx_time = 0;
	}

#ifdef CONFIG_RTW_WIFI_HAL_DEBUG
	RTW_INFO("- tx_bytes : %llu rx_bytes : %llu total bytes : %llu\n", tx_bytes, rx_bytes, trx_total_bytes);
	RTW_INFO("- netif_up = %s, on_time : %u ms\n", padapter->netif_up ? "1":"0", pwrpriv->on_time);
	RTW_INFO("- pwr_saving_time : %u (%u) ms\n", pwrpriv->pwr_saving_time, ps_time);
	RTW_INFO("- trx_total_time : %u ms\n", trx_total_time);
	RTW_INFO("- tx_time : %u ms\n", pwrpriv->tx_time);
	RTW_INFO("- rx_time : %u ms\n", pwrpriv->rx_time);
#endif /* CONFIG_RTW_WIFI_HAL_DEBUG */

}

#define DUMMY_TIME_STATICS 99
static int rtw_cfgvendor_lstats_get_info(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	_adapter *padapter = GET_PRIMARY_ADAPTER(wiphy_to_adapter(wiphy));
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	wifi_radio_stat_internal *radio;
	wifi_iface_stat *iface;
	char *output;

	output = rtw_malloc(sizeof(wifi_radio_stat_internal) + sizeof(wifi_iface_stat));
	if (output == NULL) {
		RTW_DBG("Allocate lstats info buffer fail!\n");
	}

	radio = (wifi_radio_stat_internal *)output;

	radio->num_channels = 0;
	radio->radio = 1;

	/* to get on_time, tx_time, rx_time */
	LinkLayerStats(padapter);

	radio->on_time = pwrpriv->on_time;
	radio->tx_time = pwrpriv->tx_time;
	radio->rx_time = pwrpriv->rx_time;
	radio->on_time_scan = 0;
	radio->on_time_nbd = 0;
	radio->on_time_gscan = 0;
	radio->on_time_pno_scan = 0;
	radio->on_time_hs20 = 0;
	#ifdef CONFIG_RTW_WIFI_HAL_DEBUG
	RTW_INFO("==== %s ====\n", __func__);
	RTW_INFO("radio->radio : %d\n", (radio->radio));
	RTW_INFO("pwrpriv->on_time : %u ms\n", (pwrpriv->on_time));
	RTW_INFO("pwrpriv->tx_time :  %u ms\n", (pwrpriv->tx_time));
	RTW_INFO("pwrpriv->rx_time :  %u ms\n", (pwrpriv->rx_time));
	RTW_INFO("radio->on_time :  %u ms\n", (radio->on_time));
	RTW_INFO("radio->tx_time :  %u ms\n", (radio->tx_time));
	RTW_INFO("radio->rx_time :  %u ms\n", (radio->rx_time));
	#endif /* CONFIG_RTW_WIFI_HAL_DEBUG */

	RTW_DBG(FUNC_NDEV_FMT" %s\n", FUNC_NDEV_ARG(wdev_to_ndev(wdev)), (char*)data);
	err =  rtw_cfgvendor_send_cmd_reply(wiphy, wdev_to_ndev(wdev),
		output, sizeof(wifi_iface_stat) + sizeof(wifi_radio_stat_internal));
	if (unlikely(err))
		RTW_ERR(FUNC_NDEV_FMT"Vendor Command reply failed ret:%d \n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);
	rtw_mfree(output, sizeof(wifi_iface_stat) + sizeof(wifi_radio_stat_internal));
	return err;
}
static int rtw_cfgvendor_lstats_set_info(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	RTW_INFO("%s\n", __func__);
	return err;
}
static int rtw_cfgvendor_lstats_clear_info(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	RTW_INFO("%s\n", __func__);
	return err;
}
#endif /* CONFIG_RTW_CFGVEDNOR_LLSTATS */
#ifdef CONFIG_RTW_CFGVEDNOR_RSSIMONITOR
static int rtw_cfgvendor_set_rssi_monitor(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
        _adapter *padapter = GET_PRIMARY_ADAPTER(wiphy_to_adapter(wiphy));
        struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);

        struct recv_priv *precvpriv = &padapter->recvpriv;
	int err = 0, rem, type;
        const struct nlattr *iter;

        RTW_DBG(FUNC_NDEV_FMT" %s\n", FUNC_NDEV_ARG(wdev_to_ndev(wdev)), (char*)data);

	nla_for_each_attr(iter, data, len, rem) {
		type = nla_type(iter);

		switch (type) {
        		case RSSI_MONITOR_ATTRIBUTE_MAX_RSSI:
                                pwdev_priv->rssi_monitor_max = (s8)nla_get_u32(iter);;
	        		break;
		        case RSSI_MONITOR_ATTRIBUTE_MIN_RSSI:
                                pwdev_priv->rssi_monitor_min = (s8)nla_get_u32(iter);
			        break;
        		case RSSI_MONITOR_ATTRIBUTE_START:
                                pwdev_priv->rssi_monitor_enable = (u8)nla_get_u32(iter);
	        		break;
		}
	}

	return err;
}

void rtw_cfgvendor_rssi_monitor_evt(_adapter *padapter) {
	struct wireless_dev *wdev =  padapter->rtw_wdev;
	struct wiphy *wiphy= wdev->wiphy;
        struct recv_priv *precvpriv = &padapter->recvpriv;
	struct	mlme_priv	*pmlmepriv = &(padapter->mlmepriv);
	struct	wlan_network	*pcur_network = &pmlmepriv->cur_network;
        struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(padapter);
	struct sk_buff *skb;
	u32 tot_len = NLMSG_DEFAULT_SIZE;
	gfp_t kflags;
        rssi_monitor_evt data ;
        s8 rssi = precvpriv->rssi;

        if (pwdev_priv->rssi_monitor_enable == 0 || check_fwstate(pmlmepriv, _FW_LINKED) != _TRUE)
                return;

        if (rssi < pwdev_priv->rssi_monitor_max || rssi > pwdev_priv->rssi_monitor_min)
                return;

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;

	/* Alloc the SKB for vendor_event */
	skb = rtw_cfg80211_vendor_event_alloc(wiphy, wdev, tot_len, GOOGLE_RSSI_MONITOR_EVENT, kflags);
	if (!skb) {
		goto exit;
	}

        _rtw_memset(&data, 0, sizeof(data));

        data.version = RSSI_MONITOR_EVT_VERSION;
        data.cur_rssi = rssi;
        _rtw_memcpy(data.BSSID, pcur_network->network.MacAddress, sizeof(mac_addr));

        nla_append(skb, sizeof(data), &data);

	rtw_cfg80211_vendor_event(skb, kflags);
exit:
	return;
}
#endif /* CONFIG_RTW_CFGVEDNOR_RSSIMONITR */

#ifdef CONFIG_RTW_CFGVENDOR_WIFI_LOGGER
static int rtw_cfgvendor_logger_start_logging(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = 0, rem, type;
	char ring_name[32] = {0};
	int log_level = 0, flags = 0, time_intval = 0, threshold = 0;
	const struct nlattr *iter;

	nla_for_each_attr(iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
			case LOGGER_ATTRIBUTE_RING_NAME:
				memcpy(ring_name, nla_data(iter),
					MIN(sizeof(ring_name) -1, nla_len(iter)));
				break;
			case LOGGER_ATTRIBUTE_LOG_LEVEL:
				log_level = nla_get_u32(iter);
				break;
			case LOGGER_ATTRIBUTE_RING_FLAGS:
				flags = nla_get_u32(iter);
				break;
			case LOGGER_ATTRIBUTE_LOG_TIME_INTVAL:
				time_intval = nla_get_u32(iter);
				break;
			case LOGGER_ATTRIBUTE_LOG_MIN_DATA_SIZE:
				threshold = nla_get_u32(iter);
				break;
			default:
				RTW_ERR("Unknown type: %d\n", type);
				ret = WIFI_ERROR_INVALID_ARGS;
				goto exit;
		}
	}

exit:
	return ret;
}
static int rtw_cfgvendor_logger_get_feature(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void *data, int len)
{
	int err = 0;
	u32 supported_features = 0;

	err =  rtw_cfgvendor_send_cmd_reply(wiphy, wdev_to_ndev(wdev), &supported_features, sizeof(supported_features));

	if (unlikely(err))
		RTW_ERR(FUNC_NDEV_FMT" Vendor Command reply failed ret:%d\n"
			, FUNC_NDEV_ARG(wdev_to_ndev(wdev)), err);

	return err;
}
static int rtw_cfgvendor_logger_get_version(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void *data, int len)
{
	_adapter *padapter = GET_PRIMARY_ADAPTER(wiphy_to_adapter(wiphy));
	HAL_DATA_TYPE *hal = GET_HAL_DATA(padapter);
	int ret = 0, rem, type;
	int buf_len = 1024;
	char *buf_ptr;
	const struct nlattr *iter;
	gfp_t kflags;

	kflags = in_atomic() ? GFP_ATOMIC : GFP_KERNEL;
	buf_ptr = kzalloc(buf_len, kflags);
	if (!buf_ptr) {
		RTW_ERR("failed to allocate the buffer for version n");
		ret = -ENOMEM;
		goto exit;
	}
	nla_for_each_attr(iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
			case LOGGER_ATTRIBUTE_GET_DRIVER:
				memcpy(buf_ptr, DRIVERVERSION, strlen(DRIVERVERSION)+1);
				break;
			case LOGGER_ATTRIBUTE_GET_FW:
				sprintf(buf_ptr, "v%d.%d", hal->firmware_version, hal->firmware_sub_version);
				break;
			default:
				RTW_ERR("Unknown type: %d\n", type);
				ret = -EINVAL;
				goto exit;
		}
	}
	if (ret < 0) {
		RTW_ERR("failed to get the version %d\n", ret);
		goto exit;
	}


	ret =  rtw_cfgvendor_send_cmd_reply(wiphy, wdev_to_ndev(wdev), buf_ptr, strlen(buf_ptr));
exit:
	kfree(buf_ptr);
	return ret;
}

static int rtw_cfgvendor_logger_get_ring_status(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = 0;
	int ring_id;
	char ring_buf_name[] = "RTW_RING_BUFFER";

	struct sk_buff *skb;
	wifi_ring_buffer_status ring_status;


	_rtw_memcpy(ring_status.name, ring_buf_name, strlen(ring_buf_name)+1);
	ring_status.ring_id = 1;
	/* Alloc the SKB for vendor_event */
	skb = cfg80211_vendor_cmd_alloc_reply_skb(wiphy,
		sizeof(wifi_ring_buffer_status));
	if (!skb) {
		RTW_ERR("skb allocation is failed\n");
		ret = FAIL;
		goto exit;
	}

	nla_put_u32(skb, LOGGER_ATTRIBUTE_RING_NUM, 1);
	nla_put(skb, LOGGER_ATTRIBUTE_RING_STATUS, sizeof(wifi_ring_buffer_status),
				&ring_status);
	ret = cfg80211_vendor_cmd_reply(skb);

	if (ret) {
		RTW_ERR("Vendor Command reply failed ret:%d \n", ret);
	}
exit:
	return ret;
}

static int rtw_cfgvendor_logger_get_ring_data(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = 0, rem, type;
	char ring_name[32] = {0};
	const struct nlattr *iter;

	nla_for_each_attr(iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
			case LOGGER_ATTRIBUTE_RING_NAME:
				memcpy(ring_name, nla_data(iter),
					MIN(sizeof(ring_name) -1, nla_len(iter)));
				RTW_INFO(" %s LOGGER_ATTRIBUTE_RING_NAME : %s\n", __func__, ring_name);
				break;
			default:
				RTW_ERR("Unknown type: %d\n", type);
				return ret;
		}
	}


	return ret;
}

static int rtw_cfgvendor_logger_get_firmware_memory_dump(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = WIFI_ERROR_NOT_SUPPORTED;

	return ret;
}

static int rtw_cfgvendor_logger_start_pkt_fate_monitoring(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = WIFI_SUCCESS;

	return ret;
}

static int rtw_cfgvendor_logger_get_tx_pkt_fates(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = WIFI_SUCCESS;

	return ret;
}

static int rtw_cfgvendor_logger_get_rx_pkt_fates(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
	int ret = WIFI_SUCCESS;

	return ret;
}

#endif /* CONFIG_RTW_CFGVENDOR_WIFI_LOGGER */
#ifdef CONFIG_RTW_WIFI_HAL
#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI

#ifndef ETHER_ISMULTI
#define ETHER_ISMULTI(ea) (((const u8 *)(ea))[0] & 1)
#endif


static u8 null_addr[ETH_ALEN] = {0};
static void rtw_hal_random_gen_mac_addr(u8 *mac_addr)
{
	do {
		get_random_bytes(&mac_addr[3], ETH_ALEN-3);
		if (memcmp(mac_addr, null_addr, ETH_ALEN) != 0)
			break;
	} while(1);
}

void rtw_hal_pno_random_gen_mac_addr(PADAPTER adapter)
{
	u8 mac_addr[ETH_ALEN];
	struct rtw_wdev_priv *pwdev_priv = adapter_wdev_data(adapter);

	memcpy(mac_addr, pwdev_priv->pno_mac_addr, ETH_ALEN);
	if (mac_addr[0] == 0xFF) return;
	rtw_hal_random_gen_mac_addr(mac_addr);
	memcpy(pwdev_priv->pno_mac_addr, mac_addr, ETH_ALEN);
#ifdef CONFIG_RTW_DEBUG
	print_hex_dump(KERN_DEBUG, "pno_mac_addr: ",
		       DUMP_PREFIX_OFFSET, 16, 1, pwdev_priv->pno_mac_addr,
		       ETH_ALEN, 1);
#endif
}

void rtw_hal_set_hw_mac_addr(PADAPTER adapter, u8 *mac_addr)
{
	rtw_ps_deny(adapter, PS_DENY_IOCTL);
	LeaveAllPowerSaveModeDirect(adapter);

#ifdef CONFIG_MI_WITH_MBSSID_CAM
	rtw_hal_change_macaddr_mbid(adapter, mac_addr);
#else
	rtw_hal_set_hwreg(adapter, HW_VAR_MAC_ADDR, mac_addr);
#endif
#ifdef CONFIG_RTW_DEBUG
	rtw_hal_dump_macaddr(RTW_DBGDUMP, adapter);
#endif
	rtw_ps_deny_cancel(adapter, PS_DENY_IOCTL);
}

static int rtw_cfgvendor_set_rand_mac_oui(struct wiphy *wiphy,
		struct wireless_dev *wdev, const void  *data, int len)
{
	int err = 0;
	PADAPTER adapter;
	void *devaddr;
	struct net_device *netdev;
	int type, mac_len;
	u8 pno_random_mac_oui[3];
	u8 mac_addr[ETH_ALEN] = {0};
	struct pwrctrl_priv *pwrctl;
	struct rtw_wdev_priv *pwdev_priv;

	type = nla_type(data);
	mac_len = nla_len(data);
	if (mac_len != 3) {
		RTW_ERR("%s oui len error %d != 3\n", __func__, mac_len);
		return -1;
	}

	if (type == ANDR_WIFI_ATTRIBUTE_RANDOM_MAC_OUI) {
		memcpy(pno_random_mac_oui, nla_data(data), 3);
		print_hex_dump(KERN_DEBUG, "pno_random_mac_oui: ",
			       DUMP_PREFIX_OFFSET, 16, 1, pno_random_mac_oui,
			       3, 1);

		if (ETHER_ISMULTI(pno_random_mac_oui)) {
			pr_err("%s: oui is multicast address\n", __func__);
			return -1;
		}

		adapter = wiphy_to_adapter(wiphy);
		if (adapter == NULL) {
			pr_err("%s: wiphy_to_adapter == NULL\n", __func__);
			return -1;
		}

		pwdev_priv = adapter_wdev_data(adapter);

		memcpy(mac_addr, pno_random_mac_oui, 3);
		rtw_hal_random_gen_mac_addr(mac_addr);
		memcpy(pwdev_priv->pno_mac_addr, mac_addr, ETH_ALEN);
#ifdef CONFIG_RTW_DEBUG
		print_hex_dump(KERN_DEBUG, "pno_mac_addr: ",
			       DUMP_PREFIX_OFFSET, 16, 1, pwdev_priv->pno_mac_addr,
			       ETH_ALEN, 1);
#endif
	} else {
		RTW_ERR("%s oui type error %x != 0x2\n", __func__, type);
		err = -1;
	}


	return err;
}

#endif


static int rtw_cfgvendor_set_nodfs_flag(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void *data, int len)
{
	int err = 0;
	int type;
	u32 nodfs = 0;
	_adapter *padapter = GET_PRIMARY_ADAPTER(wiphy_to_adapter(wiphy));

	RTW_DBG(FUNC_NDEV_FMT" %s\n", FUNC_NDEV_ARG(wdev_to_ndev(wdev)), (char*)data);

	type = nla_type(data);
	if (type == ANDR_WIFI_ATTRIBUTE_NODFS_SET) {
		nodfs = nla_get_u32(data);
		adapter_to_dvobj(padapter)->nodfs = nodfs;
	} else {
		err = -EINVAL;
	}

	RTW_INFO("%s nodfs=%d, err=%d\n", __func__, nodfs, err);

	return err;
}

static int rtw_cfgvendor_set_country(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void  *data, int len)
{
#define CNTRY_BUF_SZ	4	/* Country string is 3 bytes + NUL */
	int err = 0, rem, type;
	char country_code[CNTRY_BUF_SZ] = {0};
	const struct nlattr *iter;
	_adapter *padapter = GET_PRIMARY_ADAPTER(wiphy_to_adapter(wiphy));

	RTW_DBG(FUNC_NDEV_FMT" %s\n", FUNC_NDEV_ARG(wdev_to_ndev(wdev)), (char*)data);

	nla_for_each_attr(iter, data, len, rem) {
		type = nla_type(iter);
		switch (type) {
			case ANDR_WIFI_ATTRIBUTE_COUNTRY:
				_rtw_memcpy(country_code, nla_data(iter),
					MIN(nla_len(iter), CNTRY_BUF_SZ));
				break;
			default:
				RTW_ERR("Unknown type: %d\n", type);
				return -EINVAL;
		}
	}

	RTW_INFO("%s country_code:\"%c%c\" \n", __func__, country_code[0], country_code[1]);

	rtw_set_country(padapter, country_code);

	return err;
}

static int rtw_cfgvendor_set_nd_offload(struct wiphy *wiphy,
	struct wireless_dev *wdev, const void *data, int len)
{
	int err = 0;
	int type;
	u8 nd_en = 0;
	_adapter *padapter = GET_PRIMARY_ADAPTER(wiphy_to_adapter(wiphy));

	RTW_DBG(FUNC_NDEV_FMT" %s\n", FUNC_NDEV_ARG(wdev_to_ndev(wdev)), (char*)data);

	type = nla_type(data);
	if (type == ANDR_WIFI_ATTRIBUTE_ND_OFFLOAD_VALUE) {
		nd_en = nla_get_u8(data);
		/* ND has been enabled when wow is enabled */
	} else {
		err = -EINVAL;
	}

	RTW_INFO("%s nd_en=%d, err=%d\n", __func__, nd_en, err);

	return err;
}
#endif /* CONFIG_RTW_WIFI_HAL */

static const struct wiphy_vendor_command rtw_vendor_cmds[] = {
#ifdef CONFIG_RTW_CFGVEDNOR_LLSTATS
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LSTATS_SUBCMD_GET_INFO
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_lstats_get_info
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LSTATS_SUBCMD_SET_INFO
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_lstats_set_info
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LSTATS_SUBCMD_CLEAR_INFO
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_lstats_clear_info
	},
#endif /* CONFIG_RTW_CFGVEDNOR_LLSTATS */
#ifdef CONFIG_RTW_CFGVEDNOR_RSSIMONITOR
        {
                {
                        .vendor_id = OUI_GOOGLE,
                        .subcmd = WIFI_SUBCMD_SET_RSSI_MONITOR
                },
                .flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
                .doit = rtw_cfgvendor_set_rssi_monitor
        },
#endif /* CONFIG_RTW_CFGVEDNOR_RSSIMONITOR */
#ifdef CONFIG_RTW_CFGVENDOR_WIFI_LOGGER
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_START_LOGGING
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_start_logging
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_GET_FEATURE
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_feature
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_GET_VER
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_version
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_GET_RING_STATUS
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_ring_status
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_GET_RING_DATA
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_ring_data
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_TRIGGER_MEM_DUMP
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_firmware_memory_dump
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_START_PKT_FATE_MONITORING
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_start_pkt_fate_monitoring
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_GET_TX_PKT_FATES
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_tx_pkt_fates
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = LOGGER_GET_RX_PKT_FATES
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_logger_get_rx_pkt_fates
	},
#endif /* CONFIG_RTW_CFGVENDOR_WIFI_LOGGER */
#ifdef CONFIG_RTW_WIFI_HAL
#ifdef CONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = WIFI_SUBCMD_SET_PNO_RANDOM_MAC_OUI
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_set_rand_mac_oui
	},
#endif
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = WIFI_SUBCMD_NODFS_SET
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_set_nodfs_flag

	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = WIFI_SUBCMD_SET_COUNTRY_CODE
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_set_country
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = WIFI_SUBCMD_CONFIG_ND_OFFLOAD
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_set_nd_offload
	},
#endif /* CONFIG_RTW_WIFI_HAL */
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = WIFI_SUBCMD_GET_FEATURE_SET
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_get_feature_set
	},
	{
		{
			.vendor_id = OUI_GOOGLE,
			.subcmd = WIFI_SUBCMD_GET_FEATURE_SET_MATRIX
		},
		.flags = WIPHY_VENDOR_CMD_NEED_WDEV | WIPHY_VENDOR_CMD_NEED_NETDEV,
		.doit = rtw_cfgvendor_get_feature_set_matrix
	}
};

static const struct  nl80211_vendor_cmd_info rtw_vendor_events[] = {

#ifdef CONFIG_RTW_CFGVEDNOR_RSSIMONITOR
	{ OUI_GOOGLE, GOOGLE_RSSI_MONITOR_EVENT },
#endif /* RTW_CFGVEDNOR_RSSIMONITR */

};

int rtw_cfgvendor_attach(struct wiphy *wiphy)
{

	RTW_INFO("Register RTW cfg80211 vendor cmd(0x%x) interface\n", NL80211_CMD_VENDOR);

	wiphy->vendor_commands	= rtw_vendor_cmds;
	wiphy->n_vendor_commands = ARRAY_SIZE(rtw_vendor_cmds);
	wiphy->vendor_events	= rtw_vendor_events;
	wiphy->n_vendor_events	= ARRAY_SIZE(rtw_vendor_events);

	return 0;
}

int rtw_cfgvendor_detach(struct wiphy *wiphy)
{
	RTW_INFO("Vendor: Unregister RTW cfg80211 vendor interface\n");

	wiphy->vendor_commands  = NULL;
	wiphy->vendor_events    = NULL;
	wiphy->n_vendor_commands = 0;
	wiphy->n_vendor_events  = 0;

	return 0;
}

#endif /* CONFIG_IOCTL_CFG80211 */
