/*
 * File:   main.c
 * Author: Nate Hoffman
 *
 * Created on October 13, 2018, 7:43 PM
 */

#include "main.h"
// config fuse
#pragma config OSC		= HSPLL
#pragma config WDT		= OFF
#pragma config PWRT		= ON
#pragma config LVP		= OFF
#pragma config BOREN	= OFF
#pragma config BORV		= 1
#pragma config XINST	= OFF
#define _XTAL_FREQ 40000000

// On board LED for showing the status of the UART
#define LED PORTAbits.RA4

// RGB LED PWM
volatile unsigned char rDuty = 0;
volatile unsigned char gDuty = 0;
volatile unsigned char bDuty = 0;
volatile unsigned char counterLED = 0;

// Packet decoding
volatile unsigned char counterPacket = 0;
volatile unsigned char counterTotal = 0;

void main(void) {
    // UART status LED
    TRISA4 = 0;
    LED = 1;

    // Output RGB LED
    TRISB0 = 0;
    TRISB1 = 0;
    TRISB2 = 0;
    PORTB = 0;

    // Timer 2 - for PWM
    PIR1bits.TMR2IF = 0;
    PIE1bits.TMR2IE = 1;
    PR2 = 77; // 77 - 500 Hz, 96 - 400 Hz 197 - 200 Hz
    // Turn on timer 2
    T2CONbits.TMR2ON = 1;

    // Configure UART
    TRISC7 = 1;
    TRISC6 = 0;
    SPBRG = 64;

    // Turn on UART
    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
    PIE1bits.TXIE = 0;
    PIE1bits.RCIE = 1;

    // Interrupt enable
    INTCONbits.PEIE = 1;
    INTCONbits.GIE = 1;

    while (1) {
    }
}

// Interrupt service routine

void __interrupt(high_priority) myHighISR(void) {
    // Timer
    if (PIR1bits.TMR2IF) {
        PIR1bits.TMR2IF = 0;

        counterLED++;

        char rValue;
        char gValue;
        char bValue;

        // Calculate if the red LED should be on
        if ((counterLED == rDuty) && (rDuty != 255)) {
            rValue = 1;
        } else if (counterLED == 0) {
            rValue = 0;
        }

        // Calculate if the green LED should be on
        if ((counterLED == gDuty) && (gDuty != 255)) {
            gValue = 2;
        } else if (counterLED == 0) {
            gValue = 0;
        }

        // Calculate if the blue LED should be on
        if ((counterLED == bDuty) && (bDuty != 255)) {
            bValue = 4;
        } else if (counterLED == 0) {
            bValue = 0;
        }

        // Assign the port output values
        PORTB = rValue | gValue | bValue;
    }

    // UART receive
    if (PIR1bits.RCIF) {
        // Flash the LED for all data received
        LED = 0;

        if (RCSTAbits.FERR) {
            // Framing error, clear
            RCSTAbits.FERR = 0;
            RCSTAbits.SPEN = 0;
            RCSTAbits.SPEN = 1;
        } else if (RCSTAbits.OERR) {
            // Overrun error, clear
            RCSTAbits.OERR = 0;
            RCSTAbits.CREN = 0;
            RCSTAbits.CREN = 1;
        } else {
            // Valid data received
            unsigned char data = RCREG;

            if (counterPacket == 0) {
                // This byte is the packet size]
                counterPacket = data;
                counterTotal = data;

                if ((counterTotal >= 8) && (((counterTotal - 2) % 3) == 0)) {
                    // Send the next board the new number of packets
                    while (TXSTAbits.TRMT == 0);
                    TXREG = data - 3;
                }
            } else if ((counterTotal - counterPacket) == 1) {
                // Set the duty cycle of the red LED
                rDuty = data;
            } else if ((counterTotal - counterPacket) == 2) {
                // Set the duty cycle of the green LED
                gDuty = data;
            } else if ((counterTotal - counterPacket) == 3) {
                // Set the duty cycle of the blue LED
                bDuty = data;
            } else {
                // Transmit the remainder of the bytes
                if ((counterTotal >= 8) && (((counterTotal - 2) % 3) == 0)) {
                    while (TXSTAbits.TRMT == 0);
                    TXREG = data;
                }
            }
            
            // Decrement the number of bytes remain in this packet
            counterPacket--;
        }

        // Turn the LED back off
        LED = 1;
    }
}
