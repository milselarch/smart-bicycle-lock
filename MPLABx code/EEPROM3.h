#ifndef EEPROM3_HEADER_INCLUDED
#define EEPROM3_HEADER_INCLUDED

void write_string(int address, char string[]) { 
      int ctr=0; 
      while (string[ctr]!=0){    
      eeprom_write(address++,string[ctr]);    
      ctr++; 
   } 
} 

void read_string(int address, unsigned char text[]) { 
    int i =0; 
   
    while (1) { 
       text[i] = eeprom_read(address++);
       if (text[i] == '\0') {break;}
       i++;
    }  
}

void writeEEPROM(unsigned char data[]) {
    for (int i = 0; i < 8; i++) {
        eeprom_write(i, data[i]);
    }
    
    eeprom_write(i, '\0');
}

void readEEPROM(unsigned char *data) {
    read_string(0x00, data);
}

void resetEEPROM(unsigned char *data) {
    char blank[8];
    for (int i=0; i<8; i++) {blank[i] = 0; }
    write_string(0x00,blank);
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