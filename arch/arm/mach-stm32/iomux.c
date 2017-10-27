/*
 * (C) Copyright 2011-2015
 * Emcraft Systems, <www.emcraft.com>
 * Yuri Tikhonov <yur@emcraft.com>
 * Alexander Potashev <aspotashev@emcraft.com>
 * Vladimir Khusainov <vlad@emcraft.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

#include <mach/iomux.h>
#include <mach/platform.h>
#include <mach/stm32.h>
#include <mach/fb.h>

/*
 * GPIO configuration mode
 */
#define STM32F2_GPIO_MODE_IN	0x00
#define STM32F2_GPIO_MODE_OUT	0x01
#define STM32F2_GPIO_MODE_AF	0x02
#define STM32F2_GPIO_MODE_AN	0x03

/*
 * GPIO output type
 */
#define STM32F2_GPIO_OTYPE_PP	0x00
#define STM32F2_GPIO_OTYPE_OD	0x01

/*
 * GPIO output maximum frequency
 */
#define STM32F2_GPIO_SPEED_2M	0x00
#define STM32F2_GPIO_SPEED_25M	0x01
#define STM32F2_GPIO_SPEED_50M	0x02
#define STM32F2_GPIO_SPEED_100M	0x03

/*
 * GPIO pullup, pulldown configuration
 */
#define STM32F2_GPIO_PUPD_NO	0x00
#define STM32F2_GPIO_PUPD_UP	0x01
#define STM32F2_GPIO_PUPD_DOWN	0x02

/*
 * I2C AF
 */
#define STM32F2_GPIO_AF_I2C1	0x04
#define STM32F2_GPIO_AF_I2C2	0x04
#define STM32F2_GPIO_AF_I2C3	0x04
#define STM32F2_GPIO_AF_I2C4	0x04

/*
 * SPI AF
 */
#define STM32F2_GPIO_AF_SPI1	0x05
#define STM32F2_GPIO_AF_SPI2	0x05
#define STM32F2_GPIO_AF_SPI3	0x06
#define STM32F2_GPIO_AF_SPI4	0x05
#define STM32F2_GPIO_AF_SPI5	0x05
#define STM32F2_GPIO_AF_SPI6	0x05

/*
 * USART AF
 */
#define STM32F2_GPIO_AF_USART1	0x07
#define STM32F2_GPIO_AF_USART2	0x07
#define STM32F2_GPIO_AF_USART3	0x07
#define STM32F2_GPIO_AF_USART4	0x08
#define STM32F2_GPIO_AF_USART5	0x08
#define STM32F2_GPIO_AF_USART6	0x08
#define STM32F2_GPIO_AF_USART7	0x08
#define STM32F2_GPIO_AF_USART8	0x08

/*
 * CAN AF
 */
#define STM32F2_GPIO_AF_CAN1	0x09

/*
 * AF10 selection
 */
#define STM32F2_GPIO_AF_USB_OTG	0x0A

/*
 * MAC AF
 */
#define STM32F2_GPIO_AF_MAC	0x0B

/*
 * FSMC AF
 */
#define STM32F2_GPIO_AF_FSMC	0x0C

/*
 * SDIO AF
 */
#define STM32F2_GPIO_AF_SDIO	0x0C

/*
 * LTDC AF
 */
#define STM32F2_GPIO_AF_LTDC	0x0E

/*
 * GPIO roles (alternative functions); role determines by whom GPIO is used
 * WARNING: enums must be in sync with af_val[]!
 */
