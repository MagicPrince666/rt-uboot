/*
 * Copyright (c) 2010, Atheros Communications Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
 /*
 * Fortune Zhiyong Liu porting for customers. 11/24/2011. 
 * willyliu@fortune-co.com
 */

#include <config.h>
#include <linux/types.h>
#include <common.h>
#include <miiphy.h>
#include "phy.h"
#include <asm/addrspace.h>
#include "ar934x_soc.h"
#include "ar803x_phy.h"

int ATHR_PHY_BASE = 0;

int ar803x_phy_setup(int unit)
{
    char *pszAr8031_8033="AR8031/AR8033";
    char *pszAr8035="AR8035";
    char *pszName="Unknown";
    int phyid1, phyid2;
    //int val;
    //uint16_t  phyHwStatus;
//    uint16_t  timeout;
//    char cget;
    int i;

    for( i = 0; i <= ATHR_PHY4_ADDR; i++ )
    {
        phyid1 = phy_reg_read(unit, i, ATHR_PHY_ID1);
        phyid2 = phy_reg_read(unit, i, ATHR_PHY_ID2);

        if (phyid1 == ATHR_OUI)
        {
            if (phyid2 == ATHR_PHY_AR8031_8033_ID)
            {
                pszName = pszAr8031_8033;
            }
            else if (phyid2 == ATHR_PHY_AR8035_ID)
            {
                pszName = pszAr8035;
            }
            ATHR_PHY_BASE = i;
            printf("Phy %d: atheros %s\n", unit, pszName);
            printf("base address %d\n",ATHR_PHY_BASE);      
            break;
        }

    }

    if(phyid1 != ATHR_OUI)
    {
        printf("Phy %d: Unknown!\n", unit);
        printf("phyid1 :0x%x phyid2 :0x%x \n",phyid1,phyid2);
        pszName = pszAr8035;
    }

/*
    printf("Phy address %d\n", ATHR_PHY_BASE);

    phyid1 = phy_reg_read(unit, ATHR_PHY_BASE, ATHR_PHY_ID1);
    phyid2 = phy_reg_read(unit, ATHR_PHY_BASE, ATHR_PHY_ID2);
    if (phyid1 == ATHR_OUI)
    {
    	if (phyid2 == ATHR_PHY_AR8031_8033_ID)
	    {
	    	pszName = pszAr8031_8033;
	    }
	    else if (phyid2 == ATHR_PHY_AR8035_ID)
	    {
	    	pszName = pszAr8035;
	    }
	    printf("Phy %d: atheros %s\n", unit, pszName);
    }
    else
    {
	    printf("Phy %d: Unknown!\n", unit);
        printf("phyid1 :0x%x phyid2 :0x%x \n",phyid1,phyid2);
        pszName = pszAr8035;
    }
*/
    //udelay(1000*1000);
    /* delay rx_clk */
    phy_reg_write(unit, ATHR_PHY_BASE, 0x1D, 0x0);
    phy_reg_write(unit, ATHR_PHY_BASE, 0x1E, 0x82EE); /* 0x02EE - no delay, 0x82EE - rx delay*/
    //val = phy_reg_read(unit, ATHR_PHY_BASE, 0x1E);
    //printf("Debug reg 0x0 = 0x%x\n", val);

    /* delay tx_clk */
    phy_reg_write(unit, ATHR_PHY_BASE, 0x1D, 0x5);
    phy_reg_write(unit, ATHR_PHY_BASE, 0x1E, 0x2D47); /* 0x2C47 - no delay, 0x2D47 - tx delay */
    //val = phy_reg_read(unit, ATHR_PHY_BASE, 0x1E);
    //printf("Debug reg 0x5 = 0x%x\n", val);

    /* Reset PHYs*/
    phy_reg_write(unit, ATHR_PHY_BASE, ATHR_PHY_CONTROL,
                  ATHR_CTRL_AUTONEGOTIATION_ENABLE
                  | ATHR_CTRL_SOFTWARE_RESET);
    return 0;
}

int ar803x_phy_speed(int unit)
{
    int phyHwStatus;

    phyHwStatus = phy_reg_read(unit, ATHR_PHY_BASE, 0x11);

    {    
        phyHwStatus = ((phyHwStatus & ATHER_STATUS_LINK_MASK) >>
                       ATHER_STATUS_LINK_SHIFT);

        switch(phyHwStatus) {
        case 0:
            return _10BASET;
        case 1:
            return _100BASET;
        case 2:
            return _1000BASET;
        default:
            printf("Unkown speed read!\n");
        }
    }

    //printf("athr_phy_speed: link down, returning 10t\n");
    return _10BASET;
}

/******************************************************************************
*
* athrs26_phy_is_fdx - Determines whether the phy ports associated with the
* specified device are FULL or HALF duplex.
*
* RETURNS:
*    1 --> FULL
*    0 --> HALF
*/
int ar803x_phy_is_fdx(int unit)
{
    int phyHwStatus;

    phyHwStatus = phy_reg_read(unit, ATHR_PHY_BASE, 0x11);

    if (phyHwStatus & ATHER_STATUS_FULL_DEPLEX)
       return 1;

    return 0;
}

/*****************************************************************************
*
* athr_phy_is_up -- checks for significant changes in PHY state.
*
* A "significant change" is:
*     dropped link (e.g. ethernet cable unplugged) OR
*     autonegotiation completed + link (e.g. ethernet cable plugged in)
*
* When a PHY is plugged in, phyLinkGained is called.
* When a PHY is unplugged, phyLinkLost is called.
*/
int ar803x_phy_is_up(int unit)
{
    uint32_t phyHwStatus;

    phyHwStatus = phy_reg_read(unit, ATHR_PHY_BASE, 0x11);

    if (phyHwStatus & ATHER_STATUS_LINK_PASS)
       return 1;

    return 0;
}

int ar803x_reg_init(void)
{
	printf("xag Init atheros ar803x phy...\n");
	return 0;
}

