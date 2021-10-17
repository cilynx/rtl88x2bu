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
#ifndef __PCI_HAL_H__
#define __PCI_HAL_H__

#ifdef CONFIG_RTL8188E
	void rtl8188ee_set_hal_ops(_adapter *padapter);
#endif

#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
	void rtl8812ae_set_hal_ops(_adapter *padapter);
#endif

#if defined(CONFIG_RTL8192E)
	void rtl8192ee_set_hal_ops(_adapter *padapter);
#endif

#if defined(CONFIG_RTL8192F)
	void rtl8192fe_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8723B
	void rtl8723be_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8723D
	void rtl8723de_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8814A
	void rtl8814ae_set_hal_ops(_adapter *padapter);
#endif

#ifdef CONFIG_RTL8822B
	void rtl8822be_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8822C
	void rtl8822ce_set_hal_ops(PADAPTER padapter);
#endif

#ifdef CONFIG_RTL8814B
	void rtl8814be_set_hal_ops(PADAPTER padapter);
#endif

u8 rtw_set_hal_ops(_adapter *padapter);

#endif /* __PCIE_HAL_H__ */