enum stm32f2_gpio_role {
	STM32F2_GPIO_ROLE_USART1,	/* USART1			      */
	STM32F2_GPIO_ROLE_USART2,	/* USART2			      */
	STM32F2_GPIO_ROLE_USART3,	/* USART3			      */
	STM32F2_GPIO_ROLE_USART4,	/* USART4			      */
	STM32F2_GPIO_ROLE_USART5,	/* USART5			      */
	STM32F2_GPIO_ROLE_USART6,	/* USART6			      */
	STM32F2_GPIO_ROLE_USART7,	/* USART7			      */
	STM32F2_GPIO_ROLE_USART8,	/* USART8			      */
	STM32F2_GPIO_ROLE_ETHERNET,	/* MAC				      */
	STM32F2_GPIO_ROLE_SPI1,		/* SPI1				      */
	STM32F2_GPIO_ROLE_SPI2,		/* SPI2				      */
	STM32F2_GPIO_ROLE_SPI3,		/* SPI3				      */
	STM32F2_GPIO_ROLE_SPI4,		/* SPI4				      */
	STM32F2_GPIO_ROLE_SPI5,		/* SPI5				      */
	STM32F2_GPIO_ROLE_SPI6,		/* SPI6				      */
	STM32F2_GPIO_ROLE_I2C1,		/* I2C1				      */
	STM32F2_GPIO_ROLE_I2C2,		/* I2C2				      */
	STM32F2_GPIO_ROLE_I2C3,		/* I2C3				      */
	STM32F2_GPIO_ROLE_I2C4,		/* I2C4				      */
	STM32F2_GPIO_ROLE_SDIO,		/* SDIO				      */
	STM32F2_GPIO_ROLE_USB_OTG,	/* USB OTG			      */
	STM32F2_GPIO_ROLE_LTDC,		/* LCD controller		      */
	STM32F2_GPIO_ROLE_CAN1,		/* CAN				      */
	STM32F2_GPIO_ROLE_MCO,		/* MC external output clock	      */
	STM32F2_GPIO_ROLE_OUT,		/* General purpose output	      */
	STM32F2_GPIO_ROLE_IN,		/* General purpose input no pull      */
	STM32F2_GPIO_ROLE_IN_PUP	/* General purpose input w/pullup     */
};

/*
 * GPIO descriptor
 */
struct stm32f2_gpio_dsc {
	u32		port;		/* GPIO port			      */
	u32		pin;		/* GPIO pin			      */
};

/*
 * Register map bases
 */
static const unsigned long stm32_gpio_base[] = {
	STM32F2_GPIOA_BASE, STM32F2_GPIOB_BASE, STM32F2_GPIOC_BASE,
	STM32F2_GPIOD_BASE, STM32F2_GPIOE_BASE, STM32F2_GPIOF_BASE,
	STM32F2_GPIOG_BASE, STM32F2_GPIOH_BASE, STM32F2_GPIOI_BASE,
	STM32F4_GPIOJ_BASE, STM32F4_GPIOK_BASE,
};

/*
 * AF values (note, indexed by enum stm32f2_gpio_role)
 */
static const u32 af_val[] = {
	STM32F2_GPIO_AF_USART1, STM32F2_GPIO_AF_USART2, STM32F2_GPIO_AF_USART3,
	STM32F2_GPIO_AF_USART4, STM32F2_GPIO_AF_USART5, STM32F2_GPIO_AF_USART6,
	STM32F2_GPIO_AF_USART7, STM32F2_GPIO_AF_USART8, STM32F2_GPIO_AF_MAC,
	STM32F2_GPIO_AF_SPI1, STM32F2_GPIO_AF_SPI2, STM32F2_GPIO_AF_SPI3,
	STM32F2_GPIO_AF_SPI4, STM32F2_GPIO_AF_SPI5, STM32F2_GPIO_AF_SPI6,
	STM32F2_GPIO_AF_I2C1, STM32F2_GPIO_AF_I2C2, STM32F2_GPIO_AF_I2C3,
	STM32F2_GPIO_AF_I2C4,
	STM32F2_GPIO_AF_SDIO, STM32F2_GPIO_AF_USB_OTG,
	STM32F2_GPIO_AF_LTDC, STM32F2_GPIO_AF_CAN1,
	0
};

/*
 * Configure the specified GPIO for the specified role
 */
