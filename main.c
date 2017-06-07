#include <msp430.h> 
#include "stdio.h"
#include "stdint.h"
#include "nrf24l01.h"

/* My address */
uint8_t MyAddress[] = {
    0xE7,
    0xE7,
    0xE7,
    0xE7,
    0xE7
};
/* Receiver address */
uint8_t TxAddress[] = {
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E
};

uint8_t dataOut[32], dataIn[32];

/* Interrupt status */
NRF24L01_Transmit_Status_t transmissionStatus;


/*
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer

    /* configure internal digitally controlled oscillator */
    DCOCTL  = CALDCO_8MHZ;
    BCSCTL1 = CALBC1_8MHZ;

    P1IE |=  BIT3;                            // P1.3 interrupt enabled
    P1IES |= BIT3;                            // P1.3 Hi/lo edge
    P1REN |= BIT3;                            // Enable Pull Up on SW2 (P1.3)
    P1IFG &= ~BIT3;                           // P1.3 IFG cleared
                                              //BIT3 on Port 1 can be used as Switch2
    P1DIR |= BIT0 + BIT6;
    P1OUT |= BIT0 + BIT6;

    NRF24L01_Init(15, 32);

    /* Set 2MBps data rate and -18dBm output power */
    NRF24L01_SetRF(NRF24L01_DataRate_2M, NRF24L01_OutputPower_M18dBm);

    /* Set my address, 5 bytes */
    NRF24L01_SetMyAddress(MyAddress);

    /* Set TX address, 5 bytes */
    NRF24L01_SetTxAddress(TxAddress);
    __delay_cycles(500000);
    P1OUT &=~(BIT6 + BIT0);
    while(1)
    {
        sprintf((char *)dataOut, "abcdefghijklmnoszxABCDEFCBDA");
        NRF24L01_Transmit(dataOut);
        /* Set NRF state to sending */
        transmissionStatus = NRF24L01_Transmit_Status_Sending;

        if(transmissionStatus != NRF24L01_Transmit_Status_Sending)
        {
            /* Transmission was OK */
            if (transmissionStatus == NRF24L01_Transmit_Status_Ok)
            {
                P1OUT |= BIT6;
            }

            /* Message LOST */
            if (transmissionStatus == NRF24L01_Transmit_Status_Lost)
            {
                P1OUT |= BIT0;
            }
        }
        __delay_cycles(500000);

    }
	
}


/* Interrupt handler */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1_Handler(void)
{
    if(!(P1IN & BIT3))
    {
        /* Read interrupts */
        uint8_t irq = NRF24L01_GET_INTERRUPTS;

        /* Check if transmitted OK */
        if (irq & NRF24L01_IRQ_TRAN_OK)
        {
            /* Save transmission status */
            transmissionStatus = NRF24L01_Transmit_Status_Ok;

            /* Turn off led */
            P1OUT &= ~BIT6;

            /* Go back to RX mode */
            NRF24L01_PowerUpRx();
        }


        /* Check if max transmission reached and last transmission failed */
        if (irq & NRF24L01_IRQ_MAX_RT)
        {
            /* Save transmission status */
            transmissionStatus = NRF24L01_Transmit_Status_Lost;

            /* Turn off led */
            P1OUT &= ~BIT0;

            /* Go back to RX mode */
            NRF24L01_PowerUpRx();
        }

        /* If data is ready on NRF24L01+ */
        if (irq & NRF24L01_IRQ_DATA_READY)
        {
            /* Get data from NRF24L01+ */
            NRF24L01_GetData(dataIn);
        }

        /* Clear interrupts */
        NRF24L01_CLEAR_INTERRUPTS;

        P1IFG &= ~BIT3;                           // P1.3 IFG cleared
    }
}
