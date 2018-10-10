/*
#include <msp430.h> 

void configureUARTLED () {
    //Onboard LED for RX debugging
    P1DIR |= BIT0;              // Sets P1.0 to the output direction
    P1OUT &= ~BIT0;             // Clears P1.0 register
}

void configureLEDPWM () {
    // Red = P1.6 TA0 CCR0
    // Green = 2.1 TA1 CCR1
    // Blue = 2.4 TA1 CCR2

    // P1.6 Config
    P1SEL |= BIT6;              //
    P1SEL2 &= ~BIT6;            // Connects P1.6 to TIMERA
    P1DIR |= BIT6;              // Sets P1.6 to the output direction

    // P2.1 Config
    P2SEL |= BIT1;              //
    P2SEL2 &= ~BIT1;            // Connects P2.1 to TIMERA
    P2DIR |= BIT1;              // Sets P2.1 to the output direction

    // P2.4 Config
    P2SEL |= BIT4;              //
    P2SEL2 &= ~BIT4;            // Connects P2.4 to TIMERA
    P2DIR |= BIT4;              // Sets P2.4 to the output direction
}

void configureTimer () {
    // TA0 configuration
    TA0CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;
                                // Sets the timer to SM clock, up mode,
                                // the internal divider to 4, and
                                // clears the clock
    TA0CCR0 = 255;              // Sets up the PWM length to 1Mhz
    TA0CCR1 = 0;                // Sets the duty cycle to 0%
    TA0CCTL1 = OUTMOD_3;        // Sets clock to set/reset

    // TA1 configuration
    TA1CTL = TASSEL_2 + ID_2 + MC_1 + TACLR;
                                // Sets the timer to SM clock, up mode,
                                // the internal divider to 4, and
                                // clears the clock
    TA1CCR0 = 255;              // Sets up the PWM length to 1Mhz
    TA1CCR1 = 0;                // Sets the duty cycle to 0%
    TA1CCR2 = 0;                // Sets the duty cycle to 0%
    TA1CCTL1 = OUTMOD_3;        // Sets clock to set/reset
    TA1CCTL2 = OUTMOD_3;        // Sets clock to set/reset

}

void configureUART () {
    // Pin configuration for TX and RX
    P1SEL |= BIT1 + BIT2;       // Connect P1.1 to RX and P1.2 to TX
    P1SEL2 |= BIT1 + BIT2;      // Connect P1.1 to RX and P1.2 to TX

    // Sets BAUD rate to 9600
    UCA0CTL1 |= UCSSEL_2;       // Sets the BR clock to SM clock
    UCA0BR0 = 109;              // Sets the baud rate to 9600
    UCA0BR1 = 0;                // Sets the baud rate to 9600
    UCA0MCTL = UCBRS_2;         // Sets the baud rate to 9600

    UCA0CTL1 &= ~UCSWRST;       // Disables UART reset
    UC0IE |= UCA0RXIE;          // Enable UART based interrupt
    __bis_SR_register(LPM0_bits + GIE);
                                // Enter low power mode 0 and enable
                                // global interrupts
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stops the watchdog timer
    configureLEDPWM();
    configureTimer();
    configureUART();
    return 0;
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    P1OUT |= BIT0;              // Turns on onboard LED when UART
                                // message is received
    //char data = UCA0RXBUF;      // Stores the data that is received
                                // in the RX buffer and clears the flags

    // TESTING ONLY
    //TA0CCR1 = data;

    //P1OUT &= ~BIT0;             // Turns off onboard LED
}
*/

#include "msp430.h"

#define TXLED BIT0
#define RXLED BIT0
#define TXD BIT2
#define RXD BIT1

const char string[] = { "Hello World\r\n" };
unsigned int i; //Counter

int main(void)
{
   WDTCTL = WDTPW + WDTHOLD; // Stop WDT
   DCOCTL = 0; // Select lowest DCOx and MODx settings
   BCSCTL1 = CALBC1_1MHZ; // Set DCO
   DCOCTL = CALDCO_1MHZ;
   P2DIR |= 0xFF; // All P2.x outputs
   P2OUT &= 0x00; // All P2.x reset
   P1SEL |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
   P1SEL2 |= RXD + TXD ; // P1.1 = RXD, P1.2=TXD
   P1DIR |= RXLED + TXLED;
   P1OUT &= 0x00;
   UCA0CTL1 |= UCSSEL_2; // SMCLK
   UCA0BR0 = 0x08; // 1MHz 115200
   UCA0BR1 = 0x00; // 1MHz 115200
   UCA0MCTL = UCBRS2 + UCBRS0; // Modulation UCBRSx = 5
   UCA0CTL1 &= ~UCSWRST; // **Initialize USCI state machine**
   UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
   __bis_SR_register(CPUOFF + GIE); // Enter LPM0 w/ int until Byte RXed
   while (1)
   { }
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
   P1OUT |= TXLED;
     UCA0TXBUF = string[i++]; // TX next character
    if (i == sizeof string - 1) // TX over?
       UC0IE &= ~UCA0TXIE; // Disable USCI_A0 TX interrupt
    P1OUT &= ~TXLED; }

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
   P1OUT |= RXLED;
    if (UCA0RXBUF == 'a') // 'a' received?
    {
       i = 0;
       UC0IE |= UCA0TXIE; // Enable USCI_A0 TX interrupt
      UCA0TXBUF = string[i++];
    }
    P1OUT &= ~RXLED;
}
