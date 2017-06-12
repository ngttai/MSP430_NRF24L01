/*
 * Copyright (c) 2006 Nordic Semiconductor. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 *
 */
/** @file
 *  C-file defining the hardware depenent interface of the nRF24l01.
 *
 *  @author Jon Helge Nistad
 *
 */

#include "hal_nrf.h"
#include "hal_nrf_hw.h"
#ifndef BSP_H__
#include <stdint.h>
#endif

/*

// Output eight-bit value using selected bit-bang pins
void SPI_bitbang_out(uint8_t value)
{
  uint8_t x;

  for(x=8; x>0; x--)
  {
    if(value & 0x80)                                     // If bit is high...
        MOSI_PxOUT |= MOSI_BIT; // Set SIMO high...
    else
        MOSI_PxOUT &= ~MOSI_BIT;// Set SIMO low...
    value = value << 1;                                  // Rotate bits

    CLK_PxOUT &= ~CLK_BIT;  // Set clock low
    CLK_PxOUT |= CLK_BIT;   // Set clock high
  }
  CLK_PxOUT &= ~CLK_BIT;  // Set clock low
}

// Input eight-bit value using selected bit-bang pins
uint8_t SPI_bitbang_in()
{
    uint8_t x = 0, shift = 0;
    uint8_t y;

    // Determine how many bit positions SOMI is from least-significant bit
    x = MISO_BIT;
    while(x > 1)
    {
        shift++;
        x = x >> 1;
    };

    x = 0;
    for(y=8;y>0;y--)
    {
        CLK_PxOUT &= ~CLK_BIT;  // Set clock low
        CLK_PxOUT |= CLK_BIT;   // Set clock high

        x = x << 1;                             // Make room for next bit
        x = x | ((MISO_PxIN & MISO_BIT) >> shift);
    }                                         // Store next bit
    CLK_PxOUT &= ~CLK_BIT;  // Set clock low
    return(x);
}

*/

uint8_t hal_nrf_rw(uint8_t value)
{
    uint8_t x = 0, shift = 0;
    uint8_t y;

    // Determine how many bit positions SOMI is from least-significant bit
    x = MISO_BIT;
    while(x > 1)
    {
        shift++;
        x = x >> 1;
    };

    x = 0;
    for(y=8; y>0; y--)
    {
        if(value & 0x80)                                     // If bit is high...
            MOSI_PxOUT |= MOSI_BIT; // Set SIMO high...
        else
            MOSI_PxOUT &= ~MOSI_BIT;// Set SIMO low...
        value = value << 1;                                  // Rotate bits

        CLK_PxOUT &= ~CLK_BIT;  // Set clock low
        CLK_PxOUT |= CLK_BIT;   // Set clock high

        x = x << 1;                             // Make room for next bit
        x = x | ((MISO_PxIN & MISO_BIT) >> shift);
    }                                         // Store next bit
    CLK_PxOUT &= ~CLK_BIT;  // Set clock low
    return(x);
}
