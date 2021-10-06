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
#define _HCI_INTF_C_

#include <drv_types.h>
#include <hal_data.h>

#include <platform_ops.h>

#ifndef CONFIG_USB_HCI
#error "CONFIG_USB_HCI shall be on!\n"
#endif

#ifdef CONFIG_80211N_HT
extern int rtw_ht_enable;
extern int rtw_bw_mode;
extern int rtw_ampdu_enable;/* for enable tx_ampdu */
#endif

#ifdef CONFIG_GLOBAL_UI_PID
int ui_pid[3] = {0, 0, 0};
#endif


extern int pm_netdev_open(struct net_device *pnetdev, u8 bnormal);
static int rtw_suspend(struct usb_interface *intf, pm_message_t message);
static int rtw_resume(struct usb_interface *intf);


static int rtw_drv_init(struct usb_interface *pusb_intf, const struct usb_device_id *pdid);
static void rtw_dev_remove(struct usb_interface *pusb_intf);

static void rtw_dev_shutdown(struct device *dev)
{
	struct usb_interface *usb_intf = container_of(dev, struct usb_interface, dev);
	struct dvobj_priv *dvobj = NULL;
	_adapter *adapter = NULL;

	RTW_INFO("%s\n", __func__);

	if (usb_intf) {
		dvobj = usb_get_intfdata(usb_intf);
		if (dvobj) {
			adapter = dvobj_get_primary_adapter(dvobj);
			if (adapter) {
				if (!rtw_is_surprise_removed(adapter)) {
					#ifdef CONFIG_WOWLAN
					struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(adapter);

					#ifdef CONFIG_GPIO_WAKEUP
					/*default wake up pin change to BT*/
					RTW_INFO("%s:default wake up pin change to BT\n", __FUNCTION__);
					rtw_hal_switch_gpio_wl_ctrl(adapter, WAKEUP_GPIO_IDX, _FALSE);
					#endif /* CONFIG_GPIO_WAKEUP */

					if (pwrctl->wowlan_mode == _TRUE)
						RTW_PRINT("%s wowlan_mode ==_TRUE do not run rtw_hal_deinit()\n", __FUNCTION__);
					else
					#endif
					{
						rtw_hal_deinit(adapter);
						rtw_set_surprise_removed(adapter);
					}
				}
			}
			ATOMIC_SET(&dvobj->continual_io_error, MAX_CONTINUAL_IO_ERR + 1);
		}
	}
}

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 23))
/* Some useful macros to use to create struct usb_device_id */
#define USB_DEVICE_ID_MATCH_VENDOR			 0x0001
#define USB_DEVICE_ID_MATCH_PRODUCT			 0x0002
#define USB_DEVICE_ID_MATCH_DEV_LO			 0x0004
#define USB_DEVICE_ID_MATCH_DEV_HI			 0x0008
#define USB_DEVICE_ID_MATCH_DEV_CLASS			 0x0010
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS		 0x0020
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL		 0x0040
#define USB_DEVICE_ID_MATCH_INT_CLASS			 0x0080
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS		 0x0100
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL		 0x0200
#define USB_DEVICE_ID_MATCH_INT_NUMBER		 0x0400


#define USB_DEVICE_ID_MATCH_INT_INFO \
	(USB_DEVICE_ID_MATCH_INT_CLASS | \
	 USB_DEVICE_ID_MATCH_INT_SUBCLASS | \
	 USB_DEVICE_ID_MATCH_INT_PROTOCOL)


#define USB_DEVICE_AND_INTERFACE_INFO(vend, prod, cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
		       | USB_DEVICE_ID_MATCH_DEVICE, \
		       .idVendor = (vend), \
				   .idProduct = (prod), \
						.bInterfaceClass = (cl), \
						.bInterfaceSubClass = (sc), \
						.bInterfaceProtocol = (pr)

/**
 * USB_VENDOR_AND_INTERFACE_INFO - describe a specific usb vendor with a class of usb interfaces
 * @vend: the 16 bit USB Vendor ID
 * @cl: bInterfaceClass value
 * @sc: bInterfaceSubClass value
 * @pr: bInterfaceProtocol value
 *
 * This macro is used to create a struct usb_device_id that matches a
 * specific vendor with a specific class of interfaces.
 *
 * This is especially useful when explicitly matching devices that have
 * vendor specific bDeviceClass values, but standards-compliant interfaces.
 */
#define USB_VENDOR_AND_INTERFACE_INFO(vend, cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
		       | USB_DEVICE_ID_MATCH_VENDOR, \
		       .idVendor = (vend), \
				   .bInterfaceClass = (cl), \
						   .bInterfaceSubClass = (sc), \
						   .bInterfaceProtocol = (pr)

/* ----------------------------------------------------------------------- */
#endif


#define USB_VENDER_ID_REALTEK		0x0BDA


/* DID_USB_v916_20130116 */
static struct usb_device_id rtw_usb_id_tbl[] = {
#ifdef CONFIG_RTL8188E
	/*=== Realtek demoboard ===*/
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8179), .driver_info = RTL8188E}, /* 8188EUS */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x0179), .driver_info = RTL8188E}, /* 8188ETV */
	/*=== Customer ID ===*/
	/****** 8188EUS ********/
	{USB_DEVICE(0x07B8, 0x8179), .driver_info = RTL8188E}, /* Abocom - Abocom */
#endif

#ifdef CONFIG_RTL8812A
	/*=== Realtek demoboard ===*/
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8812), .driver_info = RTL8812}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x881A), .driver_info = RTL8812}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x881B), .driver_info = RTL8812}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x881C), .driver_info = RTL8812}, /* Default ID */
	/*=== Customer ID ===*/
	{USB_DEVICE(0x050D, 0x1106), .driver_info = RTL8812}, /* Belkin - sercomm */
	{USB_DEVICE(0x2001, 0x330E), .driver_info = RTL8812}, /* D-Link - ALPHA */
	{USB_DEVICE(0x7392, 0xA822), .driver_info = RTL8812}, /* Edimax - Edimax */
	{USB_DEVICE(0x0DF6, 0x0074), .driver_info = RTL8812}, /* Sitecom - Edimax */
	{USB_DEVICE(0x04BB, 0x0952), .driver_info = RTL8812}, /* I-O DATA - Edimax */
	{USB_DEVICE(0x0789, 0x016E), .driver_info = RTL8812}, /* Logitec - Edimax */
	{USB_DEVICE(0x0409, 0x0408), .driver_info = RTL8812}, /* NEC - */
	{USB_DEVICE(0x0B05, 0x17D2), .driver_info = RTL8812}, /* ASUS - Edimax */
	{USB_DEVICE(0x0E66, 0x0022), .driver_info = RTL8812}, /* HAWKING - Edimax */
	{USB_DEVICE(0x0586, 0x3426), .driver_info = RTL8812}, /* ZyXEL - */
	{USB_DEVICE(0x2001, 0x3313), .driver_info = RTL8812}, /* D-Link - ALPHA */
	{USB_DEVICE(0x1058, 0x0632), .driver_info = RTL8812}, /* WD - Cybertan*/
	{USB_DEVICE(0x1740, 0x0100), .driver_info = RTL8812}, /* EnGenius - EnGenius */
	{USB_DEVICE(0x2019, 0xAB30), .driver_info = RTL8812}, /* Planex - Abocom */
	{USB_DEVICE(0x07B8, 0x8812), .driver_info = RTL8812}, /* Abocom - Abocom */
	{USB_DEVICE(0x2001, 0x3315), .driver_info = RTL8812}, /* D-Link - Cameo */
	{USB_DEVICE(0x2001, 0x3316), .driver_info = RTL8812}, /* D-Link - Cameo */
#endif

