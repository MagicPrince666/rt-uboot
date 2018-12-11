#ifndef _AR803X_PHY_H
#define _AR803X_PHY_H
/*****************/
/* PHY Registers */
/*****************/
#define ATHR_PHY_CONTROL                 0
#define ATHR_PHY_STATUS                  1
#define ATHR_PHY_ID1                     2
#define ATHR_PHY_ID2                     3
#define ATHR_AUTONEG_ADVERT              4
#define ATHR_LINK_PARTNER_ABILITY        5
#define ATHR_AUTONEG_EXPANSION           6
#define ATHR_NEXT_PAGE_TRANSMIT          7
#define ATHR_LINK_PARTNER_NEXT_PAGE      8
#define ATHR_1000BASET_CONTROL           9
#define ATHR_1000BASET_STATUS            10
#define ATHR_PHY_SPEC_CONTROL            16
#define ATHR_PHY_SPEC_STATUS             17
#define ATHR_DEBUG_PORT_ADDRESS          29
#define ATHR_DEBUG_PORT_DATA             30

#define ATHER_STATUS_LINK_MASK                0xC000
#define ATHER_STATUS_LINK_SHIFT               14
#define ATHER_STATUS_FULL_DEPLEX              0x2000
#define ATHER_STATUS_LINK_PASS                 0x0400 

/* ATHR_PHY_CONTROL fields */
#define ATHR_CTRL_SOFTWARE_RESET                    0x8000
#define ATHR_CTRL_AUTONEGOTIATION_ENABLE            0x1000
#define ATHR_CTRL_RESTART_AUTONEGOTIATION           0x0200
#define ATHR_CTRL_SPEED_FULL_DUPLEX                 0x0100
#define ATHR_CTRL_SPEED_MSB                         0x0040
#define ATHR_CTRL_RX_ENABLE                         0x0004
#define ATHR_CTRL_TX_ENABLE                         0x0001

/* Advertisement register. */
#define ATHR_ADVERTISE_NEXT_PAGE              0x8000
#define ATHR_ADVERTISE_ASYM_PAUSE             0x0800
#define ATHR_ADVERTISE_PAUSE                  0x0400
#define ATHR_ADVERTISE_100FULL                0x0100
#define ATHR_ADVERTISE_100HALF                0x0080  
#define ATHR_ADVERTISE_10FULL                 0x0040  
#define ATHR_ADVERTISE_10HALF                 0x0020  
#define ATHR_ADVERTISE_ALL (ATHR_ADVERTISE_ASYM_PAUSE | ATHR_ADVERTISE_PAUSE | \
                            ATHR_ADVERTISE_10HALF | ATHR_ADVERTISE_10FULL | \
                            ATHR_ADVERTISE_100HALF | ATHR_ADVERTISE_100FULL)
/* 1000BASET_CONTROL */
#define ATHR_ADVERTISE_1000FULL               0x0200

#define ATHR_OUI					0x4d
#define ATHR_PHY_AR8031_8033_ID 	0xd074	
#define ATHR_PHY_AR8035_ID 		    0xd072


#define ATHR_PHY0_ADDR		0x0
#define ATHR_PHY1_ADDR		0x1
#define ATHR_PHY2_ADDR		0x2
#define ATHR_PHY3_ADDR		0x3
#define ATHR_PHY4_ADDR		0x4

//#define ATHR_PHY_BASE ATHR_PHY4_ADDR

int ar803x_phy_setup(int unit);
int ar803x_phy_speed(int unit);
int ar803x_phy_is_fdx(int unit);
int ar803x_phy_is_up(int unit);
int ar803x_reg_init(void);

#endif