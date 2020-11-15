/*	Author: Heng Tan
 *  Partner(s) Name: 
 *	Lab Section: 024
 *	Assignment: Lab 8  Exercise 1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://youtu.be/lLbmww2DHio
 */
#include <avr/io.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

void set_PWM(double frequency){
    static double current_frequency;

    if(frequency != current_frequency){
        if(!frequency) {
            TCCR3B &= 0x08;
        } else {
            TCCR3B |= 0x03;
        }

        if(frequency < 0.954) OCR3A = 0xFFFF;
        else if (frequency > 31250) OCR3A = 0x0000;
        else OCR3A = (short)(8000000 / (128 * frequency))-1;

        TCNT3 = 0;
        current_frequency = frequency;
    }
}

void PWM_on(){
    TCCR3A = (1 << COM3A0);
    TCCR3B = (1 << WGM32) | (1 < CS31) | (1 << CS30);
    set_PWM(0);
}

void PWM_off(){
    TCCR3A = 0x00;
    TCCR3B = 0x00;
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    state = start;
    PWM_on();
    unsigned char tmp = 0;
    /* Insert your solution below */
    while (1) {
        tmp = (~PINA) & 0x07;
        if(tmp == 0x01){
            set_PWM(261.63);
        } else if(tmp == 0x02){
            set_PWM(293.66);
        } else if(tmp == 0x04){
            set_PWM(329.63);
        } else {
            set_PWM(0);
        }
    }
    return 1;
}
