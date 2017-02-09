#include <SoftwareSerial.h>

#include "Packetify.h"
#include "hardware.h"
#include "password.h"
#include "misc.h"

// creates a "virtual" serial port/UART
// connect BT module TX to D13
// connect BT module RX to D15
// connect BT Vcc to 5V, GND to GND

PotatoClient BTClient;
Hardware hardware;

int TICKS = 0;

void setup() {
  	// set digital pin to control as an output
	//pinMode(13, OUTPUT);
	// set the data rate for the SoftwareSerial port
    Serial.begin(115200);
    Serial.flush();
    
	BT.begin(9600);
    BT.flush();
	// Send test message to other device

    Password::start();
}

void loop() {
    BTClient.cheak();
    
    delay(10);
    hardware.setLock();
    delay(10);

    if (BTClient.acqRecvStatus() == 2) {
        //sendPacketAuto("test", "blob");
        //sendPacketAuto("header_is", acqHeader());
        //sendPacketAuto("body_is", acqBody());
        const char *header = BTClient.acqHeader();
        const char *body = BTClient.acqBody();
        
        if (strcmp(header, "heartbeat") == 0) {
            BTClient.sendPacketAuto("heartbeat", "1");
            
        } else if (strcmp(header, "VERS") == 0) {
            BTClient.sendPacketAuto("version: ", "v1.15");
            
        } else if (strcmp(header, "WAIT") == 0) {
            //PORTAbits.RA0 = 1;
            Serial.print("RA0 is ON");
            delay(1000);
            Serial.print("RA0 is OFF");
            
        } else if (strcmp(header, "STATUS") == 0) {
            char isLocked[1];
            isLocked[0] = hardware.getLockStatus() + 1;
            BTClient.sendPacketAuto("STATUS_RETURN", isLocked);
            
        } else if (strcmp(header, "CONFIG") == 0) {
            char bLengthArray[1];
            bLengthArray[0] = BTClient.acqBodyLength();
            BTClient.sendPacketAuto("CONFIG_LENGTH", bLengthArray);
            
            for (int i = 0; i < BTClient.acqBodyLength(); i++) {
                BTClient.writeChar(BTClient.acqBody()[i]);
            }

            //UART_Write_Text("\r\n");
            //sendPacketAuto("COMMAND", acqBody());
            
        } else if (strcmp(header, "UNLOCK") == 0) {
            char password[8];
            Password::readEEPROM(password);
                    
            if (Password::cmpEEPROM(body, password) == 0) {
                hardware.unlock();
                Serial.println("UNLOCK");
                
                //PORTAbits.RA0 = 1;
                /*
                LOCKED = 0;
                PORTAbits.RA1 = 1;
                PORTAbits.RA0 = 0;
                */

                BTClient.sendPacketAuto("unlocked", "1");
            } else {
                BTClient.sendPacketAuto("UNLOCK_FAIL", "1");
            }
            
        } else if (strcmp(header, "LOCK") == 0) {
            char password[8];
            Password::readEEPROM(password);
                    
            if (Password::cmpEEPROM(body, password) == 0) {
                hardware.lock();
                /*
                //PORTAbits.RA0 = 1;
                volatile int k;
                for (k = 0; k < 100; k++) { turnDeg(LOCK_DEGREE); }
                //turnDeg(ENDLOCK_DEGREE);
                
                //delay_ms(1000);
                
                LOCKED = 1;
                PORTAbits.RA1 = 0;
                PORTAbits.RA0 = 1;
                */
                
                Serial.println("LOCKED");
                BTClient.sendPacketAuto("locked", "1");
                
            } else {
                BTClient.sendPacketAuto("LOCK_FAIL", "1");
            }
            
        } else if (strcmp(header, "CHANGE_PASSWORD") == 0) {
            bool changed = Password::changePassword(BTClient.acqBody(), BTClient.acqBody()+8);
            if (changed == true) {
                BTClient.sendPacketAuto("CHANGE_PASSWORD_STATUS", "1");
            } else {
                BTClient.sendPacketAuto("CHANGE_PASSWORD_STATUS", "0");
            }
                
        } else if (strcmp(BTClient.acqHeader(), "EE") == 0) {
            char password[8];
            Password::readEEPROM(password);
            BTClient.sendPacket("current password: ",password,18,8);
            
        } else if (strcmp(header, "EERESET") == 0) {
            //resetEEPROM(0x00);
            
            /*
            char password[8];
            //fillEEPROM(password);
            for (int i=0; i<8; i++) {
                password[i] = readEEPROM(i); 
            }
            sendPacketAuto("reset password: ",password);
            */
            
            char blank[8];
            for (int i=0; i<8; i++) {blank[i] = 0;}
            Password::writeEEPROM(blank);
            
            char password[8];
            Password::readEEPROM(password);
            BTClient.sendPacket("reset password: ",password,16,8);
        
        } else if (strcmp(header, "LIGHT") == 0) {
            //light = !light;
            //PORTAbits.RA0 = light;
            BTClient.sendPacketAuto("LIGHED", "1");
            
        } else if (strcmp(header, "BUZZ") == 0) {
            //PORTDbits.RD5 = 1;
            BTClient.sendPacketAuto("BUZZED","1");
            
        } else if (strcmp(header, "UNBUZZ") == 0) {
            //PORTDbits.RD5 = 0;
            BTClient.sendPacketAuto("UNBUZZED","1");
            
        } else {
            BTClient.sendPacketAuto("WRONG HEADER ", BTClient.acqHeader());
            BTClient.sendPacketAuto("WRONG BODY ", BTClient.acqBody());
        }

        Serial.println("RESET RECV");
        BTClient.resetRecv();
    }
    
    delay(50);

    TICKS++;
    if (TICKS % 100 == 0) {
        TICKS = 0;
        if (BTClient.available()) {
            Serial.print("B");
        } else {
            Serial.print("!");
        }
    }
}