#ifdef CONFIG_RTL8821A
	/*=== Realtek demoboard ===*/
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x0811), .driver_info = RTL8821}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x0821), .driver_info = RTL8821}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8822), .driver_info = RTL8821}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0xA811) , .driver_info = RTL8821},/* Default ID */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x0820, 0xff, 0xff, 0xff), .driver_info = RTL8821}, /* 8821AU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x0823, 0xff, 0xff, 0xff), .driver_info = RTL8821}, /* 8821AU */
	/*=== Customer ID ===*/
	{USB_DEVICE(0x7392, 0xA811), .driver_info = RTL8821}, /* Edimax - Edimax */
	{USB_DEVICE(0x04BB, 0x0953), .driver_info = RTL8821}, /* I-O DATA - Edimax */
	{USB_DEVICE(0x2001, 0x3314), .driver_info = RTL8821}, /* D-Link - Cameo */
	{USB_DEVICE(0x2001, 0x3318), .driver_info = RTL8821}, /* D-Link - Cameo */
	{USB_DEVICE(0x0E66, 0x0023), .driver_info = RTL8821}, /* HAWKING - Edimax */
	{USB_DEVICE(0x056E, 0x400E) , .driver_info = RTL8821}, /* ELECOM -  ELECOM */
	{USB_DEVICE(0x056E, 0x400F) , .driver_info = RTL8821}, /* ELECOM -  ELECOM */
	{USB_DEVICE(0x20f4, 0x804b), .driver_info = RTL8821}, /* TRENDnet  */
#endif

#ifdef CONFIG_RTL8192E
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x818B, 0xff, 0xff, 0xff), .driver_info = RTL8192E}, /* Default ID */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x818C, 0xff, 0xff, 0xff), .driver_info = RTL8192E}, /* Default ID */
#endif

#ifdef CONFIG_RTL8723B
	/* === Realtek demoboard === */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB720, 0xff, 0xff, 0xff), .driver_info = RTL8723B}, /* 8723BU 1*1 */
	/* {USB_DEVICE(USB_VENDER_ID_REALTEK, 0xB720),.driver_info = RTL8723B},  8723BU */
#endif

#ifdef CONFIG_RTL8703B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB703, 0xff, 0xff, 0xff), .driver_info = RTL8703B}, /* 8723CU 1*1 */
	/* {USB_DEVICE(USB_VENDER_ID_REALTEK, 0xB703), .driver_info = RTL723C}, */ /* 8723CU 1*1 */
#endif /* CONFIG_RTL8703B */

#ifdef CONFIG_RTL8814A

	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8813), .driver_info = RTL8814A},
	{USB_DEVICE(0x2001, 0x331a), .driver_info = RTL8814A}, /* D-Link - D-Link */
	{USB_DEVICE(0x0b05, 0x1817), .driver_info = RTL8814A}, /* ASUS - ASUSTeK */
	{USB_DEVICE(0x056E, 0x400B), .driver_info = RTL8814A}, /* ELECOM - ELECOM */
	{USB_DEVICE(0x056E, 0x400D), .driver_info = RTL8814A}, /* ELECOM - ELECOM */
	{USB_DEVICE(0x7392, 0xA834), .driver_info = RTL8814A}, /* Edimax - Edimax */
#endif /* CONFIG_RTL8814A */

#ifdef CONFIG_RTL8188F
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xF179, 0xff, 0xff, 0xff), .driver_info = RTL8188F}, /* 8188FU 1*1 */
#endif

#ifdef CONFIG_RTL8188GTV
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x018C, 0xff, 0xff, 0xff), .driver_info = RTL8188GTV}, /* 8188GTV 1*1 */
#endif

