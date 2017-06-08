#include <msp430.h> 
#include "stdio.h"
#include "stdint.h"
#include "nrf24l01.h"

#define RUN_TX

#ifdef RUN_TX
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
uint8_t dataOut[6];
#else
/* Receiver address */
uint8_t TxAddress[] = {
    0xE7,
    0xE7,
    0xE7,
    0xE7,
    0xE7
};
/* My address */
uint8_t MyAddress[] = {
    0x7E,
    0x7E,
    0x7E,
    0x7E,
    0x7E
};
uint8_t dataIn[6];
#endif

static volatile uint8_t enable;
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

    /*Configuare Timer A0 CCR0*/
    CCTL0 = CCIE;                             // CCR0 interrupt enabled
    CCR0 = 50000;
    TACTL = TASSEL_2 + MC_1 + ID_3;                  // SMCLK, upmode, DIV 8

    /* Intiialize LED */
    P1DIR |= BIT0 + BIT6;
    P1OUT |= BIT0 + BIT6;

    /* Initialize NRF24L01+ on channel 15 and 32bytes of payload */
    /* By default 2Mbps data rate and 0dBm output power */
    /* NRF24L01 goes to RX mode by default */
    NRF24L01_Init(15, 32);

    /* Set 2MBps data rate and -18dBm output power */
    NRF24L01_SetRF(NRF24L01_DataRate_2M, NRF24L01_OutputPower_M18dBm);

    /* Set my address, 5 bytes */
    NRF24L01_SetMyAddress(MyAddress);

    /* Set TX address, 5 bytes */
    NRF24L01_SetTxAddress(TxAddress);
    __delay_cycles(500000);
    P1OUT &=~(BIT6 + BIT0);
    __enable_interrupt();
    while(1)
    {
#ifdef RUN_TX
        if(enable > 40)
        {
            enable =0; //reset enable
            sprintf((char *)dataOut, "NTTai");
            NRF24L01_Transmit(dataOut);
            /* Set NRF state to sending */
            transmissionStatus = NRF24L01_Transmit_Status_Sending;
        }
        if((transmissionStatus != NRF24L01_Transmit_Status_Sending) && (enable != 0))
        {
            /* Transmission was OK */
            if (transmissionStatus == NRF24L01_Transmit_Status_Ok)
            {
                P1OUT &= ~BIT6;
            }
            /* Message LOST */
            if (transmissionStatus == NRF24L01_Transmit_Status_Lost)
            {
                P1OUT &= ~BIT0;
            }
        }
#else /* RUN_TX */
        /* Something esle */
        __delay_cycles(500000);
        P1OUT &= ~BIT6;
#endif /* !RUN_TX */
    }
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
    enable++;
}

/* Interrupt handler */
#pragma vector=PORT1_VECTOR
__interrupt void Port_1_Handler(void)
{
    if(!(P1IN & BIT3))
    {
        /* Read interrupts */
        uint8_t irq = NRF24L01_GET_INTERRUPTS;
#ifdef RUN_TX
        /* Check if transmitted OK */
        if (irq & NRF24L01_IRQ_TRAN_OK)
        {
            /* Save transmission status */
            transmissionStatus = NRF24L01_Transmit_Status_Ok;
            /* Turn on led */
            P1OUT |= BIT6;
            /* Go back to RX mode */
            NRF24L01_PowerUpRx();
        }

        /* Check if max transmission reached and last transmission failed */
        if (irq & NRF24L01_IRQ_MAX_RT)
        {
            /* Save transmission status */
            transmissionStatus = NRF24L01_Transmit_Status_Lost;
            /* Turn on led */
            P1OUT |= BIT0;
            /* Go back to RX mode */
            NRF24L01_PowerUpRx();
        }
#else
        /* If data is ready on NRF24L01+ */
        if (irq & NRF24L01_IRQ_DATA_READY)
        {
            /* Get data from NRF24L01+ */
            NRF24L01_GetData(dataIn);
            if(dataIn[0] == 'a')
                P1OUT |= BIT6;
        }
#endif
        /* Clear interrupts */
        NRF24L01_CLEAR_INTERRUPTS;

        P1IFG &= ~BIT3;                           // P1.3 IFG cleared
    }
}
