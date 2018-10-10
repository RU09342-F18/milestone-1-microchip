#include <msp430.h> 

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // Stops the watchdog timer

    return 0;
}
