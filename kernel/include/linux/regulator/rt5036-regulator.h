/*
 *  include/linux/regulator/rt5036-regulator.h
 *  Include header file to Richtek RT5036 Regulator driver
 *
 *  Copyright (C) 2014 Richtek Technology Corp.
 *  cy_huang <cy_huang@richtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 */

#ifndef __LINUX_RT5036_REGULATOR_H
#define __LINUX_RT5036_REGULATOR_H

/*BUCK*/
#define RT5036_DCDCVOUT_SHIFT1	0
#define RT5036_DCDCVOUT_MASK1	0x7F
#define RT5036_DCDCVOUT_SHIFT2	0
#define RT5036_DCDCVOUT_MASK2	0x7F
#define RT5036_DCDCVOUT_SHIFT3	0
#define RT5036_DCDCVOUT_MASK3	0x7F
#define RT5036_DCDCVOUT_SHIFT4	0
#define RT5036_DCDCVOUT_MASK4	0x7F
#define RT5036_DCDCEN_MASK1	0x08
#define RT5036_DCDCEN_MASK2	0x04
#define RT5036_DCDCEN_MASK3	0x02
#define RT5036_DCDCEN_MASK4	0x01
#define RT5036_DCDCMODE_MASK1	0x80
#define RT5036_DCDCMODE_MASK2	0x40
#define RT5036_DCDCMODE_MASK3	0x20
#define RT5036_DCDCMODE_MASK4	0x10
#define RT5036_DCDCRAMP_MASK1	0xC0
#define RT5036_DCDCRAMP_SHIFT1	6
#define RT5036_DCDCRAMP_MASK2	0x30
#define RT5036_DCDCRAMP_SHIFT2	4
#define RT5036_DCDCRAMP_MASK3	0x0C
#define RT5036_DCDCRAMP_SHIFT3	2
#define RT5036_DCDCRAMP_MASK4	0x03
#define RT5036_DCDCRAMP_SHIFT4	0

/*LDO*/
#define RT5036_LDOVOUT_SHIFT1	0
#define RT5036_LDOVOUT_MASK1	0x7F
#define RT5036_LDOVOUT_SHIFT2	0
#define RT5036_LDOVOUT_MASK2	0x7F
#define RT5036_LDOVOUT_SHIFT3	0
#define RT5036_LDOVOUT_MASK3	0x7F
#define RT5036_LDOVOUT_SHIFT4	0
#define RT5036_LDOVOUT_MASK4	0x7F
#define RT5036_LDOEN_MASK1	0x80
#define RT5036_LDOEN_MASK2	0x40
#define RT5036_LDOEN_MASK3	0x20
#define RT5036_LDOEN_MASK4	0x10
#define RT5036_LDOMODE_MASK1	0x80
#define RT5036_LDOMODE_MASK2	0x40
#define RT5036_LDOMODE_MASK3	0x20
#define RT5036_LDOMODE_MASK4	0x10
#define RT5036_LDORAMP_MASK1	0xC0
#define RT5036_LDORAMP_SHIFT1	6
#define RT5036_LDORAMP_MASK2	0x30
#define RT5036_LDORAMP_SHIFT2	4
#define RT5036_LDORAMP_MASK3	0x0C
#define RT5036_LDORAMP_SHIFT3	2
#define RT5036_LDORAMP_MASK4	0x03
#define RT5036_LDORAMP_SHIFT4	0

/*LSW*/
#define RT5036_LSWVOUT_SHIFT1	0
#define RT5036_LSWVOUT_MASK1	0x7F
#define RT5036_LSWVOUT_SHIFT2	0
#define RT5036_LSWVOUT_MASK2	0x7F
#define RT5036_LSWNEN_MASK2	0x02
#define RT5036_LSWNEN_MASK1	0x01
#define RT5036_LSWSEN_MASK2	0x08
#define RT5036_LSWSEN_MASK1	0x04
#define RT5036_LSWNMODE_MASK2	0x80
#define RT5036_LSWNMODE_MASK1	0x40
#define RT5036_LSWSMODE_MASK2	0x08
#define RT5036_LSWSMODE_MASK1	0x04
#define RT5036_LSWNRAMP_MASK2	0xC0
#define RT5036_LSWNRAMP_SHIFT2	6
#define RT5036_LSWNRAMP_MASK1	0x30
#define RT5036_LSWNRAMP_SHIFT1	4
#define RT5036_LSWSRAMP_MASK2	0x0C
#define RT5036_LSWSRAMP_SHIFT2	2
#define RT5036_LSWSRAMP_MASK1	0x03
#define RT5036_LSWSRAMP_SHIFT1	0

#endif /* __LINUX_RT5036_REGULATOR_H */