#ifndef CONFIG_ARCH_STM32F1
static int stm32f2_gpio_config(
	struct stm32f2_gpio_dsc *dsc, enum stm32f2_gpio_role role)
{
	volatile struct stm32f2_gpio_regs	*gpio_regs;

	u32	otype, ospeed, pupd, mode, i;
	int	rv;

	/*
	 * Check params
	 */
	if (!dsc || dsc->port >= ARRAY_SIZE(stm32_gpio_base) || dsc->pin > 15) {
		printk(KERN_WARNING "%s: bad I/O {%d,%d}\n", __func__,
			dsc ? dsc->port : -1, dsc ? dsc->pin : -1);
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Depending on the role, select the appropriate io params
	 */
	switch (role) {

	case STM32F2_GPIO_ROLE_USART1:
	case STM32F2_GPIO_ROLE_USART2:
	case STM32F2_GPIO_ROLE_USART3:
	case STM32F2_GPIO_ROLE_USART4:
	case STM32F2_GPIO_ROLE_USART5:
	case STM32F2_GPIO_ROLE_USART6:
	case STM32F2_GPIO_ROLE_USART7:
	case STM32F2_GPIO_ROLE_USART8:
	case STM32F2_GPIO_ROLE_SPI1:
	case STM32F2_GPIO_ROLE_SPI2:
	case STM32F2_GPIO_ROLE_SPI3:
	case STM32F2_GPIO_ROLE_SPI4:
	case STM32F2_GPIO_ROLE_SPI5:
	case STM32F2_GPIO_ROLE_SPI6:
	case STM32F2_GPIO_ROLE_CAN1:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_UP;
		break;
	case STM32F2_GPIO_ROLE_I2C1:
	case STM32F2_GPIO_ROLE_I2C2:
	case STM32F2_GPIO_ROLE_I2C3:
	case STM32F2_GPIO_ROLE_I2C4:
		otype  = STM32F2_GPIO_OTYPE_OD;
		ospeed = STM32F2_GPIO_SPEED_2M;
		pupd   = STM32F2_GPIO_PUPD_UP;
		break;
	case STM32F2_GPIO_ROLE_USB_OTG:
	case STM32F2_GPIO_ROLE_ETHERNET:
	case STM32F2_GPIO_ROLE_MCO:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_100M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		break;
	case STM32F2_GPIO_ROLE_SDIO:
	case STM32F2_GPIO_ROLE_LTDC:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		break;
	case STM32F2_GPIO_ROLE_OUT:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		break;
	case STM32F2_GPIO_ROLE_IN:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_NO;
		break;
	case STM32F2_GPIO_ROLE_IN_PUP:
		otype  = STM32F2_GPIO_OTYPE_PP;
		ospeed = STM32F2_GPIO_SPEED_50M;
		pupd   = STM32F2_GPIO_PUPD_UP;
		break;
	default:
		rv = -EINVAL;
		goto out;
	}

	/*
	 * Get reg base
	 */
	gpio_regs = (struct stm32f2_gpio_regs *)stm32_gpio_base[dsc->port];

	/*
	 * Enable GPIO clocks
	 */
	STM32_RCC->ahb1enr |= 1 << dsc->port;

	if (role != STM32F2_GPIO_ROLE_MCO &&
	    role != STM32F2_GPIO_ROLE_OUT &&
	    role != STM32F2_GPIO_ROLE_IN &&
	    role != STM32F2_GPIO_ROLE_IN_PUP) {

		/*
		 * Connect PXy to the specified controller (role)
		 */
		i = (dsc->pin & 0x07) * 4;
		gpio_regs->afr[dsc->pin >> 3] &= ~(0xF << i);
		gpio_regs->afr[dsc->pin >> 3] |= af_val[role] << i;
	}

	i = dsc->pin;

	/*
	 * Output mode configuration
	 */
	gpio_regs->otyper &= ~(0x1 << i);
	gpio_regs->otyper |= otype << i;

	i = dsc->pin * 2;

	/*
	 * Set mode
	 */
	if (role == STM32F2_GPIO_ROLE_OUT) {
		mode = STM32F2_GPIO_MODE_OUT;
	}
	else if (role == STM32F2_GPIO_ROLE_IN) {
		mode = STM32F2_GPIO_MODE_IN;
	}
	else if (role == STM32F2_GPIO_ROLE_IN_PUP) {
		mode = STM32F2_GPIO_MODE_IN;
	}
	else {
		mode = STM32F2_GPIO_MODE_AF;
	}
	gpio_regs->moder &= ~(0x3 << i);
	gpio_regs->moder |= mode << i;

	/*
	 * Speed mode configuration
	 */
	gpio_regs->ospeedr &= ~(0x3 << i);
	gpio_regs->ospeedr |= ospeed << i;

	/*
	 * Pull-up, pull-down resistor configuration
	 */
	gpio_regs->pupdr &= ~(0x3 << i);
	gpio_regs->pupdr |= pupd << i;

	rv = 0;
out:
	return rv;
}
#endif /* !CONFIG_ARCH_STM32F1 */

/*
 * USB is a special case since USB may be compiled as modules, so
 * the appropriate CONFIG_STM_USB_OTG_xS will be undefined
 */
void stm32_iomux_usb_fs_init(void)
{
	static struct stm32f2_gpio_dsc otg_gpio[] = {
		{0, 11}, {0, 12}, {0, 9}
	};
	int	i;

	for (i = 0; i < ARRAY_SIZE(otg_gpio); i++) {
		stm32f2_gpio_config(&otg_gpio[i],
				    STM32F2_GPIO_ROLE_USB_OTG);
	}
}
EXPORT_SYMBOL(stm32_iomux_usb_fs_init);

void stm32_iomux_usb_hs_init(void)
{
	static struct stm32f2_gpio_dsc otg_gpio[] = {
		{0, 0 }, /* DIR <?> */
		{2, 0 }, /* STP PC0 */
		{0, 5 }, /* CLK PA5 */
		{7, 4 }, /* NXT PH4 */

		{0, 3 }, /* DATA0 PA3  */
		{1, 0 }, /* DATA1 PB0  */
		{1, 1 }, /* DATA2 PB1  */
		{1, 10}, /* DATA3 PB10 */
		{1, 11}, /* DATA4 PB11 */
		{1, 12}, /* DATA5 PB12 */
		{1, 13}, /* DATA6 PB13 */
		{1, 5 }, /* DATA7 PB5  */
	};
	volatile struct stm32f2_gpio_regs *gpio_reg;
	int	i;

	/* select the appropriate ULPI_DIR */
	if (stm32_platform_get() == PLATFORM_STM32_STM32F7_DISCO) {
		otg_gpio[0].port = 2;
		otg_gpio[0].pin  = 2;
	} else {
		otg_gpio[0].port = 8;
		otg_gpio[0].pin  = 11;
	}

	/*
	 * if we are reset from low power mode, then ULPI_STP may be
	 * still driving high, thus not allow USB PHY to fully wake-up
	 */
	gpio_reg = (void *)stm32_gpio_base[otg_gpio[1].port];
	if (gpio_reg->idr & (1 << otg_gpio[1].pin)) {
		gpio_reg->moder &= ~(3 << (otg_gpio[1].pin * 2));
		gpio_reg->moder |=  (1 << (otg_gpio[1].pin * 2));
		gpio_reg->odr &= ~(1 << otg_gpio[1].pin);
	}

	for (i = 0; i < ARRAY_SIZE(otg_gpio); i++) {
		stm32f2_gpio_config(&otg_gpio[i],
				    STM32F2_GPIO_ROLE_USB_OTG);
	}
}
EXPORT_SYMBOL(stm32_iomux_usb_hs_init);

/*
 * Initialize the GPIO Alternative Functions of the STM32.
 */
void __init stm32_iomux_init(void)
{
	struct stm32f2_gpio_dsc		gpio_dsc;
	int				platform;

	/*
	 * Configure IOs depending on the board we're running on, and
	 * the configuration options we're using.
	 * Let's control platform strictly: if some of it does not need to
	 * play with iomux, it must be present in switch below (otherwise,
	 * the warning message will be printed-out)
	 */
	platform = stm32_platform_get();
	switch (platform) {

#ifndef CONFIG_ARCH_STM32F1
	/* STM32F2-based platforms */
	case PLATFORM_STM32_STM_STM32F439_SOM:
	case PLATFORM_STM32_STM_STM32F7_SOM:
	case PLATFORM_STM32_STM32F7_DISCO:

#if defined(CONFIG_STM32_USART1)
		gpio_dsc.port = 1;
		gpio_dsc.pin  = 6;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART1);

		gpio_dsc.port = 0;
		gpio_dsc.pin  = 10;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART1);
#endif

#if defined(CONFIG_STM32_USART2)

