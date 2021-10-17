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
#ifndef __RTL8188E_RECV_H__
#define __RTL8188E_RECV_H__

#define RECV_BLK_SZ 512
#define RECV_BLK_CNT 16
#define RECV_BLK_TH RECV_BLK_CNT

#if defined(CONFIG_USB_HCI)
	#ifndef MAX_RECVBUF_SZ
		#ifndef CONFIG_MINIMAL_MEMORY_USAGE
			/* #define MAX_RECVBUF_SZ (32768) */ /* 32k */
			/* #define MAX_RECVBUF_SZ (16384) */ /* 16K */
			/* #define MAX_RECVBUF_SZ (10240) */ /* 10K */
			#define MAX_RECVBUF_SZ (15360) /* 15k < 16k */
			/* #define MAX_RECVBUF_SZ (8192+1024) */ /* 8K+1k */
		#else
			#define MAX_RECVBUF_SZ (4000) /* about 4K */
		#endif
	#endif /* !MAX_RECVBUF_SZ */

#elif defined(CONFIG_PCI_HCI)
	/* #ifndef CONFIG_MINIMAL_MEMORY_USAGE */
	/*	#define MAX_RECVBUF_SZ (9100) */
	/* #else */
	#define MAX_RECVBUF_SZ (4000) /* about 4K
	* #endif */


#elif defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)

	#define MAX_RECVBUF_SZ (10240)

#endif

/* Rx smooth factor */
#define	Rx_Smooth_Factor (20)

#define TX_RPT1_PKT_LEN 8

typedef struct rxreport_8188e {
	/* Offset 0 */
	u32 pktlen:14;
	u32 crc32:1;
	u32 icverr:1;
	u32 drvinfosize:4;
	u32 security:3;
	u32 qos:1;
	u32 shift:2;
	u32 physt:1;
	u32 swdec:1;
	u32 ls:1;
	u32 fs:1;
	u32 eor:1;
	u32 own:1;

	/* Offset 4 */
	u32 macid:5;
	u32 tid:4;
	u32 hwrsvd:4;
	u32 amsdu:1;
	u32 paggr:1;
	u32 faggr:1;
	u32 a1fit:4;
	u32 a2fit:4;
	u32 pam:1;
	u32 pwr:1;
	u32 md:1;
	u32 mf:1;
	u32 type:2;
	u32 mc:1;
	u32 bc:1;

	/* Offset 8 */
	u32 seq:12;
	u32 frag:4;
	u32 nextpktlen:14;
	u32 nextind:1;
	u32 rsvd0831:1;

	/* Offset 12 */
	u32 rxmcs:6;
	u32 rxht:1;
	u32 gf:1;
	u32 splcp:1;
	u32 bw:1;
	u32 htc:1;
	u32 eosp:1;
	u32 bssidfit:2;
	u32 rpt_sel:2;
	u32 rsvd1216:13;
	u32 pattern_match:1;
	u32 unicastwake:1;
	u32 magicwake:1;

	/* Offset 16 */
	/*
	u32 pattern0match:1;
	u32 pattern1match:1;
	u32 pattern2match:1;
	u32 pattern3match:1;
	u32 pattern4match:1;
	u32 pattern5match:1;
	u32 pattern6match:1;
	u32 pattern7match:1;
	u32 pattern8match:1;
	u32 pattern9match:1;
	u32 patternamatch:1;
	u32 patternbmatch:1;
	u32 patterncmatch:1;
	u32 rsvd1613:19;
	*/
	u32 rsvd16;

	/* Offset 20 */
	u32 tsfl;

	/* Offset 24 */
	u32 bassn:12;
	u32 bavld:1;
	u32 rsvd2413:19;
} RXREPORT, *PRXREPORT;


#if defined(CONFIG_SDIO_HCI) || defined(CONFIG_GSPI_HCI)
	s32 rtl8188es_init_recv_priv(PADAPTER padapter);
	void rtl8188es_free_recv_priv(PADAPTER padapter);
#endif

#ifdef CONFIG_USB_HCI
	void rtl8188eu_init_recvbuf(_adapter *padapter, struct recv_buf *precvbuf);
	s32 rtl8188eu_init_recv_priv(PADAPTER padapter);
	void rtl8188eu_free_recv_priv(PADAPTER padapter);
#endif

#ifdef CONFIG_PCI_HCI
	s32 rtl8188ee_init_recv_priv(PADAPTER padapter);
	void rtl8188ee_free_recv_priv(PADAPTER padapter);
#endif

void rtl8188e_query_rx_desc_status(union recv_frame *precvframe, struct recv_stat *prxstat);

#endif /* __RTL8188E_RECV_H__ */
