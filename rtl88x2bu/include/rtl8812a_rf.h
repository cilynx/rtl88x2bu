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
#ifndef __RTL8812A_RF_H__
#define __RTL8812A_RF_H__

void
PHY_RF6052SetBandwidth8812(
		PADAPTER				Adapter,
		enum channel_width		Bandwidth);


int
PHY_RF6052_Config_8812(
		PADAPTER	Adapter);

#endif/* __RTL8188E_RF_H__ */