#ifdef CONFIG_RTL8822B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB82C, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Default ID for USB multi-function */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB812, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Default ID for USB Single-function, WiFi only */
	/*=== Customer ID ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x0846, 0x9055, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* NeatGear A6150 - AC1200 Dual Band WiFi USB Mini Adapter */
	{USB_DEVICE_AND_INTERFACE_INFO(0x0b05, 0x1841, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* ASUS AC1300 USB-AC55 B1 */
	{USB_DEVICE_AND_INTERFACE_INFO(0x0b05, 0x184c, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* ASUS USB-AC53 Nano */
	{USB_DEVICE_AND_INTERFACE_INFO(0x13b1, 0x0043, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Alpha - Alpha*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x331c, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Dlink DWA-182*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x331e, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Dlink DWA-181-A1*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x2001, 0x331f, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Dlink DWA-183 */
	{USB_DEVICE_AND_INTERFACE_INFO(0x2357, 0x0115, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* TP-Link Archer T4U V3 */
	{USB_DEVICE_AND_INTERFACE_INFO(0x2357, 0x012d, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* TP-Link Archer T3U V1 */
	{USB_DEVICE_AND_INTERFACE_INFO(0x2357, 0x0138, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* TP-Link Archer T3U Plus V1 */
	{USB_DEVICE_AND_INTERFACE_INFO(0x7392, 0xB822, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Edimax EW-7822ULC */
	{USB_DEVICE_AND_INTERFACE_INFO(0x7392, 0xC822, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Edimax EW-7822UTC */
#endif /* CONFIG_RTL8822B */

#ifdef CONFIG_RTL8723D
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xD723, 0xff, 0xff, 0xff), .driver_info = RTL8723D}, /* 8723DU 1*1 */
#endif

#ifdef CONFIG_RTL8192F
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xF192, 0xff, 0xff, 0xff), .driver_info = RTL8192F}, /* 8192FU 2*2 */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xA725, 0xff, 0xff, 0xff), .driver_info = RTL8192F}, /* 8725AU 2*2 */
#endif

#ifdef CONFIG_RTL8821C
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xb82b, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xb820, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC821, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC820, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82A, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82B, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC811, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8811CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x8811, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8811CU */
	/*=== Customer ID ===*/
#endif

#ifdef CONFIG_RTL8710B
	/*=== Realtek dongle ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB711, 0xff, 0xff, 0xff), .driver_info = RTL8710B}, /* 8710B = 8188GU 1*1 */
#endif

#ifdef CONFIG_RTL8822C
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82C, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Default ID for USB multi-function */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC812, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Default ID for USB Single-function, WiFi only */
	/*=== Customer ID ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x13b1, 0x0043, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Alpha - Alpha*/
#endif /* CONFIG_RTL8822C */

#ifdef CONFIG_RTL8814B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB814, 0xff, 0xff, 0xff), .driver_info = RTL8814B}, /* Default ID for USB multi-function */
#endif /* CONFIG_RTL8814B */

	{}	/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, rtw_usb_id_tbl);

int const rtw_usb_id_len = sizeof(rtw_usb_id_tbl) / sizeof(struct usb_device_id);

static struct specific_device_id specific_device_id_tbl[] = {
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x8177, .flags = SPEC_DEV_ID_DISABLE_HT}, /* 8188cu 1*1 dongole, (b/g mode only) */
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x817E, .flags = SPEC_DEV_ID_DISABLE_HT}, /* 8188CE-VAU USB minCard (b/g mode only) */
	{.idVendor = 0x0b05, .idProduct = 0x1791, .flags = SPEC_DEV_ID_DISABLE_HT},
	{.idVendor = 0x13D3, .idProduct = 0x3311, .flags = SPEC_DEV_ID_DISABLE_HT},
	{.idVendor = 0x13D3, .idProduct = 0x3359, .flags = SPEC_DEV_ID_DISABLE_HT}, /* Russian customer -Azwave (8188CE-VAU  g mode) */
#ifdef RTK_DMP_PLATFORM
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x8111, .flags = SPEC_DEV_ID_ASSIGN_IFNAME}, /* Realtek 5G dongle for WiFi Display */
	{.idVendor = 0x2019, .idProduct = 0xAB2D, .flags = SPEC_DEV_ID_ASSIGN_IFNAME}, /* PCI-Abocom 5G dongle for WiFi Display */
#endif /* RTK_DMP_PLATFORM */
	{}
};

struct rtw_usb_drv {
	struct usb_driver usbdrv;
	int drv_registered;
	u8 hw_type;
};

struct rtw_usb_drv usb_drv = {
	.usbdrv.name = (char *)DRV_NAME,
	.usbdrv.probe = rtw_drv_init,
	.usbdrv.disconnect = rtw_dev_remove,
	.usbdrv.id_table = rtw_usb_id_tbl,
	.usbdrv.suspend =  rtw_suspend,
	.usbdrv.resume = rtw_resume,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22))
	.usbdrv.reset_resume   = rtw_resume,
#endif
#ifdef CONFIG_AUTOSUSPEND
	.usbdrv.supports_autosuspend = 1,
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19))
	.usbdrv.drvwrap.driver.shutdown = rtw_dev_shutdown,
#else
	.usbdrv.driver.shutdown = rtw_dev_shutdown,
#endif
};

static inline int RT_usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN;
}

static inline int RT_usb_endpoint_dir_out(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT;
}

static inline int RT_usb_endpoint_xfer_int(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT;
}

static inline int RT_usb_endpoint_xfer_bulk(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK;
}

static inline int RT_usb_endpoint_is_bulk_in(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_bulk(epd) && RT_usb_endpoint_dir_in(epd);
}

static inline int RT_usb_endpoint_is_bulk_out(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_bulk(epd) && RT_usb_endpoint_dir_out(epd);
}

static inline int RT_usb_endpoint_is_int_in(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_int(epd) && RT_usb_endpoint_dir_in(epd);
}

static inline int RT_usb_endpoint_num(const struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

static u8 rtw_init_intf_priv(struct dvobj_priv *dvobj)
{
	u8 rst = _SUCCESS;

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_rtw_mutex_init(&dvobj->usb_vendor_req_mutex);
#endif


#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	dvobj->usb_alloc_vendor_req_buf = rtw_zmalloc(MAX_USB_IO_CTL_SIZE);
	if (dvobj->usb_alloc_vendor_req_buf == NULL) {
		RTW_INFO("alloc usb_vendor_req_buf failed... /n");
		rst = _FAIL;
		goto exit;
	}
	dvobj->usb_vendor_req_buf  =
		(u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(dvobj->usb_alloc_vendor_req_buf), ALIGNMENT_UNIT);
exit:
#endif

	return rst;

}

static u8 rtw_deinit_intf_priv(struct dvobj_priv *dvobj)
{
	u8 rst = _SUCCESS;

#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	if (dvobj->usb_vendor_req_buf)
		rtw_mfree(dvobj->usb_alloc_vendor_req_buf, MAX_USB_IO_CTL_SIZE);
#endif

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_rtw_mutex_free(&dvobj->usb_vendor_req_mutex);
#endif

	return rst;
}
static void rtw_decide_chip_type_by_usb_info(struct dvobj_priv *pdvobjpriv, const struct usb_device_id *pdid)
{
	pdvobjpriv->chip_type = pdid->driver_info;

#ifdef CONFIG_RTL8188E
	if (pdvobjpriv->chip_type == RTL8188E)
		rtl8188eu_set_hw_type(pdvobjpriv);
#endif

#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
	if (pdvobjpriv->chip_type == RTL8812 || pdvobjpriv->chip_type == RTL8821)
		rtl8812au_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8192E
	if (pdvobjpriv->chip_type == RTL8192E)
		rtl8192eu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8723B
	if (pdvobjpriv->chip_type == RTL8723B)
		rtl8723bu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8814A
	if (pdvobjpriv->chip_type == RTL8814A)
		rtl8814au_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8814A */

#ifdef CONFIG_RTL8188F
	if (pdvobjpriv->chip_type == RTL8188F)
		rtl8188fu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8188GTV
	if (pdvobjpriv->chip_type == RTL8188GTV)
		rtl8188gtvu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8703B
	if (pdvobjpriv->chip_type == RTL8703B)
		rtl8703bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8703B */

#ifdef CONFIG_RTL8822B
	if (pdvobjpriv->chip_type == RTL8822B)
		rtl8822bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8822B */

#ifdef CONFIG_RTL8723D
	if (pdvobjpriv->chip_type == RTL8723D)
		rtl8723du_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8723D */

#ifdef CONFIG_RTL8821C
	if (pdvobjpriv->chip_type == RTL8821C)
		rtl8821cu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8821C */

#ifdef CONFIG_RTL8710B
	if (pdvobjpriv->chip_type == RTL8710B)
		rtl8710bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8710B */

#ifdef CONFIG_RTL8192F
	if (pdvobjpriv->chip_type == RTL8192F)
		rtl8192fu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8192F */

#ifdef CONFIG_RTL8822C
	if (pdvobjpriv->chip_type == RTL8822C)
		rtl8822cu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8822C */

#ifdef CONFIG_RTL8814B
	if (pdvobjpriv->chip_type == RTL8814B)
		rtl8814bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8814B */
}

static struct dvobj_priv *usb_dvobj_init(struct usb_interface *usb_intf, const struct usb_device_id *pdid)
{
	int	i;
	int	status = _FAIL;
	struct dvobj_priv *pdvobjpriv;
	struct usb_device_descriptor	*pdev_desc;
	struct usb_host_config			*phost_conf;
	struct usb_config_descriptor		*pconf_desc;
	struct usb_host_interface		*phost_iface;
	struct usb_interface_descriptor	*piface_desc;
	struct usb_host_endpoint		*phost_endp;
	struct usb_endpoint_descriptor	*pendp_desc;
	struct usb_device				*pusbd;



	pdvobjpriv = devobj_init();
	if (pdvobjpriv == NULL)
		goto exit;


	pdvobjpriv->pusbintf = usb_intf ;
	pusbd = pdvobjpriv->pusbdev = interface_to_usbdev(usb_intf);
	usb_set_intfdata(usb_intf, pdvobjpriv);

	pdvobjpriv->RtNumInPipes = 0;
	pdvobjpriv->RtNumOutPipes = 0;

	/* padapter->EepromAddressSize = 6; */
	/* pdvobjpriv->nr_endpoint = 6; */

	pdev_desc = &pusbd->descriptor;

#if 0
	RTW_INFO("\n8712_usb_device_descriptor:\n");
	RTW_INFO("bLength=%x\n", pdev_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", pdev_desc->bDescriptorType);
	RTW_INFO("bcdUSB=%x\n", pdev_desc->bcdUSB);
	RTW_INFO("bDeviceClass=%x\n", pdev_desc->bDeviceClass);
	RTW_INFO("bDeviceSubClass=%x\n", pdev_desc->bDeviceSubClass);
	RTW_INFO("bDeviceProtocol=%x\n", pdev_desc->bDeviceProtocol);
	RTW_INFO("bMaxPacketSize0=%x\n", pdev_desc->bMaxPacketSize0);
	RTW_INFO("idVendor=%x\n", pdev_desc->idVendor);
	RTW_INFO("idProduct=%x\n", pdev_desc->idProduct);
	RTW_INFO("bcdDevice=%x\n", pdev_desc->bcdDevice);
	RTW_INFO("iManufacturer=%x\n", pdev_desc->iManufacturer);
	RTW_INFO("iProduct=%x\n", pdev_desc->iProduct);
	RTW_INFO("iSerialNumber=%x\n", pdev_desc->iSerialNumber);
	RTW_INFO("bNumConfigurations=%x\n", pdev_desc->bNumConfigurations);
#endif

	phost_conf = pusbd->actconfig;
	pconf_desc = &phost_conf->desc;


	/* RTW_INFO("\n***** num of altsetting = (%d) *****\n", pusb_interface->num_altsetting); */

	phost_iface = &usb_intf->altsetting[0];
	piface_desc = &phost_iface->desc;

	pdvobjpriv->nr_endpoint = piface_desc->bNumEndpoints;

	/* RTW_INFO("\ndump usb_endpoint_descriptor:\n"); */

	for (i = 0; i < pdvobjpriv->nr_endpoint; i++) {
		phost_endp = phost_iface->endpoint + i;
		if (phost_endp) {
			pendp_desc = &phost_endp->desc;

			RTW_INFO("\nusb_endpoint_descriptor(%d):\n", i);
			RTW_INFO("bLength=%x\n", pendp_desc->bLength);
			RTW_INFO("bDescriptorType=%x\n", pendp_desc->bDescriptorType);
			RTW_INFO("bEndpointAddress=%x\n", pendp_desc->bEndpointAddress);
			/* RTW_INFO("bmAttributes=%x\n",pendp_desc->bmAttributes); */
			RTW_INFO("wMaxPacketSize=%d\n", le16_to_cpu(pendp_desc->wMaxPacketSize));
			RTW_INFO("bInterval=%x\n", pendp_desc->bInterval);
			/* RTW_INFO("bRefresh=%x\n",pendp_desc->bRefresh); */
			/* RTW_INFO("bSynchAddress=%x\n",pendp_desc->bSynchAddress); */

			if (RT_usb_endpoint_is_bulk_in(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_bulk_in = %x\n", RT_usb_endpoint_num(pendp_desc));
				pdvobjpriv->RtInPipe[pdvobjpriv->RtNumInPipes] = RT_usb_endpoint_num(pendp_desc);
				pdvobjpriv->RtNumInPipes++;
			} else if (RT_usb_endpoint_is_int_in(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_int_in = %x, Interval = %x\n", RT_usb_endpoint_num(pendp_desc), pendp_desc->bInterval);
				pdvobjpriv->RtInPipe[pdvobjpriv->RtNumInPipes] = RT_usb_endpoint_num(pendp_desc);
				pdvobjpriv->RtNumInPipes++;
			} else if (RT_usb_endpoint_is_bulk_out(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_bulk_out = %x\n", RT_usb_endpoint_num(pendp_desc));
				pdvobjpriv->RtOutPipe[pdvobjpriv->RtNumOutPipes] = RT_usb_endpoint_num(pendp_desc);
				pdvobjpriv->RtNumOutPipes++;
			}
			pdvobjpriv->ep_num[i] = RT_usb_endpoint_num(pendp_desc);
		}
	}

	RTW_INFO("nr_endpoint=%d, in_num=%d, out_num=%d\n\n", pdvobjpriv->nr_endpoint, pdvobjpriv->RtNumInPipes, pdvobjpriv->RtNumOutPipes);

	switch (pusbd->speed) {
	case USB_SPEED_LOW:
		RTW_INFO("USB_SPEED_LOW\n");
		pdvobjpriv->usb_speed = RTW_USB_SPEED_1_1;
		break;
	case USB_SPEED_FULL:
		RTW_INFO("USB_SPEED_FULL\n");
		pdvobjpriv->usb_speed = RTW_USB_SPEED_1_1;
		break;
	case USB_SPEED_HIGH:
		RTW_INFO("USB_SPEED_HIGH\n");
		pdvobjpriv->usb_speed = RTW_USB_SPEED_2;
		break;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31))
	case USB_SPEED_SUPER:
		RTW_INFO("USB_SPEED_SUPER\n");
		pdvobjpriv->usb_speed = RTW_USB_SPEED_3;
		break;
#endif
	default:
		RTW_INFO("USB_SPEED_UNKNOWN(%x)\n", pusbd->speed);
		pdvobjpriv->usb_speed = RTW_USB_SPEED_UNKNOWN;
		break;
	}

	if (pdvobjpriv->usb_speed == RTW_USB_SPEED_UNKNOWN) {
		RTW_INFO("UNKNOWN USB SPEED MODE, ERROR !!!\n");
		goto free_dvobj;
	}

	if (rtw_init_intf_priv(pdvobjpriv) == _FAIL) {
		goto free_dvobj;
	}

	/*step 1-1., decide the chip_type via driver_info*/
	pdvobjpriv->interface_type = RTW_USB;
	rtw_decide_chip_type_by_usb_info(pdvobjpriv, pdid);

	/* .3 misc */
	_rtw_init_sema(&(pdvobjpriv->usb_suspend_sema), 0);
	rtw_reset_continual_io_error(pdvobjpriv);

	usb_get_dev(pusbd);

	status = _SUCCESS;

free_dvobj:
	if (status != _SUCCESS && pdvobjpriv) {
		usb_set_intfdata(usb_intf, NULL);

		devobj_deinit(pdvobjpriv);

		pdvobjpriv = NULL;
	}
exit:
	return pdvobjpriv;
}

static void usb_dvobj_deinit(struct usb_interface *usb_intf)
{
	struct dvobj_priv *dvobj = usb_get_intfdata(usb_intf);


	usb_set_intfdata(usb_intf, NULL);
	if (dvobj) {
		rtw_deinit_intf_priv(dvobj);
		devobj_deinit(dvobj);
	}

	/* RTW_INFO("%s %d\n", __func__, ATOMIC_READ(&usb_intf->dev.kobj.kref.refcount)); */
	usb_put_dev(interface_to_usbdev(usb_intf));

}

static int usb_reprobe_switch_usb_mode(PADAPTER Adapter)
{
	struct registry_priv *registry_par = &Adapter->registrypriv;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(Adapter);
	u8 ret = _FALSE;

	/* efuse not allow driver to switch usb mode */
	if (pHalData->EEPROMUsbSwitch == _FALSE)
		goto exit;

	/* registry not allow driver to switch usb mode */
	if (registry_par->switch_usb_mode == 0)
		goto exit;

	rtw_hal_set_hwreg(Adapter, HW_VAR_USB_MODE, &ret);

exit:
	return ret;
}

u8 rtw_set_hal_ops(_adapter *padapter)
{
	/* alloc memory for HAL DATA */
	if (rtw_hal_data_init(padapter) == _FAIL)
		return _FAIL;

#ifdef CONFIG_RTL8188E
	if (rtw_get_chip_type(padapter) == RTL8188E)
		rtl8188eu_set_hal_ops(padapter);
#endif

#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
	if (rtw_get_chip_type(padapter) == RTL8812 || rtw_get_chip_type(padapter) == RTL8821)
		rtl8812au_set_hal_ops(padapter);
#endif

#ifdef CONFIG_RTL8192E
	if (rtw_get_chip_type(padapter) == RTL8192E)
		rtl8192eu_set_hal_ops(padapter);
#endif
#ifdef CONFIG_RTL8723B
	if (rtw_get_chip_type(padapter) == RTL8723B)
		rtl8723bu_set_hal_ops(padapter);
#endif
#ifdef CONFIG_RTL8814A
	if (rtw_get_chip_type(padapter) == RTL8814A)
		rtl8814au_set_hal_ops(padapter);
#endif /* CONFIG_RTL8814A */

#ifdef CONFIG_RTL8188F
	if (rtw_get_chip_type(padapter) == RTL8188F)
		rtl8188fu_set_hal_ops(padapter);
#endif

#ifdef CONFIG_RTL8188GTV
	if (rtw_get_chip_type(padapter) == RTL8188GTV)
		rtl8188gtvu_set_hal_ops(padapter);
#endif

#ifdef CONFIG_RTL8703B
	if (rtw_get_chip_type(padapter) == RTL8703B)
		rtl8703bu_set_hal_ops(padapter);
#endif /* CONFIG_RTL8703B */

#ifdef CONFIG_RTL8822B
	if (rtw_get_chip_type(padapter) == RTL8822B)
		rtl8822bu_set_hal_ops(padapter);
#endif /* CONFIG_RTL8822B */

#ifdef CONFIG_RTL8723D
	if (rtw_get_chip_type(padapter) == RTL8723D)
		rtl8723du_set_hal_ops(padapter);
#endif /* CONFIG_RTL8723D */


#ifdef CONFIG_RTL8821C
	if (rtw_get_chip_type(padapter) == RTL8821C) {
		if (rtl8821cu_set_hal_ops(padapter) == _FAIL)
			return _FAIL;
	}
#endif

#ifdef CONFIG_RTL8710B
	if (rtw_get_chip_type(padapter) == RTL8710B)
		rtl8710bu_set_hal_ops(padapter);
#endif /* CONFIG_RTL8710B */


#ifdef CONFIG_RTL8192F
	if (rtw_get_chip_type(padapter) == RTL8192F)
		rtl8192fu_set_hal_ops(padapter);
#endif

#ifdef CONFIG_RTL8822C
	if (rtw_get_chip_type(padapter) == RTL8822C)
		rtl8822cu_set_hal_ops(padapter);
#endif /* CONFIG_RTL8822C */

#ifdef CONFIG_RTL8814B
	if (rtw_get_chip_type(padapter) == RTL8814B)
		rtl8814bu_set_hal_ops(padapter);
#endif /* CONFIG_RTL8814B */


	if (_FAIL == rtw_hal_ops_check(padapter))
		return _FAIL;

	if (hal_spec_init(padapter) == _FAIL)
		return _FAIL;

	return _SUCCESS;
}

static void usb_intf_start(_adapter *padapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(padapter);

	rtw_hal_inirp_init(padapter);
	hal->usb_intf_start = _TRUE;


}

static void usb_intf_stop(_adapter *padapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(padapter);

	/* disabel_hw_interrupt */
	if (!rtw_is_surprise_removed(padapter)) {
		/* device still exists, so driver can do i/o operation */
		/* TODO: */
	}

	/* cancel in irp */
	rtw_hal_inirp_deinit(padapter);

	/* cancel out irp */
	rtw_write_port_cancel(padapter);

	/* todo:cancel other irps */

	hal->usb_intf_start = _FALSE;

}

static void process_spec_devid(const struct usb_device_id *pdid)
{
	u16 vid, pid;
	u32 flags;
	int i;
	int num = sizeof(specific_device_id_tbl) / sizeof(struct specific_device_id);

	for (i = 0; i < num; i++) {
		vid = specific_device_id_tbl[i].idVendor;
		pid = specific_device_id_tbl[i].idProduct;
		flags = specific_device_id_tbl[i].flags;

#ifdef CONFIG_80211N_HT
		if ((pdid->idVendor == vid) && (pdid->idProduct == pid) && (flags & SPEC_DEV_ID_DISABLE_HT)) {
			rtw_ht_enable = 0;
			rtw_bw_mode = 0;
			rtw_ampdu_enable = 0;
		}
#endif

#ifdef RTK_DMP_PLATFORM
		/* Change the ifname to wlan10 when PC side WFD dongle plugin on DMP platform. */
		/* It is used to distinguish between normal and PC-side wifi dongle/module. */
		if ((pdid->idVendor == vid) && (pdid->idProduct == pid) && (flags & SPEC_DEV_ID_ASSIGN_IFNAME)) {
			extern char *ifname;
			strncpy(ifname, "wlan10", 6);
			/* RTW_INFO("%s()-%d: ifname=%s, vid=%04X, pid=%04X\n", __FUNCTION__, __LINE__, ifname, vid, pid); */
		}
#endif /* RTK_DMP_PLATFORM */

	}
}

#ifdef SUPPORT_HW_RFOFF_DETECTED
int rtw_hw_suspend(_adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv;
	struct usb_interface *pusb_intf;
	struct net_device *pnetdev;

	if (NULL == padapter)
		goto error_exit;

	if ((_FALSE == padapter->bup) || RTW_CANNOT_RUN(padapter)) {
		RTW_INFO("padapter->bup=%d bDriverStopped=%s bSurpriseRemoved = %s\n"
			 , padapter->bup
			 , rtw_is_drv_stopped(padapter) ? "True" : "False"
			, rtw_is_surprise_removed(padapter) ? "True" : "False");
		goto error_exit;
	}

	pwrpriv = adapter_to_pwrctl(padapter);
	pusb_intf = adapter_to_dvobj(padapter)->pusbintf;
	pnetdev = padapter->pnetdev;

	LeaveAllPowerSaveMode(padapter);

	RTW_INFO("==> rtw_hw_suspend\n");
	_enter_pwrlock(&pwrpriv->lock);
	pwrpriv->bips_processing = _TRUE;
	/* padapter->net_closed = _TRUE; */
	/* s1. */
	if (pnetdev) {
		rtw_netif_carrier_off(pnetdev);
		rtw_netif_stop_queue(pnetdev);
	}

	/* s2. */
	rtw_disassoc_cmd(padapter, 500, RTW_CMDF_DIRECTLY);

	/* s2-2.  indicate disconnect to os */
	/* rtw_indicate_disconnect(padapter); */
	{
		struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;
		if (check_fwstate(pmlmepriv, _FW_LINKED)) {
			_clr_fwstate_(pmlmepriv, _FW_LINKED);
			rtw_led_control(padapter, LED_CTL_NO_LINK);

			rtw_os_indicate_disconnect(padapter, 0, _FALSE);

#ifdef CONFIG_LPS
			/* donnot enqueue cmd */
			rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_DISCONNECT, RTW_CMDF_DIRECTLY);
#endif
		}
	}
	/* s2-3. */
	rtw_free_assoc_resources(padapter, _TRUE);

	/* s2-4. */
	rtw_free_network_queue(padapter, _TRUE);
#ifdef CONFIG_IPS
	rtw_ips_dev_unload(padapter);
#endif
	pwrpriv->rf_pwrstate = rf_off;
	pwrpriv->bips_processing = _FALSE;
	_exit_pwrlock(&pwrpriv->lock);

	return 0;

error_exit:
	RTW_INFO("%s, failed\n", __FUNCTION__);
	return -1;

}

int rtw_hw_resume(_adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct usb_interface *pusb_intf = adapter_to_dvobj(padapter)->pusbintf;
	struct net_device *pnetdev = padapter->pnetdev;

	RTW_INFO("==> rtw_hw_resume\n");
	_enter_pwrlock(&pwrpriv->lock);
	pwrpriv->bips_processing = _TRUE;
	rtw_reset_drv_sw(padapter);

	if (pm_netdev_open(pnetdev, _FALSE) != 0) {
		_exit_pwrlock(&pwrpriv->lock);
		goto error_exit;
	}
	rtw_netif_device_attach(pnetdev);
	rtw_netif_carrier_on(pnetdev);

	rtw_netif_wake_queue(pnetdev);

	pwrpriv->bkeepfwalive = _FALSE;
	pwrpriv->brfoffbyhw = _FALSE;

	pwrpriv->rf_pwrstate = rf_on;
	pwrpriv->bips_processing = _FALSE;
	_exit_pwrlock(&pwrpriv->lock);


	return 0;
error_exit:
	RTW_INFO("%s, Open net dev failed\n", __FUNCTION__);
	return -1;
}
#endif

static int rtw_suspend(struct usb_interface *pusb_intf, pm_message_t message)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;
	PADAPTER padapter;
	int ret = 0;


	dvobj = usb_get_intfdata(pusb_intf);
	pwrpriv = dvobj_to_pwrctl(dvobj);
	pdbgpriv = &dvobj->drv_dbg;
	padapter = dvobj_get_primary_adapter(dvobj);

	if (pwrpriv->bInSuspend == _TRUE) {
		RTW_INFO("%s bInSuspend = %d\n", __FUNCTION__, pwrpriv->bInSuspend);
		pdbgpriv->dbg_suspend_error_cnt++;
		goto exit;
	}

	if ((padapter->bup) || !rtw_is_drv_stopped(padapter) || !rtw_is_surprise_removed(padapter)) {
#ifdef CONFIG_AUTOSUSPEND
		if (pwrpriv->bInternalAutoSuspend) {

#ifdef SUPPORT_HW_RFOFF_DETECTED
			/* The FW command register update must after MAC and FW init ready. */
			if ((GET_HAL_DATA(padapter)->bFWReady) && (pwrpriv->bHWPwrPindetect) && (padapter->registrypriv.usbss_enable)) {
				u8 bOpen = _TRUE;
				rtw_interface_ps_func(padapter, HAL_USB_SELECT_SUSPEND, &bOpen);
			}
#endif/* SUPPORT_HW_RFOFF_DETECTED */
		}
#endif/* CONFIG_AUTOSUSPEND */
	}

	ret =  rtw_suspend_common(padapter);

exit:
	return ret;
}

int rtw_resume_process(_adapter *padapter)
{
	int ret;
#if defined(CONFIG_BT_COEXIST) && defined(CONFIG_AUTOSUSPEND)
	int pm_cnt = 0;
#endif
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct dvobj_priv *pdvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobj->drv_dbg;


	if (pwrpriv->bInSuspend == _FALSE) {
		pdbgpriv->dbg_resume_error_cnt++;
		RTW_INFO("%s bInSuspend = %d\n", __FUNCTION__, pwrpriv->bInSuspend);
		return -1;
	}

#if defined(CONFIG_BT_COEXIST) && defined(CONFIG_AUTOSUSPEND) /* add by amy for 8723as-vau */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
	RTW_INFO("%s...pm_usage_cnt(%d)  pwrpriv->bAutoResume=%x.  ....\n", __func__, atomic_read(&(adapter_to_dvobj(padapter)->pusbintf->pm_usage_cnt)), pwrpriv->bAutoResume);
	pm_cnt = atomic_read(&(adapter_to_dvobj(padapter)->pusbintf->pm_usage_cnt));
#else /* kernel < 2.6.32 */
	RTW_INFO("...pm_usage_cnt(%d).....\n", adapter_to_dvobj(padapter)->pusbintf->pm_usage_cnt);
	pm_cnt = adapter_to_dvobj(padapter)->pusbintf->pm_usage_cnt;
#endif /* kernel < 2.6.32 */

	RTW_INFO("pwrpriv->bAutoResume (%x)\n", pwrpriv->bAutoResume);
	if (_TRUE == pwrpriv->bAutoResume) {
		pwrpriv->bInternalAutoSuspend = _FALSE;
		pwrpriv->bAutoResume = _FALSE;
		RTW_INFO("pwrpriv->bAutoResume (%x)  pwrpriv->bInternalAutoSuspend(%x)\n", pwrpriv->bAutoResume, pwrpriv->bInternalAutoSuspend);

	}
#endif /* #ifdef CONFIG_BT_COEXIST &CONFIG_AUTOSUSPEND& */

	/*
	 * Due to usb wow suspend flow will cancel read/write port via intf_stop and
	 * bReadPortCancel and bWritePortCancel are set _TRUE in intf_stop.
	 * But they will not be clear in intf_start during wow resume flow.
	 * It should move to os_intf in the feature.
	 */
	RTW_ENABLE_FUNC(padapter, DF_RX_BIT);
	RTW_ENABLE_FUNC(padapter, DF_TX_BIT);

	ret =  rtw_resume_common(padapter);

#ifdef CONFIG_AUTOSUSPEND
	if (pwrpriv->bInternalAutoSuspend) {
#ifdef SUPPORT_HW_RFOFF_DETECTED
		/* The FW command register update must after MAC and FW init ready. */
		if ((GET_HAL_DATA(padapter)->bFWReady) && (pwrpriv->bHWPwrPindetect) && (padapter->registrypriv.usbss_enable)) {
			u8 bOpen = _FALSE;
			rtw_interface_ps_func(padapter, HAL_USB_SELECT_SUSPEND, &bOpen);
		}
#endif
#ifdef CONFIG_BT_COEXIST /* for 8723as-vau */
		RTW_INFO("pwrpriv->bAutoResume (%x)\n", pwrpriv->bAutoResume);
		if (_TRUE == pwrpriv->bAutoResume) {
			pwrpriv->bInternalAutoSuspend = _FALSE;
			pwrpriv->bAutoResume = _FALSE;
			RTW_INFO("pwrpriv->bAutoResume (%x)  pwrpriv->bInternalAutoSuspend(%x)\n", pwrpriv->bAutoResume, pwrpriv->bInternalAutoSuspend);
		}

#else	/* #ifdef CONFIG_BT_COEXIST */
		pwrpriv->bInternalAutoSuspend = _FALSE;
#endif	/* #ifdef CONFIG_BT_COEXIST */
		pwrpriv->brfoffbyhw = _FALSE;
	}
#endif/* CONFIG_AUTOSUSPEND */


	return ret;
}

static int rtw_resume(struct usb_interface *pusb_intf)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;
	PADAPTER padapter;
	struct mlme_ext_priv *pmlmeext;
	int ret = 0;


	dvobj = usb_get_intfdata(pusb_intf);
	pwrpriv = dvobj_to_pwrctl(dvobj);
	pdbgpriv = &dvobj->drv_dbg;
	padapter = dvobj_get_primary_adapter(dvobj);
	pmlmeext = &padapter->mlmeextpriv;

	RTW_INFO("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid);
	pdbgpriv->dbg_resume_cnt++;

	#ifdef CONFIG_AUTOSUSPEND
	if (pwrpriv->bInternalAutoSuspend)
		ret = rtw_resume_process(padapter);
	else
	#endif
	{
		if (pwrpriv->wowlan_mode || pwrpriv->wowlan_ap_mode) {
			rtw_resume_lock_suspend();
			ret = rtw_resume_process(padapter);
			rtw_resume_unlock_suspend();
		} else {
#ifdef CONFIG_RESUME_IN_WORKQUEUE
			rtw_resume_in_workqueue(pwrpriv);
#else
			if (rtw_is_earlysuspend_registered(pwrpriv)) {
				/* jeff: bypass resume here, do in late_resume */
				rtw_set_do_late_resume(pwrpriv, _TRUE);
			} else {
				rtw_resume_lock_suspend();
				ret = rtw_resume_process(padapter);
				rtw_resume_unlock_suspend();
			}
#endif
		}
	}

	pmlmeext->last_scan_time = rtw_get_current_time();
	RTW_INFO("<========  %s return %d\n", __FUNCTION__, ret);

	return ret;
}



#ifdef CONFIG_AUTOSUSPEND
void autosuspend_enter(_adapter *padapter)
{
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);

	RTW_INFO("==>autosuspend_enter...........\n");

	pwrpriv->bInternalAutoSuspend = _TRUE;
	pwrpriv->bips_processing = _TRUE;

	if (rf_off == pwrpriv->change_rfpwrstate) {
#ifndef	CONFIG_BT_COEXIST
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
		usb_enable_autosuspend(dvobj->pusbdev);
#else
		dvobj->pusbdev->autosuspend_disabled = 0;/* autosuspend disabled by the user */
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
		usb_autopm_put_interface(dvobj->pusbintf);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
		usb_autopm_enable(dvobj->pusbintf);
#else
		usb_autosuspend_device(dvobj->pusbdev, 1);
#endif
#else	/* #ifndef	CONFIG_BT_COEXIST */
		if (1 == pwrpriv->autopm_cnt) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
			usb_enable_autosuspend(dvobj->pusbdev);
#else
			dvobj->pusbdev->autosuspend_disabled = 0;/* autosuspend disabled by the user */
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
			usb_autopm_put_interface(dvobj->pusbintf);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
			usb_autopm_enable(dvobj->pusbintf);
#else
			usb_autosuspend_device(dvobj->pusbdev, 1);
#endif
			pwrpriv->autopm_cnt--;
		} else
			RTW_INFO("0!=pwrpriv->autopm_cnt[%d]   didn't usb_autopm_put_interface\n", pwrpriv->autopm_cnt);

#endif	/* #ifndef	CONFIG_BT_COEXIST */
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
	RTW_INFO("...pm_usage_cnt(%d).....\n", atomic_read(&(dvobj->pusbintf->pm_usage_cnt)));
#else
	RTW_INFO("...pm_usage_cnt(%d).....\n", dvobj->pusbintf->pm_usage_cnt);
#endif

}

int autoresume_enter(_adapter *padapter)
{
	int result = _SUCCESS;
	struct security_priv *psecuritypriv = &(padapter->securitypriv);
	struct mlme_ext_priv	*pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct pwrctrl_priv *pwrpriv = dvobj_to_pwrctl(dvobj);

	RTW_INFO("====> autoresume_enter\n");

	if (rf_off == pwrpriv->rf_pwrstate) {
		pwrpriv->ps_flag = _FALSE;
#ifndef	CONFIG_BT_COEXIST
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
		if (usb_autopm_get_interface(dvobj->pusbintf) < 0) {
			RTW_INFO("can't get autopm: %d\n", result);
			result = _FAIL;
			goto error_exit;
		}
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
		usb_autopm_disable(dvobj->pusbintf);
#else
		usb_autoresume_device(dvobj->pusbdev, 1);
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
		RTW_INFO("...pm_usage_cnt(%d).....\n", atomic_read(&(dvobj->pusbintf->pm_usage_cnt)));
#else
		RTW_INFO("...pm_usage_cnt(%d).....\n", dvobj->pusbintf->pm_usage_cnt);
#endif
#else	/* #ifndef	CONFIG_BT_COEXIST */
		pwrpriv->bAutoResume = _TRUE;
		if (0 == pwrpriv->autopm_cnt) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
			if (usb_autopm_get_interface(dvobj->pusbintf) < 0) {
				RTW_INFO("can't get autopm: %d\n", result);
				result = _FAIL;
				goto error_exit;
			}
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
			usb_autopm_disable(dvobj->pusbintf);
#else
			usb_autoresume_device(dvobj->pusbdev, 1);
#endif
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
			RTW_INFO("...pm_usage_cnt(%d).....\n", atomic_read(&(dvobj->pusbintf->pm_usage_cnt)));
#else
			RTW_INFO("...pm_usage_cnt(%d).....\n", dvobj->pusbintf->pm_usage_cnt);
#endif
			pwrpriv->autopm_cnt++;
		} else
			RTW_INFO("0!=pwrpriv->autopm_cnt[%d]   didn't usb_autopm_get_interface\n", pwrpriv->autopm_cnt);
#endif /* #ifndef	CONFIG_BT_COEXIST */
	}
	RTW_INFO("<==== autoresume_enter\n");
error_exit:

	return result;
}
#endif

