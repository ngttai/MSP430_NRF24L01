/* Copyright (c) 2007 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is property of Nordic Semiconductor ASA.
 * Terms and conditions of usage are described in detail in NORDIC
 * SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT. 
 *
 * Licensees are granted free, non-transferable use of the information. NO
 * WARRENTY of ANY KIND is provided. This heading must NOT be removed from
 * the file.
 *
 * $LastChangedRevision: 2132 $
 */ 

/** @file
 * Header file defining the hardware depenent interface of the msp430
 *
 *
 */

#ifndef HAL_NRF_HW_H__
#define HAL_NRF_HW_H__

#include <msp430.h>
#ifndef BSP_H__
#include <stdint.h>
#endif

#define MISO_PxDIR    P1DIR
#define MISO_PxIN     P1IN
#define MISO_BIT      BIT3

#define MOSI_PxDIR    P1DIR
#define MOSI_PxOUT    P1OUT
#define MOSI_BIT      BIT4

#define CLK_PxDIR    P1DIR
#define CLK_PxOUT    P1OUT
#define CLK_BIT      BIT5

#define CSN_PxDIR    P1DIR
#define CSN_PxOUT    P1OUT
#define CSN_BIT      BIT1

#define CE_PxDIR    P1DIR
#define CE_PxOUT    P1OUT
#define CE_BIT     BIT2

/** Macro that set radio's CSN line LOW.
 *
 */
#define CSN_LOW() do {CSN_PxOUT &= ~CSN_BIT;} while(0)

/** Macro that set radio's CSN line HIGH.
 *
 */
#define CSN_HIGH() do {CSN_PxOUT |= CSN_BIT;} while(0)

/** Macro that set radio's CE line LOW.
 *
 */
#define CE_LOW() do {CE_PxOUT &= ~CE_BIT;} while(0)

/** Macro that set radio's CE line HIGH.
 *
 */
#define CE_HIGH() do {CE_PxOUT |= CE_BIT;} while(0)

/**
 * Pulses the CE to nRF24L01 for at least 10 us
 */
#define CE_PULSE() do { \
  uint8_t count; \
  count = 20; \
  CE_HIGH();  \
  while(count--) \
    ; \
  CE_LOW();  \
  } while(0)

#endif /* HAL_NRF_HW_H__ */
