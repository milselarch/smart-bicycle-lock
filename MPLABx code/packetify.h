#ifndef PACKETIFY_H_INCLUDED
#define PACKETIFY_H_INCLUDED

#include <p18f4550.h>
#include "uart.h"
#include "string_cmp.h"
#include "Timer.h"
#include <xc.h>

#define _XTAL_FREQ 48000000

unsigned short RECV_STATUS = 0; 
unsigned short STARTED = 0; 
char recvIndex = 0;
short headerLength;
short bodyLength;

unsigned char recvPacket[257];
unsigned char header[252];
unsigned char body[250];

unsigned short acqRecvStatus() { return RECV_STATUS; }
short acqHeaderLength() { return headerLength; }
short acqBodyLength() { return bodyLength; }
char *acqRecvPacket() { return recvPacket; }
char *acqHeader() { return header; }
char *acqBody() { return body; }

void resetRecv() { 
    if (RECV_STATUS == 2) { 
        RECV_STATUS = 0; 
        resetTimer();
    } 
}

void packetifyInit() {
    UART_Init(1200);
    initTimer();
    /*
    INTCONbits.GIE = 1; 
	INTCONbits.INT0IE = 1;
    T0CON = 0b10000111; 
    T0CONbits.TMR0ON = 1;
    */
}

unsigned short int sendPacket(
    const char *header,
    const char *body,
    const unsigned short headerLength,
    const unsigned short bodyLength 
    ) {
    
    const unsigned int totalLength = headerLength+bodyLength+1;
    UART_Write(totalLength);
    UART_Write(headerLength);
    
    for (int i = 0; i < headerLength; i++) {
        UART_Write(header[i]);
    }
    
    for (int i = 0; i < bodyLength; i++) {
        UART_Write(body[i]);
    }
}

void sendPacketAuto(
    const unsigned char *header,
    const unsigned char *body
    ) {
    
    unsigned short headerLength = 0;
    unsigned short bodyLength = 0;
    
    for (int i = 0; header[i] != '\0'; i++) {
        headerLength++;
    }
    
    for (int i = 0; body[i] != '\0'; i++) {
        bodyLength++;
    }
    
    const unsigned short totalLength = headerLength+bodyLength+1; 
    UART_Write(totalLength);
    UART_Write(headerLength);
    
    for (int i = 0; i < headerLength; i++) {
        UART_Write(header[i]);
    }
    
    for (int i = 0; i < bodyLength; i++) {
        UART_Write(body[i]);
    }
}

unsigned short acqPacketHeader(char *output) {
	for (int i = 0; i < recvPacket[1]; i++) {
		output[i] = recvPacket[2+i];
    }
    
    return recvPacket[1];
}

unsigned short acqPacketBody(char *output) {
    const unsigned short length = recvPacket[0]-recvPacket[1]-1;
    const unsigned short bodyIndex = recvPacket[1]+2;
    
	for (int i = 0; i < length; i++) {
		output[i] = recvPacket[bodyIndex+i];
    }
    
    return length;
}

void wipeRecvPacket() {
    for (int i = 0; i < 257; i++) {
        recvPacket[i] = '\0';
    }
    
}

void sendRecvPacketError() {
    sendPacket("PACKET_TIMEOUT_DATA: ",recvPacket,21,128);
    wipeRecvPacket();
}

void processPacket() {
    //cheakTimer();
    /*
    char num[1];
    num[0] = ((int) acqTimePassed)%256;
    sendPacketAuto("TIMER_", num);
    */
    
    if (UART_Data_Ready()) {
        PORTAbits.RA3 = 1;
    } else {
        PORTAbits.RA3 = 0;
    }
    
    if (TRMT) {
        PORTAbits.RA4 = 1;
    } else {
        PORTAbits.RA4 = 0;
    }
    
    if (acqCountsPassed() > 1500 && RECV_STATUS == 1) {
        char currentIndex[1];
        currentIndex[0] = recvIndex;
        sendPacket("PACKET_TIMEOUT", currentIndex, 14, 1);
        sendRecvPacketError();
        //PORTAbits.RA0 = !PORTAbits.RA0;
        
        RECV_STATUS = 0;
        recvIndex = 0;
        resetTimer();
    }
    
    if (UART_Data_Ready() && RECV_STATUS != 2) {
        if (RECV_STATUS == 0) {
            clearArray(header,255);
            headerLength = 0;
            clearArray(body,255);
            bodyLength = 0;
            
            wipeRecvPacket();
            resetTimer();
            startTimer();
        
            recvPacket[0] = UART_Read();
            
            recvIndex = 1;
            RECV_STATUS = 1; //receiving data

        } else if (RECV_STATUS == 1) {
            recvPacket[recvIndex] = UART_Read(); 
            recvIndex += 1;

            if (recvIndex > recvPacket[0]) {
                headerLength = acqPacketHeader(header);
                bodyLength = acqPacketBody(body);
                wipeRecvPacket();

                // packet acquired, waiting to be dealt with
                pauseTimer();
                RECV_STATUS = 2; 
            
            } else {
                resetTimer();
            }
        }
    }
}

#endif