#ifdef CONFIG_PLATFORM_RTD2880B
extern void rtd2885_wlan_netlink_sendMsg(char *action_string, char *name);
#endif

/*
 * drv_init() - a device potentially for us
 *
 * notes: drv_init() is called when the bus driver has located a card for us to support.
 *        We accept the new device by returning 0.
*/
_adapter *rtw_usb_primary_adapter_init(struct dvobj_priv *dvobj,
	struct usb_interface *pusb_intf)
{
	_adapter *padapter = NULL;
	int status = _FAIL;

	padapter = (_adapter *)rtw_zvmalloc(sizeof(*padapter));
	if (padapter == NULL)
		goto exit;

	if (loadparam(padapter) != _SUCCESS)
		goto free_adapter;

	padapter->dvobj = dvobj;


	rtw_set_drv_stopped(padapter);/*init*/

	dvobj->padapters[dvobj->iface_nums++] = padapter;
	padapter->iface_id = IFACE_ID0;

	/* set adapter_type/iface type for primary padapter */
	padapter->isprimary = _TRUE;
	padapter->adapter_type = PRIMARY_ADAPTER;
#ifdef CONFIG_MI_WITH_MBSSID_CAM/*Configure all IFACE to PORT0-MBSSID*/
	padapter->hw_port = HW_PORT0;
#else
	padapter->hw_port = HW_PORT0;
#endif

