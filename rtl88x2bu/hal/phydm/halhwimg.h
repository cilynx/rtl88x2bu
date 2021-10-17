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
#pragma once
#ifndef __INC_HW_IMG_H
#define __INC_HW_IMG_H

/*@
 * 2011/03/15 MH Add for different IC HW image file selection. code size consideration.
 *   */
#if RT_PLATFORM == PLATFORM_LINUX

	#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
		/* @For 92C */
		#define		RTL8192CE_HWIMG_SUPPORT					1
		#define		RTL8192CE_TEST_HWIMG_SUPPORT			0
		#define		RTL8192CU_HWIMG_SUPPORT					0
		#define		RTL8192CU_TEST_HWIMG_SUPPORT			0

		/* @For 92D */
		#define		RTL8192DE_HWIMG_SUPPORT					1
		#define		RTL8192DE_TEST_HWIMG_SUPPORT			0
		#define		RTL8192DU_HWIMG_SUPPORT					0
		#define		RTL8192DU_TEST_HWIMG_SUPPORT			0

		/* @For 8723 */
		#define		RTL8723E_HWIMG_SUPPORT					1
		#define		RTL8723U_HWIMG_SUPPORT					0
		#define		RTL8723S_HWIMG_SUPPORT					0

		/* @For 88E */
		#define		RTL8188EE_HWIMG_SUPPORT					0
		#define		RTL8188EU_HWIMG_SUPPORT					0
		#define		RTL8188ES_HWIMG_SUPPORT					0

	#elif (DEV_BUS_TYPE == RT_USB_INTERFACE)
		/* @For 92C */
		#define	RTL8192CE_HWIMG_SUPPORT				0
		#define	RTL8192CE_TEST_HWIMG_SUPPORT			0
		#define	RTL8192CU_HWIMG_SUPPORT				1
		#define	RTL8192CU_TEST_HWIMG_SUPPORT			0

		/* @For 92D */
		#define	RTL8192DE_HWIMG_SUPPORT				0
		#define	RTL8192DE_TEST_HWIMG_SUPPORT			0
		#define	RTL8192DU_HWIMG_SUPPORT				1
		#define	RTL8192DU_TEST_HWIMG_SUPPORT			0

		/* @For 8723 */
		#define	RTL8723E_HWIMG_SUPPORT					0
		#define	RTL8723U_HWIMG_SUPPORT					1
		#define	RTL8723S_HWIMG_SUPPORT					0

		/* @For 88E */
		#define		RTL8188EE_HWIMG_SUPPORT					0
		#define		RTL8188EU_HWIMG_SUPPORT					0
		#define		RTL8188ES_HWIMG_SUPPORT					0

	#elif (DEV_BUS_TYPE == RT_SDIO_INTERFACE)
		/* @For 92C */
		#define	RTL8192CE_HWIMG_SUPPORT				0
		#define	RTL8192CE_TEST_HWIMG_SUPPORT			0
		#define	RTL8192CU_HWIMG_SUPPORT				1
		#define	RTL8192CU_TEST_HWIMG_SUPPORT			0

		/* @For 92D */
		#define	RTL8192DE_HWIMG_SUPPORT				0
		#define	RTL8192DE_TEST_HWIMG_SUPPORT			0
		#define	RTL8192DU_HWIMG_SUPPORT				1
		#define	RTL8192DU_TEST_HWIMG_SUPPORT			0

		/* @For 8723 */
		#define	RTL8723E_HWIMG_SUPPORT					0
		#define	RTL8723U_HWIMG_SUPPORT					0
		#define	RTL8723S_HWIMG_SUPPORT					1

		/* @For 88E */
		#define		RTL8188EE_HWIMG_SUPPORT					0
		#define		RTL8188EU_HWIMG_SUPPORT					0
		#define		RTL8188ES_HWIMG_SUPPORT					0
	#endif

#else	/* PLATFORM_WINDOWS & MacOSX */

	/* @For 92C */
	#define		RTL8192CE_HWIMG_SUPPORT						1
	#define		RTL8192CE_TEST_HWIMG_SUPPORT				1
	#define		RTL8192CU_HWIMG_SUPPORT						1
	#define		RTL8192CU_TEST_HWIMG_SUPPORT				1

	/* @For 92D */
	#define		RTL8192DE_HWIMG_SUPPORT					1
	#define		RTL8192DE_TEST_HWIMG_SUPPORT				1
	#define		RTL8192DU_HWIMG_SUPPORT					1
	#define		RTL8192DU_TEST_HWIMG_SUPPORT				1

	#if defined(UNDER_CE)
		/* @For 8723 */
		#define		RTL8723E_HWIMG_SUPPORT					0
		#define		RTL8723U_HWIMG_SUPPORT					0
		#define		RTL8723S_HWIMG_SUPPORT					1

		/* @For 88E */
		#define		RTL8188EE_HWIMG_SUPPORT					0
		#define		RTL8188EU_HWIMG_SUPPORT					0
		#define		RTL8188ES_HWIMG_SUPPORT					0

	#else

		/* @For 8723 */
		#define		RTL8723E_HWIMG_SUPPORT					1
		/* @#define		RTL_8723E_TEST_HWIMG_SUPPORT			1 */
		#define		RTL8723U_HWIMG_SUPPORT					1
		/* @#define		RTL_8723U_TEST_HWIMG_SUPPORT			1 */
		#define		RTL8723S_HWIMG_SUPPORT					1
		/* @#define		RTL_8723S_TEST_HWIMG_SUPPORT			1 */

		/* @For 88E */
		#define		RTL8188EE_HWIMG_SUPPORT					1
		#define		RTL8188EU_HWIMG_SUPPORT					1
		#define		RTL8188ES_HWIMG_SUPPORT					1
	#endif

#endif

#endif /* @__INC_HW_IMG_H */
