/*	Author: Heng Tan
 *  Partner(s) Name: 
 *	Lab Section: 024
 *	Assignment: Lab 8  Exercise 3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *  Demo Link: https://youtu.be/DfVLgiGVRRo
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
    TCCR1B = 0x0B;
    OCR1A = 125;
    TIMSK1 = 0x02;
    TCNT1 = 0;
    _avr_timer_cntcurr = _avr_timer_M;
    SREG |= 0x80;
}

void TimerOff(){
    TCCR1B = 0x00;
}

void TimerISR(){
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
    _avr_timer_cntcurr--;
    if(_avr_timer_cntcurr == 0){
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }
}

void TimerSet(unsigned long M){
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

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

enum States {start, wait, on} state;

unsigned char read = 0;
unsigned char OnOff = 0;
unsigned char count = 0;
unsigned char timer  = 0;
double table[20] = {261.63, 293.66, 329.63, 392.00, 392.00,
                    440.00, 392.00, 329.63, 261.63, 293.66,
                    329.63, 329.63, 293.66, 261.63, 293.66,
                    329.63, 329.63, 293.66, 261.63, 293.66};

void Tick(){
    read = (~PINA) & 0x01;
    switch(state){
        case start: PORTC = count = 0x00; state = wait; break;
        case wait: 
            if(read == 0x01){
                PWM_on();
                state = on;
            } else {
                PWM_off();
                state = wait;
            }
            break;
        case on: 
            if(timer == 1){
                timer = 0;
                count++;
                if(count == 24){
                    count = 0;
                    state = wait;
                }
            } else {
                timer++;
                state = on;
            }
            break;
        default: break;
    }

    switch(state){
        case start:break;
        case wait:break;
        case on: 
            set_PWM(table[count]);
            PORTC = count/5;
            break;
        default: break;
    }
}

int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xFF; PORTC = 0x00;
    state = start;
    
    TimerSet(100);
    TimerOn();
    /* Insert your solution below */
    while (1) {
        Tick();
        while(!TimerFlag){}
        TimerFlag = 0;
    }
    return 1;
}
