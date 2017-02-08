#ifndef PASSWORD_H_INCLUDED
#define PASSWORD_H_INCLUDED

#include <EEPROM.h>

class Password {
public:
    static void start (unsigned short memsize) {
        Serial.println("START EEPROM");
        EEPROM.begin(memsize);
    }

    static void start () {
        Serial.println("START EEPROM");
        EEPROM.begin(9);
    }

    static void readEEPROM(char data[]) {
        Serial.println("READ EEPROM: ");
        
        for (int i = 0; i < 8; i++) {
            data[i] = EEPROM.read(i);
            Serial.print("EE-READ: ");
            Serial.println(EEPROM.read(i));
        }
        
        Serial.println();
    }
    
    static void writeEEPROM(const char _data[]) {
        Serial.println("EEPROM write");
        
        for (int i = 0; i < 8; i++) {
             Serial.print("EE-");
             Serial.print(String(int(i)));
             Serial.print("-");
             Serial.println(String(int(_data[i])));
             
             EEPROM.write(i, (byte) _data[i]);
             if (_data[i] == '\0') {break;};
        }

        EEPROM.commit();
        delay(20);
    
        for (int i = 0; i < 8; i++) {
            Serial.print("REE-");
            Serial.println((byte) EEPROM.read(i));
        }
    }
    
    static char cmpEEPROM(const char input[], const char check[]) {
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
    
    static char cmpEEPROML(
        const char input[], const char check[], int length
        ) {
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
    
    static bool changePassword(
        const char *currentPassword, const char *newPassword
        ) {
        char password[8];
        readEEPROM(password);
        
        if (Password::cmpEEPROML(currentPassword, password, 8) == 0) {
            Serial.print("NEW PASSWORD: ");
            Serial.println(newPassword);
            
            Password::writeEEPROM(newPassword);
            Password::readEEPROM(password);     
            Serial.println("READ CPASSWORD: ");
            Serial.print(password);
            Serial.println("");
            return true;
            
        } else {
            return false;
        }
    }
};

#endif

