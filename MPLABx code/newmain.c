/*
 * File:   newmain.c
 * Author: lenovo
 *
 * Created on 9 August, 2016, 3:51 PM
 */

#include <xc.h>
#include "delays.h"
#include "string_cmp.h"
#include "uart.h"
#include "Timer.h"
#include "packetify.h"
#include "EEPROM.h"

void turnDeg(const int degree) {
    const long delay = 900 + (degree/180.0)*1200;
    PORTBbits.RB0 = 1;
    delay_us(delay);
    PORTBbits.RB0 = 0;
    delay_ms(10);
}

void changePassword(unsigned char *currentPassword, unsigned char *newPassword) {
    unsigned char password[8];
    readEEPROM(password);
    
    //unsigned char passwordMatch = 1;
    //unsigned char pseudochar;
    
    /*
    int i = 0;
    for (i=0; i<8; i++) {
        pseudochar = password[i];
        if (pseudochar == 0xff) {
            pseudochar = 0;
        }
        
        if (currentPassword[i] != pseudochar) {
            passwordMatch = 0;
            break;
        } else if (currentPassword[i] == '\0') {
            break;
        }
    }
    */
    
    if (cmpEEPROML(currentPassword,password,8) == 0) {
        //write_data_nvm(newPassword,0x00);
        writeEEPROM(newPassword);
        readEEPROM(password);        
                
        //writeEEPROM(newPassword);
        sendPacketAuto("CHANGE_PASSWORD_STATUS","1");
        //sendPacketAuto("CHANGE_PASSWORD_SET",newPassword);
        //sendPacketAuto("NOW_PASSWORD",password);
        
    } else {
        sendPacketAuto("CHANGE_PASSWORD_STATUS","0");
        //sendPacketAuto("PASSWORD_NOW",password);
        //sendPacketAuto("PASSWORD_SENT",currentPassword);
        //sendPacket("PASSCHAR_FAIL",&i,13,1);
    }
}

unsigned short LOCKED = 0;
unsigned int light;
const unsigned int LOCK_DEGREE = 0; 
const unsigned int UNLOCK_DEGREE = 150; 
//const unsigned int ENDLOCK_DEGREE = 150; 