		gpio_dsc.port = 0;
		gpio_dsc.pin  = 2;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART2);

		gpio_dsc.port = 0;
		gpio_dsc.pin  = 3;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART2);

#endif

#if defined(CONFIG_STM32_USART3)
		gpio_dsc.port = 2;
		gpio_dsc.pin  = 10;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART3);

		gpio_dsc.port = 2;
		gpio_dsc.pin  = 11;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART3);
#endif
#if defined(CONFIG_STM32_USART6)
		gpio_dsc.port = 2;
		gpio_dsc.pin  = 6;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART6);

		gpio_dsc.port = 2;
		gpio_dsc.pin  = 7;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART6);
#endif
#if defined(CONFIG_STM32_USART7)
		gpio_dsc.port = 5;
		gpio_dsc.pin  = 7;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART7);

		gpio_dsc.port = 5;
		gpio_dsc.pin  = 6;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART7);
#endif
#if defined(CONFIG_STM32_CAN1)
		gpio_dsc.port = 1;
		gpio_dsc.pin  = 8;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_CAN1);

		gpio_dsc.port = 1;
		gpio_dsc.pin  = 9;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_CAN1);
#endif
		goto uartdone;

	case PLATFORM_STM32_STM3220G_EVAL:
	case PLATFORM_STM32_STM3240G_EVAL:
	case PLATFORM_STM32_STM_SOM:
	case PLATFORM_STM32_STM_DISCO:

