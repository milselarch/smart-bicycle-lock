#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

unsigned long TIMER_COUNTS = 0;

void interrupt timer2(void)
{
    //if (LIGHT > 50) {PORTAbits.RA0 = 0; }
    //else {PORTAbits.RA0 = 1; }
    
    //PORTAbits.RA0 = LIGHT > 500 ? 1 : 0;
    TIMER_COUNTS++;
    TMR2IF = 0;
}

void initTimer() {
    //TRISA = 0x00;
    ADCON1 = 15;
   // initialize Timer2

    T2CON = 0x4D;
    PR2 = 249;
    TMR2IE = 1;
    PEIE = 1;
    TMR2ON = 1;
    TMR2IP = 1;
   // Turn on all interrupts
    GIE = 1;
}

void resetTimer() {
    TIMER_COUNTS = 0;
}

void startTimer() {
    T2CONbits.TMR2ON = 0;
}

void pauseTimer() {
    T2CONbits.TMR2ON = 0;
    TMR2IF = 0; 
}

unsigned long acqCountsPassed() {
    return TIMER_COUNTS;
}

/*
void initTimer() {
    T0CON = 0b10000111; 
    //T0CONbits.TMR0ON = 1;
    T0CONbits.TMR0ON = 1;
    TMR0H = 0xED;
    TMR0L = 0xAF;
}

void resetTimer() { TIMER_COUNTS = 0; }

void startTimer() { 
    T0CONbits.TMR0ON = 1;
    INTCONbits.TMR0IF = 1; 
}
void pauseTimer() { 
    T0CONbits.TMR0ON = 0;
    INTCONbits.TMR0IF = 0; 
}

double acqTimePassed() {
    return TIMER_COUNTS/10.0;
} 

void cheakTimer() {
    if (INTCONbits.TMR0IF == 0) {
        //INTCONbits.TMR0IF = 0;

    } else {
        TIMER_COUNTS++;
        INTCONbits.TMR0IF = 0;
        TMR0H = 0xED;
        TMR0L = 0xAF;
    }
}
*/

#endif