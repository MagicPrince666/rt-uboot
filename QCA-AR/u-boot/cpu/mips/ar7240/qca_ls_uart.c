/*
 * Qualcomm/Atheros Low-Speed UART driver
 *
 * Copyright (C) 2016 Piotr Dymacz <piotr@dymacz.pl>
 * Copyright (C) 2008-2010 Atheros Communications Inc.
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include <config.h>
#include <common.h>
#include <asm/addrspace.h>
#include <soc/qca_soc_common.h>

DECLARE_GLOBAL_DATA_PTR;

#define QCA_GPIO_OUT_FUNCTION3_ENABLE_GPIO_13(x)	((0xff&x)<< 8)

#define AR934X_LS_UART0_TX				13
#define AR934X_LS_UART0_RX				15

#define DEFAULT_LS_UART0_TX				10
#define DEFAULT_LS_UART0_RX				9

#define ATH_GPIO_OUT_FUNCTION2_ENABLE_GPIO_10(x)	((0xff&x)<<16)
#define ATH_GPIO_OUT_FUNCTION2_ENABLE_GPIO_9(x)		((0xff&x)<< 8)

void serial_setbrg(void)
{
	u32 div;

	/*
	 * TODO: prepare list of supported range of baudrate values
	 * For 40 MHz ref_clk, successfully tested up to 1152000 on AR9344
	 */

	/* Round to closest, final baudrate = ref_clk / (16 * div) */
	if (qca_xtal_is_40mhz() == 1) {
		div = (VAL_40MHz + (8 * gd->baudrate)) / (16 * gd->baudrate);
	} else {
		div = (VAL_25MHz + (8 * gd->baudrate)) / (16 * gd->baudrate);
	}

#if 0
	u32 data;

	//set AR934X_LS_UART0_TX as output
	//set AR934X_LS_UART0_RX as input
	data = qca_soc_reg_read(QCA_GPIO_OE_REG);
	data = (data & (~BIT(AR934X_LS_UART0_TX))) | BIT(AR934X_LS_UART0_RX);
	qca_soc_reg_write(QCA_GPIO_OE_REG,data);

	//set AR934X_LS_UART0_TX to TX
	data = qca_soc_reg_read(QCA_GPIO_OUT_FUNC3_REG);
	data = (data & ~QCA_GPIO_OUT_FUNCX_GPIO13_EN_MASK) |
		QCA_GPIO_OUT_FUNCTION3_ENABLE_GPIO_13(QCA_GPIO_OUT_MUX_LSUART_TXD_VAL);
	qca_soc_reg_write(QCA_GPIO_OUT_FUNC3_REG, data);

	data = qca_soc_reg_read(QCA_GPIO_IN_EN0_REG);
	data = (data&0x00FF) | (AR934X_LS_UART0_RX<<8);
	qca_soc_reg_write(QCA_GPIO_IN_EN0_REG, data);

#else

	u32 data;

	//set DEFAULT_LS_UART0_TX as output
	//set DEFAULT_LS_UART0_RX as input
	data = qca_soc_reg_read(QCA_GPIO_OE_REG);
	data = (data & (~BIT(DEFAULT_LS_UART0_TX))) | BIT(DEFAULT_LS_UART0_RX);
	qca_soc_reg_write(QCA_GPIO_OE_REG,data);

	//set AR934X_LS_UART0_TX to TX
	data = qca_soc_reg_read(QCA_GPIO_OUT_FUNC2_REG);
	data = (data & ~QCA_GPIO_OUT_FUNCX_GPIO10_EN_MASK) |
		ATH_GPIO_OUT_FUNCTION2_ENABLE_GPIO_10(QCA_GPIO_OUT_MUX_LSUART_TXD_VAL);
	qca_soc_reg_write(QCA_GPIO_OUT_FUNC2_REG, data);


	data = qca_soc_reg_read(QCA_GPIO_IN_EN0_REG);
	data = (data&0x00FF) | DEFAULT_LS_UART0_RX << 8;
	qca_soc_reg_write(QCA_GPIO_IN_EN0_REG, data);

#endif

	/* Set DLAB bit in LCR register unlocks DLL/DLH registers */
	qca_soc_reg_read_set(QCA_LSUART_LCR_REG, QCA_LSUART_LCR_DLAB_MASK);

	/* Write div into DLL and DLH registers */
	qca_soc_reg_write(QCA_LSUART_DLL_REG, (div & 0xFF));
	qca_soc_reg_write(QCA_LSUART_DLH_REG, ((div >> 8) & 0xFF));

	/* Clear DLAB bit in LCR register */
	qca_soc_reg_read_clear(QCA_LSUART_LCR_REG, QCA_LSUART_LCR_DLAB_MASK);
}

int serial_init(void)
{
	serial_setbrg();

	/* No interrupt */
	qca_soc_reg_write(QCA_LSUART_IER_REG, 0x0);

	/* No FIFO/DMA */
	qca_soc_reg_write(QCA_LSUART_FCR_REG, 0x0);

	/*
	 * Low-Speed UART controller configuration:
	 * - data: 8bits
	 * - stop: 1bit
	 * - parity: no
	 */
	qca_soc_reg_write(QCA_LSUART_LCR_REG,
					  QCA_LSUART_LCR_CLS_8BIT_VAL << QCA_LSUART_LCR_CLS_SHIFT);

	return 0;
}

void serial_putc(const char c)
{
	u32 line_status;

	if (c == '\n')
		serial_putc('\r');

	/* Wait for empty THR */
	do {
		line_status = qca_soc_reg_read(QCA_LSUART_LSR_REG)
					  & QCA_LSUART_LSR_THRE_MASK;
	} while (line_status == 0);

	/* Put data in THR */
	qca_soc_reg_write(QCA_LSUART_THR_REG, (u32)c);
}

int serial_getc(void)
{
	while (!serial_tstc())
		;

	/* Get data from RBR */
	return qca_soc_reg_read(QCA_LSUART_RBR_REG)
		   & QCA_LSUART_RBR_RBR_MASK;
}

int serial_tstc(void)
{
	/* Check data ready bit */
	return qca_soc_reg_read(QCA_LSUART_LSR_REG)
		   & QCA_LSUART_LSR_DR_MASK;
}

void serial_puts(const char *s)
{
	while (*s)
		serial_putc(*s++);
}