#if defined(CONFIG_STM32_USART1)
		gpio_dsc.port = 0;
		gpio_dsc.pin  = 9;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART1);

		gpio_dsc.port = 0;
		gpio_dsc.pin  = 10;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART1);
#endif
#if defined(CONFIG_STM32_USART3)
		gpio_dsc.port = 2;
		gpio_dsc.pin  = 10;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART3);

		gpio_dsc.port = 2;
		gpio_dsc.pin  = 11;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_USART3);
#endif

uartdone:
#if 0
#if defined(CONFIG_STM32_MAC)
		do {
			static struct stm32f2_gpio_dsc mii_gpio[] = {
				{0,  1}, {0,  2}, {0,  7},
				{1,  5}, {1,  8},
				{2,  1}, {2,  2}, {2,  3}, {2,  4}, {2,  5},
				{6, 11}, {6, 13}, {6, 14},
				{7,  2}, {7,  3}, {7,  6}, {7,  7},
				{8, 10}
			};
			static struct stm32f2_gpio_dsc rmii_gpio[] = {
				{0,  1}, {0,  2}, {0,  7},
				{2,  1}, {2,  4}, {2,  5},
				{6, 11}, {6, 13}, {6, 14},
			};
			int	i;

			if (platform == PLATFORM_STM32_STM_SOM ||
			    platform == PLATFORM_STM32_STM_DISCO ||
			    platform == PLATFORM_STM32_STM_STM32F439_SOM ||
			    platform == PLATFORM_STM32_STM_STM32F7_SOM ||
			    platform == PLATFORM_STM32_STM32F7_DISCO) {
				for (i = 0; i < ARRAY_SIZE(rmii_gpio); i++) {
					stm32f2_gpio_config(&rmii_gpio[i],
						STM32F2_GPIO_ROLE_ETHERNET);
				}
			} else {
				for (i = 0; i < ARRAY_SIZE(mii_gpio); i++) {
					stm32f2_gpio_config(&mii_gpio[i],
						STM32F2_GPIO_ROLE_ETHERNET);
				}
			}
		} while (0);
