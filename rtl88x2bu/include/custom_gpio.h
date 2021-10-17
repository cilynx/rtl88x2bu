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
#ifndef __CUSTOM_GPIO_H__
#define __CUSTOM_GPIO_H___

#include <drv_conf.h>
#include <osdep_service.h>

typedef enum cust_gpio_modes {
	WLAN_PWDN_ON,
	WLAN_PWDN_OFF,
	WLAN_POWER_ON,
	WLAN_POWER_OFF,
	WLAN_BT_PWDN_ON,
	WLAN_BT_PWDN_OFF
} cust_gpio_modes_t;

extern int rtw_wifi_gpio_init(void);
extern int rtw_wifi_gpio_deinit(void);
extern void rtw_wifi_gpio_wlan_ctrl(int onoff);

#endif
