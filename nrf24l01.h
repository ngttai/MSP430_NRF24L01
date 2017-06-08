/*
 * nrf24l01.h
 *
 *  Created on: Jun 7, 2017
 *      Author: nthanhtai
 */

#ifndef NRF24L01_H_
#define NRF24L01_H_

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * This library allows you to work with nRF24L01+ modules.
 *
 * You can send and receive data from nRF24L01+ modules.
 *
 * \par Default pinout
 *
@verbatim
NRF24L01+   MSP430   DESCRIPTION

GND         GND         Ground
VCC         3.3V        3.3V
CE          PD8         RF activated pin
CSN         PD7         Chip select pin for SPI
SCK         PC10        SCK pin for SPI
MOSI        PC12        MOSI pin for SPI
MISO        PC11        MISO pin for SPI
IRQ         Not used    Interrupt pin. Goes low when active. Pin functionality is active, but not used in library
 *
 * IRQ pin is not used in this library, but it's functionality is enabled by this software.
 * You can still set any pin on F4xx to be an external interrupt and handle interrupts from nRF24L01+ module.
 * The easiest way to that is to use TM EXTI library and attach interrupt functionality to this pin
*/

#include "msp430.h"
#include "stdint.h"
/**
 * @defgroup NRF24L01P_Macros
 * @brief    Library defines
 * @{
 */

#ifndef st
#define st(x)      do { x } while (__LINE__ == -1)
#endif
/* bit value */
#ifndef BV
#define BV(n)      (1 << (n))
#endif


/* Default SPI used */
#define USE_INTF_BITBANG
//#define USE_INTF_USCIB0

//Define I/O for SPI Software
#ifdef USE_INTF_BITBANG
/* Define for SIMO */
#define SPI_SIMO_DIR P2DIR
#define SPI_SIMO_OUT P2OUT
#define SPI_SIMO_IN  P2IN
#define SPI_BIT_SIMO  BIT4
/* Define for SOMI */
#define SPI_SOMI_DIR P2DIR
#define SPI_SOMI_OUT P2OUT
#define SPI_SOMI_IN  P2IN
#define SPI_BIT_SOMI  BIT2
/* Define for CLK */
#define SPI_CLK_DIR P2DIR
#define SPI_CLK_OUT P2OUT
#define SPI_CLK_IN  P2IN
#define SPI_BIT_CLK  BIT1

#endif /* USE_INTF_BITBANG */

/* SPI chip enable pin */
#define NRF24L01_CSN_BIT            3

/* Chip enable for transmitting */
#define NRF24L01_CE_BIT             0

/* Pins configuration for CE */
#define NRF24L01_CE_INIT            st(P2DIR |=  BV(NRF24L01_CE_BIT);)
#define NRF24L01_CE_LOW             st(P2OUT &=  ~BV(NRF24L01_CE_BIT);)
#define NRF24L01_CE_HIGH            st(P2OUT |=  BV(NRF24L01_CE_BIT);)
/* Pins configuration for CSN */
#define NRF24L01_CSN_INIT           st(P2DIR |=  BV(NRF24L01_CSN_BIT);)
#define NRF24L01_CSN_LOW            st(P2OUT &=  ~BV(NRF24L01_CSN_BIT);)
#define NRF24L01_CSN_HIGH           st(P2OUT |=  BV(NRF24L01_CSN_BIT);)

/**
 * @defgroup NRF24L01P_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Transmission status enumeration
 */
typedef enum {
    NRF24L01_Transmit_Status_Lost = 0x00,   /*!< Message is lost, reached maximum number of retransmissions */
    NRF24L01_Transmit_Status_Ok = 0x01,     /*!< Message sent successfully */
    NRF24L01_Transmit_Status_Sending = 0xFF /*!< Message is still sending */
} NRF24L01_Transmit_Status_t;

/**
 * @brief  Data rate enumeration
 */
typedef enum {
    NRF24L01_DataRate_2M,  /*!< Data rate set to 2Mbps */
    NRF24L01_DataRate_1M,  /*!< Data rate set to 1Mbps */
    NRF24L01_DataRate_250k /*!< Data rate set to 250kbps */
} NRF24L01_DataRate_t;

/**
 * @brief  Output power enumeration
 */
typedef enum {
    NRF24L01_OutputPower_M18dBm, /*!< Output power set to -18dBm */
    NRF24L01_OutputPower_M12dBm, /*!< Output power set to -12dBm */
    NRF24L01_OutputPower_M6dBm,  /*!< Output power set to -6dBm */
    NRF24L01_OutputPower_0dBm    /*!< Output power set to 0dBm */
} NRF24L01_OutputPower_t;

/* Clear interrupt flags */
#define NRF24L01_CLEAR_INTERRUPTS   do { NRF24L01_WriteRegister(0x07, 0x70); } while (0)