	/* step init_io_priv */
	if (rtw_init_io_priv(padapter, usb_set_intf_ops) == _FAIL)
		goto free_adapter;

	/* step 2. hook HalFunc, allocate HalData */
	if (rtw_set_hal_ops(padapter) == _FAIL)
		goto free_hal_data;


	padapter->intf_start = &usb_intf_start;
	padapter->intf_stop = &usb_intf_stop;

	/* step read_chip_version */
	rtw_hal_read_chip_version(padapter);

	/* step usb endpoint mapping */
	rtw_hal_chip_configure(padapter);

#ifdef CONFIG_BT_COEXIST
	rtw_btcoex_Initialize(padapter);
#endif
	rtw_btcoex_wifionly_initialize(padapter);

	/* step read efuse/eeprom data and get mac_addr */
	if (rtw_hal_read_chip_info(padapter) == _FAIL)
		goto free_hal_data;

	/* step 5. */
	if (rtw_init_drv_sw(padapter) == _FAIL) {
		goto free_hal_data;
	}

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18))
	if (dvobj_to_pwrctl(dvobj)->bSupportRemoteWakeup) {
		dvobj->pusbdev->do_remote_wakeup = 1;
		pusb_intf->needs_remote_wakeup = 1;
		device_init_wakeup(&pusb_intf->dev, 1);
		RTW_INFO("pwrctrlpriv.bSupportRemoteWakeup~~~~~~\n");
		RTW_INFO("pwrctrlpriv.bSupportRemoteWakeup~~~[%d]~~~\n", device_may_wakeup(&pusb_intf->dev));
	}