#endif
#endif

#if defined(CONFIG_STM32_SPI1)
#error		IOMUX for STM32 SPI1 undefined
#endif
#if defined(CONFIG_STM32_SPI2)
		gpio_dsc.port = 8;	/* CLCK */
		gpio_dsc.pin  = 1;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI2);

		gpio_dsc.port = 1;	/* DI */
		gpio_dsc.pin  = 14;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI2);

		gpio_dsc.port = 1;	/* DO */
		gpio_dsc.pin  = 15;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI2);

		gpio_dsc.port = 1;	/* CS */
		gpio_dsc.pin  = 9;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
#endif
#if defined(CONFIG_STM32_SPI3)
#error		IOMUX for STM32 SPI3 undefined
#endif
#if defined(CONFIG_STM32_SPI4)
		gpio_dsc.port = 4;	/* CLCK */
		gpio_dsc.pin  = 2;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI4);

		gpio_dsc.port = 4;	/* DI */
		gpio_dsc.pin  = 5;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI4);

		gpio_dsc.port = 4;	/* DO */
		gpio_dsc.pin  = 6;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI4);

		gpio_dsc.port = 4;	/* CS */
		gpio_dsc.pin  = 4;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
#endif
#if defined(CONFIG_STM32_SPI5)
		gpio_dsc.port = 7;	/* CLCK */
		gpio_dsc.pin  = 6;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI5);

		gpio_dsc.port = 5;	/* DI */
		gpio_dsc.pin  = 8;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI5);

		gpio_dsc.port = 5;	/* DO */
		gpio_dsc.pin  = 9;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_SPI5);

		gpio_dsc.port = 7;	/* CS */
		gpio_dsc.pin  = 5;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
#endif
#if defined(CONFIG_STM32_SPI6)
#error		IOMUX for STM32 SPI6 undefined
#endif

#if defined(CONFIG_STM32_I2C1)
		if (platform == PLATFORM_STM32_STM_STM32F439_SOM ||
			platform == PLATFORM_STM32_STM_STM32F7_SOM) {
			gpio_dsc.port = 1;	/* SCL */
			gpio_dsc.pin  = 8;
		}
		else {
			gpio_dsc.port = 1;	/* SCL */
			gpio_dsc.pin  = 9;
		}
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C1);

		if (platform == PLATFORM_STM32_STM_STM32F439_SOM ||
			platform == PLATFORM_STM32_STM_STM32F7_SOM) {
			gpio_dsc.port = 1;	/* SDA */
			gpio_dsc.pin  = 7;
		}
		else {
			gpio_dsc.port = 1;	/* SDA */
			gpio_dsc.pin  = 8;
		}
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C1);
#endif
#if defined(CONFIG_STM32_I2C2)
		gpio_dsc.port = 1;	/* SCL: PB10*/
		gpio_dsc.pin  = 10;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C2);

		gpio_dsc.port = 1;	/* SDA: PB11*/
		gpio_dsc.pin  = 11;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C2);
#endif
#if defined(CONFIG_STM32_I2C3)
		gpio_dsc.port = 0;	/* SCL: PA8*/
		gpio_dsc.pin  = 8;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C3);

		gpio_dsc.port = 2;	/* SDA: PC9*/
		gpio_dsc.pin  = 9;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C3);
