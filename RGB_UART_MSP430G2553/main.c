#include <msp430.h> 

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

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stops the watchdog timer
    configureLEDPWM();
    configureTimer();
    return 0;
}