#endif
#endif

#ifdef CONFIG_AUTOSUSPEND
	if (padapter->registrypriv.power_mgnt != PS_MODE_ACTIVE) {
		if (padapter->registrypriv.usbss_enable) {	/* autosuspend (2s delay) */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 38))
			dvobj->pusbdev->dev.power.autosuspend_delay = 0 * HZ;/* 15 * HZ; idle-delay time */
#else
			dvobj->pusbdev->autosuspend_delay = 0 * HZ;/* 15 * HZ; idle-delay time */
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 35))
			usb_enable_autosuspend(dvobj->pusbdev);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 22) && LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 34))
			padapter->bDisableAutosuspend = dvobj->pusbdev->autosuspend_disabled ;
			dvobj->pusbdev->autosuspend_disabled = 0;/* autosuspend disabled by the user */
#endif

			/* usb_autopm_get_interface(adapter_to_dvobj(padapter)->pusbintf ); */ /* init pm_usage_cnt ,let it start from 1 */

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32))
			RTW_INFO("%s...pm_usage_cnt(%d).....\n", __FUNCTION__, atomic_read(&(dvobj->pusbintf->pm_usage_cnt)));
#else
			RTW_INFO("%s...pm_usage_cnt(%d).....\n", __FUNCTION__, dvobj->pusbintf->pm_usage_cnt);