#endif
#if defined(CONFIG_STM32_I2C4)
		gpio_dsc.port = 7;	/* SCL: PH11*/
		gpio_dsc.pin  = 11;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C4);

		gpio_dsc.port = 7;	/* SDA: PH12*/
		gpio_dsc.pin  = 12;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_I2C4);
#endif

#if defined(CONFIG_MMC_ARMMMCI) || defined(CONFIG_MMC_ARMMMCI_MODULE)
		do {
			static struct stm32f2_gpio_dsc sdcard_gpio[] = {
				{2,  8}, {2,  9}, {2, 10}, {2, 11},
				{2, 12}, {3,  2}
			};
			int	i;

			for (i = 0; i < ARRAY_SIZE(sdcard_gpio); i++) {
				stm32f2_gpio_config(&sdcard_gpio[i],
						    STM32F2_GPIO_ROLE_SDIO);
			}
		} while (0);
#endif /* CONFIG_MMC_ARMMMCI */

#if defined(CONFIG_PM)
	if (platform == PLATFORM_STM32_STM_STM32F7_SOM ||
	    platform == PLATFORM_STM32_OLYMPUS_STM32F7) {
		/* PJ4 = Wake-Up GPIO */
		gpio_dsc.port = 9;
		gpio_dsc.pin  = 4;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_IN_PUP);
	}
#endif

#if defined(CONFIG_GPIOLIB) && defined(CONFIG_GPIO_SYSFS)

	/*
	 * Pin configuration for the user pushbutton and
	 * the user LED of the SOM-BSB-EXT baseboard.
	 * !!! That GPIO may have other connections on other baseboards.
	 */
	if (platform == PLATFORM_STM32_STM_STM32F439_SOM) {
		/* PE2 = User Push Button */
		gpio_dsc.port = 4;
		gpio_dsc.pin  = 2;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_IN_PUP);
		/* PB2 = LED DS4 */
		gpio_dsc.port = 1;
		gpio_dsc.pin  = 2;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
	}
	if (platform == PLATFORM_STM32_STM_STM32F7_SOM) {
		/* PH2 = User Push Button */
		gpio_dsc.port = 7;
		gpio_dsc.pin  = 2;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_IN_PUP);
		/* PB2 = LED DS4 */
		gpio_dsc.port = 1;
		gpio_dsc.pin  = 2;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
	}
	if (platform == PLATFORM_STM32_STM32F7_DISCO) {
		/* PI11 = User Push Button */
		gpio_dsc.port = 8;
		gpio_dsc.pin  = 11;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_IN_PUP);
		/* PI1 = LED LD1 */
		gpio_dsc.port = 8;
		gpio_dsc.pin  = 1;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
	}

	/*
	 * Pin configuration for the User Push Button and User LEDs
	 * on the STM32F429 Discovery board.
	 * !!! That GPIO may have other connections on other boards.
	 */
	if (platform == PLATFORM_STM32_STM_DISCO) {
		/* PA0 = User Push Button */
		gpio_dsc.port = 0;
		gpio_dsc.pin  = 0;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_IN);
		/* PE3 = LED_3
		 * Here, things are tricky. The default hardware routes
		 * the LED to a pin that interferes with RMII. The config
		 * below assumes that the RMII signal is disconnected from
		 * the LED and instead the LED is connected to the PE3 GPIO.
		 */
		gpio_dsc.port = 4;
		gpio_dsc.pin  = 3;
		stm32f2_gpio_config(&gpio_dsc, STM32F2_GPIO_ROLE_OUT);
	}

#endif /* CONFIG_GPIOLIB */

