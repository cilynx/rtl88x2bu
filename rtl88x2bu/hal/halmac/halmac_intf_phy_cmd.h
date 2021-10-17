/******************************************************************************
 *
 * Copyright(c) 2016 - 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef HALMAC_INTF_PHY_CMD
#define HALMAC_INTF_PHY_CMD

/* Cut mask */
enum halmac_intf_phy_cut {
	HALMAC_INTF_PHY_CUT_TESTCHIP = BIT(0),
	HALMAC_INTF_PHY_CUT_A = BIT(1),
	HALMAC_INTF_PHY_CUT_B = BIT(2),
	HALMAC_INTF_PHY_CUT_C = BIT(3),
	HALMAC_INTF_PHY_CUT_D = BIT(4),
	HALMAC_INTF_PHY_CUT_E = BIT(5),
	HALMAC_INTF_PHY_CUT_F = BIT(6),
	HALMAC_INTF_PHY_CUT_G = BIT(7),
	HALMAC_INTF_PHY_CUT_ALL = 0x7FFF,
};

/* IP selection */
enum halmac_ip_sel {
	HALMAC_IP_INTF_PHY = 0,
	HALMAC_IP_SEL_MAC = 1,
	HALMAC_IP_PCIE_DBI = 2,
	HALMAC_IP_SEL_UNDEFINE = 0x7FFF,
};

/* Platform mask */
enum halmac_intf_phy_platform {
	HALMAC_INTF_PHY_PLATFORM_ALL = BIT(0),
	HALMAC_INTF_PHY_PLATFORM_ASUS = BIT(1),
	HALMAC_INTF_PHY_PLATFORM_FOR_ALL = 0x7FFF,
};

#endif
