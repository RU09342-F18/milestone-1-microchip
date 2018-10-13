#include <msp430.h> 
char counter;
char total;

void configureUARTLED () {
    //Onboard LED for RX debugging
    P1DIR |= BIT0;              // Sets P1.0 to the output direction
    P1OUT &= ~BIT0;             // Clears P1.0 register
}

void configureLEDPWM () {
    // Red = P1.6 TA0 CCR0
    // Green = P2.1 TA1 CCR1
    // Blue = P2.4 TA1 CCR2

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
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stops the watchdog timer
    configureLEDPWM();
    configureTimer();
    configureUART();
    __bis_SR_register(LPM0_bits + GIE);
                                // Enter low power mode 0 and enable
                                // global interrupts
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void) {
    P1OUT |= BIT0;              // Turns on onboard LED when UART
                                // message is received
    UC0IE |= UCA0TXIE;          // Enable transmit based interrup
    char data = UCA0RXBUF;      // Stores the data that is received
                                // in the RX buffer and clears the flags

    if (counter == 0){
        counter = data;
        total = data;
        if (data >= 8) {
            UCA0TXBUF = data - 3;
        }
    } else if (total - counter == 1){
        TA0CCR1 = data;
    } else if (total - counter == 2){
        TA1CCR1 = data;
    } else if (total - counter == 3){
        TA1CCR2 = data;
    } else {
        if (total >= 8) {
            UCA0TXBUF = data;
        }
    }

    counter --;
    P1OUT &= ~BIT0;             // Turns off onboard LED
}

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
   P1OUT |= BIT0;               // Turns on onboard LED when UART
                                // message is sent
   UC0IE &= ~UCA0TXIE;          // Clears the TX based interrupt
   P1OUT &= ~BIT0;              // Turns off the onboard LED
}