#if defined(CONFIG_STM32_FB)
		if (!stm32f4_fb_is_running()) {
			static struct stm32f2_gpio_dsc lcd_gpio[] = {
# if defined(CONFIG_ARCH_STM32F7)
				{8, 14}, /* PI14 = LCD_CLK */
				{10,7 }, /* PK7  = LCD_DE */
				{8, 12}, /* PI12 = LCD_HSYNC */
				{8, 13}, /* PI13 = LCD_VSYNC */
				{9, 12}, /* PJ12 = LCD_B0 */
				{9, 13}, /* PJ13 = LCD_B1 */
				{9, 14}, /* PJ14 = LCD_B2 */
				{9, 15}, /* PJ15 = LCD_B3 */
				{10,3 }, /* PK3  = LCD_B4 */
				{10,4 }, /* PK4  = LCD_B5 */
				{10,5 }, /* PK5  = LCD_B6 */
				{10,6 }, /* PK6  = LCD_B7 */
				{9, 7 }, /* PJ7  = LCD_G0 */
				{9, 8 }, /* PJ8  = LCD_G1 */
				{9, 9 }, /* PJ9  = LCD_G2 */
				{9, 10}, /* PJ10 = LCD_G3 */
				{9, 11}, /* PJ11 = LCD_G4 */
				{10,0 }, /* PK0  = LCD_G5 */
				{10,1 }, /* PK1  = LCD_G6 */
				{10,2 }, /* PK2  = LCD_G7 */
				{8, 15}, /* PI15 = LCD_R0 */
				{9, 0 }, /* PJ0  = CD_R1 */
				{9, 1 }, /* PJ1  = LCD_R2 */
				{9, 2 }, /* PJ2  = LCD_R3 */
				{9, 3 }, /* PJ3  = LCD_R4 */
				{9, 4 }, /* PJ4  = LCD_R5 */
				{9, 5 }, /* PJ5  = LCD_R6 */
				{9, 6 }, /* PJ6  = LCD_R7 */
# else
				{6, 7 }, /* PG7  = LCD_CLK */
				{5, 10}, /* PF10 = LCD_DE */
				{8, 10}, /* PI10 = LCD_HSYNC */
				{8, 9 }, /* PI9  = LCD_VSYNC */
				{6, 12}, /* PG12 = LCD_B1 */
				{6, 10}, /* PG10 = LCD_B2 */
				{8, 4 }, /* PI4  = LCD_B4 */
				{8, 5 }, /* PI5  = LCD_B5 */
				{8, 6 }, /* PI6  = LCD_B6 */
				{8, 7 }, /* PI7  = LCD_B7 */
				{7, 13}, /* PH13 = LCD_G2 */
				{7, 14}, /* PH14 = LCD_G3 */
				{7, 15}, /* PH15 = LCD_G4 */
				{8, 0 }, /* PI0  = LCD_G5 */
				{2, 7 }, /* PC7  = LCD_G6 */
				{8, 2 }, /* PI2  = LCD_G7 */
				{7, 2 }, /* PH2  = LCD_R0 */
				{7, 3 }, /* PH3  = LCD_R1 */
				{7, 8 }, /* PH8  = LCD_R2 */
				{7, 9 }, /* PH9  = LCD_R3 */
				{7, 10}, /* PH10 = LCD_R4 */
				{7, 11}, /* PH11 = LCD_R5 */
				{7, 12}, /* PH12 = LCD_R6 */
				{6, 6 }, /* PG6  = LCD_R7 */
# endif
			};
			int i;

			for (i = 0; i < ARRAY_SIZE(lcd_gpio); i++) {
				stm32f2_gpio_config(&lcd_gpio[i],
						    STM32F2_GPIO_ROLE_LTDC);
			}
		}
#endif /* CONFIG_STM32_FB */

		break;
#else
	/* STM32F1-based platforms */
	case PLATFORM_STM32_SWISSEMBEDDED_COMM:
		/*
		 * Rely on the IOMUX configuration initialized by the bootloader
		 */
		break;
#endif
	default:
		printk(KERN_WARNING "%s: unsupported platform %d\n", __func__,
			platform);
		/*
		 * Just to avoid compilation warns in case of configuration
		 * which doesn't need iomux, 'use' gpio_dsc var
		 */
		gpio_dsc.port = gpio_dsc.pin = 0;
		break;
	}
}
