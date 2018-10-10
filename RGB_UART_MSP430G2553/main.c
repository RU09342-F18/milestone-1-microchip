#include <msp430.h> 

void configureLEDPWM () {
    // Red = P1.6 TA0 CCR0
    // Green = 2.1 TA1 CCR1
    // Blue = 2.4 TA1 CCR2

    // P1.6 Config
    P1SEL |= BIT6;      //
    P1SEL2 &= ~BIT6;    // Connects P1.6 to TIMERA
    P1DIR |= BIT6;      // Sets P1.6 to the output direction

    // P2.1 Config
    P2SEL |= BIT1;      //
    P2SEL2 &= ~BIT1;    // Connects P2.1 to TIMERA
    P2DIR |= BIT1;      // Sets P2.1 to the output direction

    // P2.4 Config
    P2SEL |= BIT4;      //
    P2SEL2 &= ~BIT4;    // Connects P2.4 to TIMERA
    P2DIR |= BIT4;      // Sets P2.4 to the output direction
}

void configureTimer () {

}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stops the watchdog timer
    configureLEDPWM();
    configureTimer();
    return 0;
}
