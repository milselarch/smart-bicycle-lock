#ifndef EEMPROM2_HEADER_INCLUDED
#define EEMPROM2_HEADER_INCLUDED

void writeEEPROM(unsigned char *data) {
    for (int i = 0; i < 8; i++) {
        eeprom_write(i, data[i]);
    }
    
    eeprom_write(i, '\0');
}

void readEEPROM(unsigned char *data) {
    for (int i = 0; i < 8; i++) {
        data[i] = eeprom_read(i);
    }
}

void resetEEPROM(unsigned char *data) {
    for (int i = 0; i < 8; i++) {
        eeprom_write(i, 0);
    }
}

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

#endif