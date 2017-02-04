#include <SoftwareSerial.h>
#include "Packetify.h"
#include "misc.h"

// creates a "virtual" serial port/UART
// connect BT module TX to D13
// connect BT module RX to D15
// connect BT Vcc to 5V, GND to GND

PotatoClient BTClient;

void setup() {
  	// set digital pin to control as an output
	//pinMode(13, OUTPUT);
	// set the data rate for the SoftwareSerial port
    Serial.begin(115200);
    Serial.flush();
    
	BT.begin(9600);
    BT.flush();
	// Send test message to other device
	BT.print("Hello from Arduino");
}

char password[8];

void readEEPROM(const char *data) {
    data = password;
}

void writeEEPROM(const char _data[]) {
    for (char i=0; i<8; i++) {
        password[i] = _data[i];
        
        //ee_write_byte(i, _data+i);;
        //PORTAbits.RA0 = !PORTAbits.RA0;
        delay(20);
        if (_data[i] == '\0') {break;};
    }
}

char cmpEEPROM(const char input[], const char check[]) {
    for (int i = 0;; i++) {
        char inputChar = input[i];
        char checkChar = check[i];
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

char cmpEEPROML(const char input[], const char check[], int length) {
    for (int i = 0; i<length ; i++) {
        char inputChar = input[i];
        char checkChar = check[i];
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

void changePassword(const char *currentPassword, const char *newPassword) {
    char password[8];
    readEEPROM(password);
    
    //char passwordMatch = 1;
    //char pseudochar;
    
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
        BTClient.sendPacketAuto("CHANGE_PASSWORD_STATUS","1");
        //sendPacketAuto("CHANGE_PASSWORD_SET",newPassword);
        //sendPacketAuto("NOW_PASSWORD",password);
        
    } else {
        BTClient.sendPacketAuto("CHANGE_PASSWORD_STATUS","0");
        //sendPacketAuto("PASSWORD_NOW",password);
        //sendPacketAuto("PASSWORD_SENT",currentPassword);
        //sendPacket("PASSCHAR_FAIL",&i,13,1);
    }
}

short LOCKED = 0;

void loop() {
    BTClient.cheak();

    if (BTClient.acqRecvStatus() == 2) {
        //sendPacketAuto("test", "blob");
        //sendPacketAuto("header_is", acqHeader());
        //sendPacketAuto("body_is", acqBody());
        const char *header = BTClient.acqHeader();
        
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
            isLocked[0] = LOCKED + 1;
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
            readEEPROM(password);
                    
            if (cmpEEPROM(BTClient.acqBody(), password) == 0) {
                delay(100);
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
            readEEPROM(password);
                    
            if (cmpEEPROM(header, password) == 0) {
                delay(100);

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
                Serial.print("LOCKED");
                BTClient.sendPacketAuto("locked", "1");
                
            } else {
                BTClient.sendPacketAuto("LOCK_FAIL", "1");
            }
            
        } else if (strcmp(header, "CHANGE_PASSWORD") == 0) {
            changePassword(BTClient.acqBody(), BTClient.acqBody()+8);
                
        } else if (strcmp(BTClient.acqHeader(), "EE") == 0) {
            char password[8];
            readEEPROM(password);
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
            writeEEPROM(blank);
            
            char password[8];
            readEEPROM(password);
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
            Serial.println("WRONG_DATA");
            Serial.print((char*) BTClient.acqHeader());
            Serial.println("POST HEADER ACQ");
            BTClient.sendPacketAuto("WRONG HEADER ", BTClient.acqHeader());
            Serial.print("WRONG HEAD SEND DONE");
            BTClient.sendPacketAuto("WRONG BODY ", BTClient.acqBody());
            Serial.print("WRONG BODY SEND DONE");
        }
        
        /*
        sendPacket(
            acqHeader(),acqBody()
            ,acqHeaderLength(),acqBodyLength()
            );
        */

        Serial.print("RESET RECV");
        BTClient.resetRecv();
    }
    
    delay(50);
}