/* Gets interrupt status from device */
#define NRF24L01_GET_INTERRUPTS     NRF24L01_GetStatus()

/* Interrupt masks */
#define NRF24L01_IRQ_DATA_READY     0x40 /*!< Data ready for receive */
#define NRF24L01_IRQ_TRAN_OK        0x20 /*!< Transmission went OK */
#define NRF24L01_IRQ_MAX_RT         0x10 /*!< Max retransmissions reached, last transmission failed */

/**
 * @}
 */

/**
 * @defgroup NRF24L01P_Functions
 * @brief    Library Functions
 *
 * Here are listed very basic functions to work with NRF modules
 *
 * @{
 */

/**
 * @brief  Initializes NRF24L01+ module
 * @param  channel: channel you will use for communication, from 0 to 125 eg. working frequency from 2.4 to 2.525 GHz
 * @param  payload_size: maximum data to be sent in one packet from one NRF to another.
 * @note   Maximal payload size is 32bytes
 * @retval 1
 */
uint8_t NRF24L01_Init(uint8_t channel, uint8_t payload_size);

/**
 * @brief  Sets own address. This is used for settings own id when communication with other modules
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void NRF24L01_SetMyAddress(uint8_t* adr);

/**
 * @brief  Sets address you will communicate with
 * @note   "Own" address of one device must be the same as "TX" address of other device (and vice versa),
 *         if you want to get successful communication
 * @param  *adr: Pointer to 5-bytes length array with address
 * @retval None
 */
void NRF24L01_SetTxAddress(uint8_t* adr);

/**
 * @brief  Gets number of retransmissions needed in last transmission
 * @param  None
 * @retval Number of retransmissions, between 0 and 15.
 */
uint8_t NRF24L01_GetRetransmissionsCount(void);

/**
 * @brief  Sets NRF24L01+ to TX mode
 * @note   In this mode is NRF able to send data to another NRF module
 * @param  None
 * @retval None
 */
void NRF24L01_PowerUpTx(void);

/**
 * @brief  Sets NRF24L01+ to RX mode
 * @note   In this mode is NRF able to receive data from another NRF module.
 *         This is default mode and should be used all the time, except when sending data
 * @param  None
 * @retval None
 */
void NRF24L01_PowerUpRx(void);

/**
 * @brief  Sets NRF24L01+ to power down mode
 * @note   In power down mode, you are not able to transmit/receive data.
 *         You can wake up device using @ref NRF24L01_PowerUpTx() or @ref NRF24L01_PowerUpRx() functions
 * @param  None
 * @retval None
 */
void NRF24L01_PowerDown(void);

/**
 * @brief  Gets transmissions status
 * @param  None
 * @retval Transmission status. Return is based on @ref NRF24L01_Transmit_Status_t enumeration
 */
NRF24L01_Transmit_Status_t NRF24L01_GetTransmissionStatus(void);

/**
 * @brief  Transmits data with NRF24L01+ to another NRF module
 * @param  *data: Pointer to 8-bit array with data.
 *         Maximum length of array can be the same as "payload_size" parameter on initialization
 * @retval None
 */
void NRF24L01_Transmit(uint8_t *data);

/**
 * @brief  Checks if data is ready to be read from NRF24L01+
 * @param  None
 * @retval Data ready status:
 *            - 0: No data available for receive in bufferReturns
 *            - > 0: Data is ready to be collected
 */
uint8_t NRF24L01_DataReady(void);

/**
 * @brief  Gets data from NRF24L01+
 * @param  *data: Pointer to 8-bits array where data from NRF will be saved
 * @retval None
 */
void NRF24L01_GetData(uint8_t *data);

/**
 * @brief  Sets working channel
 * @note   Channel value is just an offset in units MHz from 2.4GHz
 *         For example, if you select channel 65, then operation frequency will be set to 2.465GHz.
 * @param  channel: RF channel where device will operate
 * @retval None
 */
void NRF24L01_SetChannel(uint8_t channel);

/**
 * @brief  Sets RF parameters for NRF24L01+
 * @param  DataRate: Data rate selection for NRF module. This parameter can be a value of @ref NRF24L01_DataRate_t enumeration
 * @param  OutPwr: Output power selection for NRF module. This parameter can be a value of @ref NRF24L01_OutputPower_t enumeration
 * @retval None
 */
void NRF24L01_SetRF(NRF24L01_DataRate_t DataRate, NRF24L01_OutputPower_t OutPwr);

/**
 * @brief  Gets NRLF+ status register value
 * @param  None
 * @retval Status register from NRF
 */
uint8_t NRF24L01_GetStatus(void);

/* Private */
void NRF24L01_WriteRegister(uint8_t reg, uint8_t value);

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif




#endif /* NRF24L01_H_ */