#endif
		}
	}
#endif
	/* 2012-07-11 Move here to prevent the 8723AS-VAU BT auto suspend influence */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
	if (usb_autopm_get_interface(pusb_intf) < 0)
		RTW_INFO("can't get autopm:\n");
#endif
#ifdef CONFIG_BT_COEXIST
	dvobj_to_pwrctl(dvobj)->autopm_cnt = 1;
#endif

	/* set mac addr */
	rtw_macaddr_cfg(adapter_mac_addr(padapter), get_hal_mac_addr(padapter));
#ifdef CONFIG_MI_WITH_MBSSID_CAM
	rtw_mbid_camid_alloc(padapter, adapter_mac_addr(padapter));
#endif

#ifdef CONFIG_P2P
	rtw_init_wifidirect_addrs(padapter, adapter_mac_addr(padapter), adapter_mac_addr(padapter));
#endif /* CONFIG_P2P */
	RTW_INFO("bDriverStopped:%s, bSurpriseRemoved:%s, bup:%d, hw_init_completed:%d\n"
		 , rtw_is_drv_stopped(padapter) ? "True" : "False"
		 , rtw_is_surprise_removed(padapter) ? "True" : "False"
		 , padapter->bup
		 , rtw_get_hw_init_completed(padapter)
		);

	status = _SUCCESS;

free_hal_data:
	if (status != _SUCCESS && padapter->HalData)
		rtw_hal_free_data(padapter);
free_adapter:
	if (status != _SUCCESS && padapter) {
		#ifdef RTW_HALMAC
		rtw_halmac_deinit_adapter(dvobj);
		#endif
		rtw_vmfree((u8 *)padapter, sizeof(*padapter));
		padapter = NULL;
	}
exit:
	return padapter;
}

static void rtw_usb_primary_adapter_deinit(_adapter *padapter)
{
#if defined(CONFIG_WOWLAN) || defined(CONFIG_BT_COEXIST)
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);
#endif
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;

	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

	if (check_fwstate(pmlmepriv, _FW_LINKED))
		rtw_disassoc_cmd(padapter, 0, RTW_CMDF_DIRECTLY);

#ifdef CONFIG_AP_MODE
	if (MLME_IS_AP(padapter) || MLME_IS_MESH(padapter)) {
		free_mlme_ap_info(padapter);
		#ifdef CONFIG_HOSTAPD_MLME
		hostapd_mode_unload(padapter);
		#endif
	}
#endif

	/*rtw_cancel_all_timer(if1);*/

#ifdef CONFIG_WOWLAN
	pwrctl->wowlan_mode = _FALSE;
#endif /* CONFIG_WOWLAN */

	rtw_dev_unload(padapter);

	RTW_INFO("+r871xu_dev_remove, hw_init_completed=%d\n", rtw_get_hw_init_completed(padapter));

