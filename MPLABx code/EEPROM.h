#ifndef EEPROM_H_INCLUDED
#define EEPROM_H_INCLUDED

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include "delays.h"

//http://innovatelogics.com/internal-eeprom-of-pic18f4550/

#pragma stack 0x300 0xff // set 64 byte stack at 0x300, needed by sdcc

char cmpEEPROM(unsigned char input[], unsigned char check[]) {
    for (unsigned int i = 0;; i++) {
        unsigned char inputChar = input[i];
        unsigned char checkChar = check[i];
        if (inputChar == 0xff) {inputChar = 0; }
        if (checkChar == 0xff) {checkChar = 0; }
        
        if (inputChar == '\0' && checkChar == '\0') { break; }
        else if (inputChar == '\0' && checkChar != '\0') { return 1; }
        else if (inputChar != '\0' && checkChar == '\0') { return -1; }
        else if (inputChar > checkChar) { return 1; }
        else if (inputChar < checkChar) { return -1; }
        else {}
    }
    return 0;
}

char cmpEEPROML(unsigned char input[], unsigned char check[], const unsigned int length) {
    for (unsigned int i = 0; i<length ; i++) {
        unsigned char inputChar = input[i];
        unsigned char checkChar = check[i];
        if (inputChar == 0xff) {inputChar = 0; }
        if (checkChar == 0xff) {checkChar = 0; }
        
        if (inputChar == '\0' && checkChar == '\0') { break; }
        else if (inputChar == '\0' && checkChar != '\0') { return 1; }
        else if (inputChar != '\0' && checkChar == '\0') { return -1; }
        else if (inputChar > checkChar) { return 1; }
        else if (inputChar < checkChar) { return -1; }
        else {}
    }
    return 0;
}

void ee_write(unsigned int address, unsigned char _data[]) {
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1; // enable writes to data EEPROM
    INTCONbits.GIE = 0;  // disable interrupts

    for (volatile int index = 0; _data[index] != '\0'; index++) {
        EEDATA = _data[index];
        EEADR = address;
        // start write sequence as described in datasheet, page 91
       
        EECON2 = 0x55;
        EECON2 = 0x0AA;
        EECON1bits.WR = 1;   // start writing
        while(EECON1bits.WR){Nop();}
        if(EECON1bits.WRERR){
            PORTAbits.RA1 = !PORTAbits.RA1;
            //printf("ERROR: writing to EEPROM failed!n");
        }
        
        address++;
    }
    
    EECON1bits.WREN = 0;
    INTCONbits.GIE = 1;  // enable interrupts
}

void ee_write_byte(unsigned char address, unsigned char *_data){
    //sendPacket("ADDR",&address,4,1);

    EEADR = address;
    EEDATA = *_data;
    Nop(); 
    Nop();
    
    // start write sequence as described in datasheet, page 91
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1; // enable writes to data EEPROM
    INTCONbits.GIE = 0;  // disable interrupts
    EECON2 = 0x55;
    EECON2 = 0x0AA;
    EECON1bits.WR = 1;   // start writing
    /*
    while(EECON1bits.WR){Nop();}
    if(EECON1bits.WRERR){
        printf("ERROR: writing to EEPROM failed!n");
    }
    */
    INTCONbits.GIE = 1;  // enable interrupts
    EECON1bits.WREN = 0;
}

void ee_write_char(unsigned char address, const unsigned char chunk){

    EEDATA = chunk;
    EEADR = address;
    // start write sequence as described in datasheet, page 91
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS = 0;
    EECON1bits.WREN = 1; // enable writes to data EEPROM
    INTCONbits.GIE = 0;  // disable interrupts
    EECON2 = 0x55;
    EECON2 = 0x0AA;
    EECON1bits.WR = 1;   // start writing
    while(EECON1bits.WR){Nop();}
    if(EECON1bits.WRERR){
        printf("ERROR: writing to EEPROM failed!n");
    }
    EECON1bits.WREN = 0;
    INTCONbits.GIE = 1;  // enable interrupts
}

void writeEEPROM(unsigned char _data[]) {
    for (unsigned volatile char i=0; i<8; i++) {
        ee_write_byte(i, _data+i);
        Nop();Nop();Nop();Nop();Nop();
        //PORTAbits.RA0 = !PORTAbits.RA0;
        delay_ms(20);
        if (_data[i] == '\0') {break;};
    }
}

void ee_read_byte(unsigned char address, unsigned char *_data){
    EEADR = address;
    EECON1bits.CFGS = 0;
    EECON1bits.EEPGD = 0;
    EECON1bits.RD = 1;
    *_data = EEDATA;
}

void readEEPROM(unsigned char _data[]) {
    for (int i=0;; i++) {
        ee_read_byte(i, _data+i);
        if (_data[i] == '\0' || _data[i] == 0xff) {break;}
    }
}

/*
void fillEEPROM(char *data) {
    for (int i = 0; i < 8; i++) {
        eeprom_write(i, data[i]);
    }
}

void readEEPROM(char *data) {
    for (int i = 0; i < 8; i++) {
        data[i] = eeprom_read(i);
    }
}

void clearEEPROM(char *data) {
    for (int i = 0; i < 8; i++) {
        eeprom_write(i, 0);
    }
}
*/
/*
void resetEEPROM(unsigned char address) {
    EECON1 = 0x04;
    for (int i=0; i<9; i++) {
        EEADR = address;
        EEDATA = 0;
        EECON2 = 0x55;
        delay_ms(50);
        EECON2 = 0xaa;
        delay_ms(50);
        WR = 1;
    //  Wait for the byte to be written in EEPROM
        while(EEIF == 0);
        EEIF = 0;
        address++;
    }
    
    EECON1 = 0x00;
}

void write_str_nvm(char *data, unsigned char address) {
    unsigned int x = 0;
//  Allowing write cycle to EEPROM
    EECON1 = 0x04;
    while (data[x] != 0) {
        EEADR = address;
        EEDATA = data[x];
        EECON2 = 0x55;
        delay_ms(50);
        EECON2 = 0xaa;
        delay_ms(50);
        WR = 1;
    //  Wait for the byte to be written in EEPROM
        while(EEIF == 0);
        EEIF = 0;
        address++;
        x++;
    }
    
    EEADR = address;
    EEDATA = 0; //set NULL
    EECON2 = 0x55;
    delay_ms(50);
    EECON2 = 0xaa;
    delay_ms(50);
    WR = 1;
//  Wait for the byte to be written in EEPROM
    while(EEIF == 0);
    EEIF = 0;
    
    //  Disabling write cycles to EEPROM
    EECON1 = 0x00;
}

//  Function for writing single byte to non-volatile memory
void write_data_nvm(char data, unsigned char address){
//  Enabling write cycles to EEPROM
    EECON1 = 0x04;
    EECON2 = 0x55;
    delay_ms(50);
    EECON2 = 0xaa;
    delay_ms(50);
    EEADR = address;
    EEDATA = data;
    WR = 1;
//  Wait for the byte to be written in EEPROM
    while(EEIF == 0);
    EEIF = 0;
//  Disabling write cycles to EEPROM
    EECON1 = 0x00;
}

char *readEEPROM(unsigned int addr) {
    EEADR = addr;
    RD    = 1;
    while (RD == 1);
    return EEDATA;
}
*/

#endif