void main(void) {
    TRISD = 0b00000000;
    PORTD = 0b00000101;
    
    ADCON0bits.ADON = 0;
    ADCON1 = 0x07;
    //CMCON = 0x07;
    TRISA = 0x00;
    TRISB = 0x00;
    TRISD = 0x00;
    
    /* To set up USART */
    RCSTAbits.SPEN = 1;
    PORTCbits.RC7 = 1; //input coming from Rx pin
    PORTCbits.RC0 = 0; //output meaning write to Tx pin
    //TXSTA = 0b01101110; //configurations for transmit register
    //RCSTA = 0b00110000; //configurations for receive register
    
    //MAD debug
    PORTAbits.RA3 = 0;
    PORTAbits.RA4 = 1;
    delay_ms(1000);
    PORTAbits.RA3 = 1;
    PORTAbits.RA4 = 0;
    
    for (int k = 0; k < 100; k++) { turnDeg(LOCK_DEGREE); }
    //turnDeg(ENDLOCK_DEGREE);
    LOCKED = 1;
    PORTAbits.RA1 = 0;
    PORTAbits.RA0 = 1;
    
    packetifyInit();
    //UART_Write_Text("AT+PIN6543");
    //UART_Write_Text("AT+PIN6543\r\n");
    //UART_Write_Text("AT+PIN654321\r\n");
    
    //fillEEPROM("\0\0\0\0\0\0\0\0");
    //char AtCommand[27];
    //char eeData[8];
    //readEEPROM(eeData);
    //sendPacket("EE",eeData,2,8);

    while (1) {
        processPacket();
        
        if (acqRecvStatus() == 2) {
            //sendPacketAuto("test", "blob");
            //sendPacketAuto("header_is", acqHeader());
            //sendPacketAuto("body_is", acqBody());
            if (strcmp(acqHeader(), "heartbeat") == 0) {
                sendPacketAuto("heartbeat", "1");
                
            } else if (strcmp(acqHeader(), "VERS") == 0) {
                sendPacketAuto("version: ", "v1.15");
                
            } else if (strcmp(acqHeader(), "WAIT") == 0) {
                PORTAbits.RA0 = 1;
                delay_ms(1000);
                PORTAbits.RA0 = 0;
                
            } else if (strcmp(acqHeader(), "STATUS") == 0) {
                char isLocked[1];
                isLocked[0] = LOCKED+1;
                sendPacketAuto("STATUS_RETURN",isLocked);
                
            } else if (strcmp(acqHeader(), "CONFIG") == 0) {
                char bLengthArray[1];
                bLengthArray[0] = acqBodyLength();
                sendPacketAuto("CONFIG_LENGTH", bLengthArray);
                for (int i = 0; i < acqBodyLength(); i++) {
                    UART_Write(acqBody()[i]);
                }

                //UART_Write_Text("\r\n");
                //sendPacketAuto("COMMAND", acqBody());
                
            } else if (strcmp(acqHeader(), "UNLOCK") == 0) {
                char password[8];
                readEEPROM(password);
                        
                if (cmpEEPROM(acqBody(), password) == 0) {
                    delay_ms(100);
                    //PORTAbits.RA0 = 1;
                    LOCKED = 0;
                    PORTAbits.RA1 = 1;
                    PORTAbits.RA0 = 0;
                    
                    volatile int k;
                    for (k = 0; k < 100; k++) { turnDeg(UNLOCK_DEGREE); }
                    //delay_ms(1000);
                    
                    
                    sendPacketAuto("unlocked", "1");
                } else {
                    sendPacketAuto("UNLOCK_FAIL", "1");
                }
                
            } else if (strcmp(acqHeader(), "LOCK") == 0) {
                char password[8];
                readEEPROM(password);
                        
                if (cmpEEPROM(acqBody(), password) == 0) {
                    delay_ms(100);
                    //PORTAbits.RA0 = 1;
                    volatile int k;
                    for (k = 0; k < 100; k++) { turnDeg(LOCK_DEGREE); }
                    //turnDeg(ENDLOCK_DEGREE);
                    
                    //delay_ms(1000);
                    
                    LOCKED = 1;
                    PORTAbits.RA1 = 0;
                    PORTAbits.RA0 = 1;
                    sendPacketAuto("locked", "1");
                    
                } else {
                    sendPacketAuto("LOCK_FAIL", "1");
                }
                
            } else if (strcmp(acqHeader(), "CHANGE_PASSWORD") == 0) {
                changePassword(acqBody(), acqBody()+8);
                    
            } else if (strcmp(acqHeader(), "EE") == 0) {
                char password[8];
                readEEPROM(password);
                sendPacket("current password: ",password,18,8);
                
            } else if (strcmp(acqHeader(), "EERESET") == 0) {
                //resetEEPROM(0x00);
                
                /*
                unsigned char password[8];
                //fillEEPROM(password);
                for (unsigned int i=0; i<8; i++) {
                    password[i] = readEEPROM(i); 
                }
                sendPacketAuto("reset password: ",password);
                */
                unsigned char blank[8];
                for (int i=0; i<8; i++) {blank[i] = 0;}
                writeEEPROM(blank);
                
                char password[8];
                readEEPROM(password);
                sendPacket("reset password: ",password,16,8);
            
            } else if (strcmp(acqHeader(), "LIGHT") == 0) {
                light = !light;
                PORTAbits.RA0 = light;
                sendPacketAuto("LIGHED", "1");
                
            } else if (strcmp(acqHeader(), "BUZZ") == 0) {
                PORTDbits.RD5 = 1;
                sendPacketAuto("BUZZED","1");
            } else if (strcmp(acqHeader(), "UNBUZZ") == 0) {
                PORTDbits.RD5 = 0;
                sendPacketAuto("UNBUZZED","1");
                
            } else {
                sendPacketAuto("WRONG HEADER ", acqHeader());
                sendPacketAuto("WRONG BODY ", acqBody());
            }
            
            /*
            sendPacket(
                acqHeader(),acqBody()
                ,acqHeaderLength(),acqBodyLength()
                );
            */
            
            resetRecv();
        }
    }
    
    return;
}