#ifdef CONFIG_BT_COEXIST
	if (1 == pwrctl->autopm_cnt) {
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
		usb_autopm_put_interface(adapter_to_dvobj(padapter)->pusbintf);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
		usb_autopm_enable(adapter_to_dvobj(padapter)->pusbintf);
#else
		usb_autosuspend_device(adapter_to_dvobj(padapter)->pusbdev, 1);
#endif
		pwrctl->autopm_cnt--;
	}
#endif

	rtw_free_drv_sw(padapter);

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndev_free(padapter);

#ifdef RTW_HALMAC
	rtw_halmac_deinit_adapter(adapter_to_dvobj(padapter));
#endif /* RTW_HALMAC */

	rtw_vmfree((u8 *)padapter, sizeof(_adapter));

#ifdef CONFIG_PLATFORM_RTD2880B
	RTW_INFO("wlan link down\n");
	rtd2885_wlan_netlink_sendMsg("linkdown", "8712");
#endif

}

static int rtw_drv_init(struct usb_interface *pusb_intf, const struct usb_device_id *pdid)
{
	_adapter *padapter = NULL;
	int status = _FAIL;
	struct dvobj_priv *dvobj;
#ifdef CONFIG_CONCURRENT_MODE
	int i;
#endif

	/* RTW_INFO("+rtw_drv_init\n"); */

	/* step 0. */
	process_spec_devid(pdid);

	/* Initialize dvobj_priv */
	dvobj = usb_dvobj_init(pusb_intf, pdid);
	if (dvobj == NULL) {
		goto exit;
	}

	padapter = rtw_usb_primary_adapter_init(dvobj, pusb_intf);
	if (padapter == NULL) {
		RTW_INFO("rtw_usb_primary_adapter_init Failed!\n");
		goto free_dvobj;
	}

	if (usb_reprobe_switch_usb_mode(padapter) == _TRUE)
		goto free_if_prim;

#ifdef CONFIG_CONCURRENT_MODE
	if (padapter->registrypriv.virtual_iface_num > (CONFIG_IFACE_NUMBER - 1))
		padapter->registrypriv.virtual_iface_num = (CONFIG_IFACE_NUMBER - 1);

	for (i = 0; i < padapter->registrypriv.virtual_iface_num; i++) {
		if (rtw_drv_add_vir_if(padapter, usb_set_intf_ops) == NULL) {
			RTW_INFO("rtw_drv_add_iface failed! (%d)\n", i);
			goto free_if_vir;
		}
	}
#endif

#ifdef CONFIG_GLOBAL_UI_PID
	if (ui_pid[1] != 0) {
		RTW_INFO("ui_pid[1]:%d\n", ui_pid[1]);
		rtw_signal_process(ui_pid[1], SIGUSR2);
	}
#endif

	/* dev_alloc_name && register_netdev */
	if (rtw_os_ndevs_init(dvobj) != _SUCCESS)
		goto free_if_vir;

#ifdef CONFIG_HOSTAPD_MLME
	hostapd_mode_init(padapter);
#endif

#ifdef CONFIG_PLATFORM_RTD2880B
	RTW_INFO("wlan link up\n");
	rtd2885_wlan_netlink_sendMsg("linkup", "8712");
#endif


	status = _SUCCESS;

#if 0 /* not used now */
os_ndevs_deinit:
	if (status != _SUCCESS)
		rtw_os_ndevs_deinit(dvobj);
#endif
free_if_vir:
	if (status != _SUCCESS) {
		#ifdef CONFIG_CONCURRENT_MODE
		rtw_drv_stop_vir_ifaces(dvobj);
		rtw_drv_free_vir_ifaces(dvobj);
		#endif
	}

free_if_prim:
	if (status != _SUCCESS && padapter)
		rtw_usb_primary_adapter_deinit(padapter);

free_dvobj:
	if (status != _SUCCESS)
		usb_dvobj_deinit(pusb_intf);
exit:
	return status == _SUCCESS ? 0 : -ENODEV;
}

/*
 * dev_remove() - our device is being removed
*/
/* rmmod module & unplug(SurpriseRemoved) will call r871xu_dev_remove() => how to recognize both */
static void rtw_dev_remove(struct usb_interface *pusb_intf)
{
	struct dvobj_priv *dvobj = usb_get_intfdata(pusb_intf);
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	struct pwrctrl_priv *pwrctl = dvobj_to_pwrctl(dvobj);
#endif
	_adapter *padapter = dvobj_get_primary_adapter(dvobj);

	RTW_INFO("+rtw_dev_remove\n");

	dvobj->processing_dev_remove = _TRUE;

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndevs_unregister(dvobj);

	if (usb_drv.drv_registered == _TRUE) {
		/* RTW_INFO("r871xu_dev_remove():padapter->bSurpriseRemoved == _TRUE\n"); */
		rtw_set_surprise_removed(padapter);
	}
	/*else
	{

		rtw_set_hw_init_completed(padapter, _FALSE);
	}*/


#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	rtw_unregister_early_suspend(pwrctl);
#endif

	if (GET_HAL_DATA(padapter)->bFWReady == _TRUE) {
		rtw_pm_set_ips(padapter, IPS_NONE);
		rtw_pm_set_lps(padapter, PS_MODE_ACTIVE);

		LeaveAllPowerSaveMode(padapter);
	}
	rtw_set_drv_stopped(padapter);	/*for stop thread*/
	rtw_stop_cmd_thread(padapter);
#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_stop_vir_ifaces(dvobj);
#endif /* CONFIG_CONCURRENT_MODE */

#ifdef CONFIG_BT_COEXIST
#ifdef CONFIG_BT_COEXIST_SOCKET_TRX
	if (GET_HAL_DATA(padapter)->EEPROMBluetoothCoexist)
		rtw_btcoex_close_socket(padapter);
#endif
	rtw_btcoex_HaltNotify(padapter);
#endif

	rtw_usb_primary_adapter_deinit(padapter);

#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_free_vir_ifaces(dvobj);
#endif /* CONFIG_CONCURRENT_MODE */

	usb_dvobj_deinit(pusb_intf);

	RTW_INFO("-r871xu_dev_remove, done\n");

	return;

}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
extern int console_suspend_enabled;
#endif

static int __init rtw_drv_entry(void)
{
	int ret = 0;

	RTW_PRINT("module init start\n");
	dump_drv_version(RTW_DBGDUMP);
#ifdef BTCOEXVERSION
	RTW_PRINT(DRV_NAME" BT-Coex version = %s\n", BTCOEXVERSION);
#endif /* BTCOEXVERSION */

	ret = platform_wifi_power_on();
	if (ret != 0) {
		RTW_INFO("%s: power on failed!!(%d)\n", __FUNCTION__, ret);
		ret = -1;
		goto exit;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
	/* console_suspend_enabled=0; */
#endif

	usb_drv.drv_registered = _TRUE;
	rtw_suspend_lock_init();
	rtw_drv_proc_init();
	rtw_ndev_notifier_register();
	rtw_inetaddr_notifier_register();

	ret = usb_register(&usb_drv.usbdrv);

	if (ret != 0) {
		usb_drv.drv_registered = _FALSE;
		rtw_suspend_lock_uninit();
		rtw_drv_proc_deinit();
		rtw_ndev_notifier_unregister();
		rtw_inetaddr_notifier_unregister();
		goto exit;
	}

exit:
	RTW_PRINT("module init ret=%d\n", ret);
	return ret;
}

static void __exit rtw_drv_halt(void)
{
	RTW_PRINT("module exit start\n");

	usb_drv.drv_registered = _FALSE;

	usb_deregister(&usb_drv.usbdrv);

	platform_wifi_power_off();

	rtw_suspend_lock_uninit();
	rtw_drv_proc_deinit();
	rtw_ndev_notifier_unregister();
	rtw_inetaddr_notifier_unregister();

	RTW_PRINT("module exit success\n");

	rtw_mstat_dump(RTW_DBGDUMP);
}

module_init(rtw_drv_entry);
module_exit(rtw_drv_halt);
