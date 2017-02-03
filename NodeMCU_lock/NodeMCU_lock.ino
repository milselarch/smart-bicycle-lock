#include <SoftwareSerial.h>

// creates a "virtual" serial port/UART
// connect BT module TX to D13
// connect BT module RX to D15
// connect BT Vcc to 5V, GND to GND

SoftwareSerial BT(D7, D8);

void clearArray(unsigned char *arr, const unsigned short arrLength) {
    for (int i = 0; i < arrLength; i++) {
        arr[i] = 0;
    }
}

class PotatoClient {
private: 
    unsigned char recvPacket[257];
    unsigned char header[255];
    unsigned char body[255];

    unsigned short RECV_STATUS = 0;
    unsigned short STARTED = 0;
    unsigned char recvIndex = 0;

    unsigned short headerLength;
    unsigned short bodyLength;
    
    unsigned long startTime;
    unsigned long upkeep;
    
public:
    PotatoClient() {
        this->resetTimer();
    }

    unsigned long timePassed() {
        if (this->upkeep == 0) {
            unsigned long duration = millis() - this->startTime;
            return duration;
        } else {
            return this->upkeep;
        }
    }

    void writeChar(unsigned char character) {
        /*
        BT.print(String(char(character)));
        Serial.print(String(char(character)));

        Serial.write NOT Serial.print
        print will read the char as a number and send the characters for each digit
        write will send the character whose VALUE equals what the char's value is
        */
        Serial.write(character);
        BT.write(character);
        BT.flush();
    }
    
    void resetTimer() {
        this->startTime = millis();
        this->upkeep = 0;
    }

    void pauseTimer() {
        this->upkeep = this->timePassed();
        if (this->upkeep == 0) { this->upkeep = 1; }
    }

    void startTimer() {
        if (this->upkeep != 0) {
            this->startTime = millis() - this->upkeep;
            this->upkeep = 0;
        }
    }

    
    void sendPacket(
        const unsigned char *header, String body,
        const unsigned short headerLength,
        const unsigned short bodyLength
        ) {
        this->sendPacket(header, body.c_str(), headerLength, bodyLength);
    }
    void sendPacket(
        String header, const unsigned char *body,
        const unsigned short headerLength,
        const unsigned short bodyLength
        ) {
        this->sendPacket(header.c_str(), body, headerLength, bodyLength);
    }
    void sendPacket(
        String header, String body,
        const unsigned short headerLength,
        const unsigned short bodyLength
        ) {
        this->sendPacket(header.c_str(), body.c_str(), headerLength, bodyLength);
    }

    void sendPacket(
        const unsigned char *header,
        const unsigned char *body,
        const unsigned short headerLength,
        const unsigned short bodyLength
        ) {

        const unsigned char totalLength = headerLength + bodyLength + 1;
        this->writeChar(totalLength);
        this->writeChar(headerLength);

        for (int i = 0; i < headerLength; i++) {
            this->writeChar(header[i]);
        }

        for (int i = 0; i < bodyLength; i++) {
            this->writeChar(body[i]);
        }
    }

    void sendPacketAuto(const unsigned char *header, String body) {
        Serial.println("START HEADC SBODY AUTOSEND");
        const char *cbody = body.c_str();
        this->sendUcharPacketAuto((char*) header, (char*) cbody);
    }
    void sendPacketAuto(String header, const unsigned char *body) {
        Serial.println("AA");
        const char *cheader = header.c_str();
        this->sendUcharPacketAuto((char *) cheader, (char*) body);
    }
    void sendPacketAuto(String header, String body) {
        Serial.println("START HEADS SBODY AUTOSEND");
        const char *cheader = header.c_str();
        const char *cbody = body.c_str();
        this->sendUcharPacketAuto(cheader, cbody);
    }

    void sendUcharPacketAuto(
        const unsigned char *header,
        const unsigned char *body
        ) {

        unsigned char headerLength = 0;
        unsigned char bodyLength = 0;

        Serial.println("PRE-LOOP");

        for (int i = 0; header[i] != 0; i++) { headerLength++; }
        for (int i = 0; body[i] != 0; i++) { bodyLength++; }

        Serial.print("HEADERLEN ");
        Serial.println(headerLength);
        Serial.print("BODYLEN ");
        Serial.println(bodyLength);

        const unsigned char totalLength = headerLength + bodyLength + 1;
        this->writeChar(totalLength);
        this->writeChar(headerLength);

        for (int i = 0; i < headerLength; i++) {
            this->writeChar(header[i]);
        }

        for (int i = 0; i < bodyLength; i++) {
            this->writeChar(body[i]);
        }
    }

    void sendUcharPacketAuto(
        const char *header,
        const char *body
        ) {

        unsigned char headerLength = 0;
        unsigned char bodyLength = 0;

        Serial.println("PRE-LOOP");

        for (int i = 0; header[i] != 0; i++) { headerLength++; }
        for (int i = 0; body[i] != 0; i++) { bodyLength++; }

        Serial.print("HEADERLEN ");
        Serial.println(headerLength);
        Serial.print("BODYLEN ");
        Serial.println(bodyLength);

        const unsigned char totalLength = headerLength + bodyLength + 1;
        this->writeChar(totalLength);
        this->writeChar(headerLength);

        for (int i = 0; i < headerLength; i++) {
            this->writeChar(header[i]);
        }

        for (int i = 0; i < bodyLength; i++) {
            this->writeChar(body[i]);
        }
    }

    void sendPacketAuto(
        const char *header,
        const char *body
        ) {

        unsigned char headerLength = 0;
        unsigned char bodyLength = 0;

        Serial.println("PRE-LOOP");

        for (int i = 0; header[i] != 0; i++) { headerLength++; }
        for (int i = 0; body[i] != 0; i++) { bodyLength++; }

        Serial.print("HEADERLEN ");
        Serial.println(headerLength);
        Serial.print("BODYLEN ");
        Serial.println(bodyLength);

        const unsigned char totalLength = headerLength + bodyLength + 1;
        this->writeChar(totalLength);
        this->writeChar(headerLength);

        for (int i = 0; i < headerLength; i++) {
            this->writeChar(header[i]);
        }

        for (int i = 0; i < bodyLength; i++) {
            this->writeChar(body[i]);
        }
    }

    unsigned short acqPacketHeader(unsigned char *output) {
        for (int i = 0; i < recvPacket[1]; i++) {
            output[i] = recvPacket[2 + i];
        }

        return recvPacket[1];
    }

    unsigned short acqPacketBody(unsigned char *output) {
        const unsigned short packetLength = recvPacket[0] - recvPacket[1] - 1;
        const unsigned short bodyIndex = recvPacket[1] + 2;

        for (int i = 0; i < packetLength; i++) {
            output[i] = recvPacket[bodyIndex + i];
        }

        return packetLength;
    }

    void wipeRecvPacket() {
        for (int i = 0; i < 257; i++) {
            recvPacket[i] = 0;
        }
    }

    void sendRecvPacketError() {
        unsigned char errorHeader[] = "PACKET_TIMEOUT_DATA: ";
        sendPacket(errorHeader, recvPacket, 21, 128);
    }

    void cheak() {
        if (timePassed() > 1500 and RECV_STATUS == 1) {
            unsigned char currentIndex[1];
            currentIndex[0] = recvIndex;

            unsigned char errorHeader[] = "PACKET_TIMEOUT";
            sendPacket(errorHeader, currentIndex, 14, 1);
            sendRecvPacketError();

            RECV_STATUS = 0;
            recvIndex = 0;
            resetTimer();
        }

        //Serial.print("BT AVA: ");
        //Serial.println(BT.available());
        //Serial.print("RECV STATUS: ");
        //Serial.println(RECV_STATUS);

        while (BT.available() and RECV_STATUS != 2) {
            // if text arrived in from BT serial...
            Serial.print("!");
            
            if (RECV_STATUS == 0) {
                clearArray(header, 255);    
                headerLength = 0;
                clearArray(body, 255);
                bodyLength = 0;

                wipeRecvPacket();
                this->resetTimer();
                this->startTimer();

                recvPacket[0] = BT.read();
                Serial.println(String(char(recvPacket[recvIndex])));

                recvIndex = 1;
                RECV_STATUS = 1;
                
            } else if (RECV_STATUS == 1) {
                recvPacket[recvIndex] = BT.read();
                Serial.println(String(char(recvPacket[recvIndex])));
                recvIndex += 1;

                Serial.print("INDEX-CHEAK ");
                Serial.print(recvIndex);
                Serial.print(" ");
                Serial.write(recvPacket[0]);
                Serial.println(recvPacket[0]);
                
                if (recvIndex > recvPacket[0]) {
                    headerLength = acqPacketHeader(header);
                    bodyLength = acqPacketBody(body);
                    wipeRecvPacket();

                    pauseTimer();
                    RECV_STATUS = 2;
                    Serial.print("RECV_COMPLETE");
                
                } else {
                    resetTimer();
                }
            }
        }
    }

    void resetRecv() {
        if (RECV_STATUS == 2) {
            RECV_STATUS = 0;
            Serial.println("RESET RECV STATUS");
            resetTimer();
        }    
    }

    unsigned short acqRecvStatus() { return this->RECV_STATUS; }
    short acqHeaderLength() { return this->headerLength; }
    short acqBodyLength() { return this->bodyLength; }
    unsigned char *acqRecvPacket() { return this->recvPacket; }
    unsigned char *acqHeader() { return this->header; }
    unsigned char *acqBody() { return this->body; }
};


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

String charArrayToString(unsigned char *charArray) {
    String answer = "";
    for (int i = 0; charArray[i] != '\0'; i++) {
        answer += charArray[i];
    }

    return answer;
}

int cmp(unsigned char input[], char check[]) {
    for (int i = 0;; i++) {
        if (input[i] == '\0' && check[i] == '\0') { break; }
        else if (input[i] == '\0' && check[i] != '\0') { return 1; }
        else if (input[i] != '\0' && check[i] == '\0') { return -1; }
        else if (input[i] > check[i]) { return 1; }
        else if (input[i] < check[i]) { return -1; }
        else {}
    }
    
    return 0;
}

int cmp(unsigned char input[], String check) {
    return cmp(input, check.c_str());
}

unsigned char password[8];

void readEEPROM(unsigned char *data) {
    data = password;
}

void writeEEPROM(unsigned char _data[]) {
    for (unsigned char i=0; i<8; i++) {
        password[i] = _data[i];
        
        //ee_write_byte(i, _data+i);;
        //PORTAbits.RA0 = !PORTAbits.RA0;
        delay(20);
        if (_data[i] == '\0') {break;};
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

unsigned short LOCKED = 0;

void loop() {
    BTClient.cheak();

    if (BTClient.acqRecvStatus() == 2) {
        //sendPacketAuto("test", "blob");
        //sendPacketAuto("header_is", acqHeader());
        //sendPacketAuto("body_is", acqBody());
        if (cmp(BTClient.acqHeader(), "heartbeat") == 0) {
            BTClient.sendPacketAuto("heartbeat", "1");
            
        } else if (cmp(BTClient.acqHeader(), "VERS") == 0) {
            BTClient.sendPacketAuto("version: ", "v1.15");
            
        } else if (cmp(BTClient.acqHeader(), "WAIT") == 0) {
            //PORTAbits.RA0 = 1;
            Serial.print("RA0 is ON");
            delay(1000);
            Serial.print("RA0 is OFF");
            
        } else if (cmp(BTClient.acqHeader(), "STATUS") == 0) {
            unsigned char isLocked[1];
            isLocked[0] = LOCKED + 1;
            BTClient.sendPacketAuto("STATUS_RETURN", isLocked);
            
        } else if (cmp(BTClient.acqHeader(), "CONFIG") == 0) {
            unsigned char bLengthArray[1];
            bLengthArray[0] = BTClient.acqBodyLength();
            BTClient.sendPacketAuto("CONFIG_LENGTH", bLengthArray);
            
            for (int i = 0; i < BTClient.acqBodyLength(); i++) {
                BTClient.writeChar(BTClient.acqBody()[i]);
            }

            //UART_Write_Text("\r\n");
            //sendPacketAuto("COMMAND", acqBody());
            
        } else if (cmp(BTClient.acqHeader(), "UNLOCK") == 0) {
            unsigned char password[8];
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
            
        } else if (cmp(BTClient.acqHeader(), "LOCK") == 0) {
            unsigned char password[8];
            readEEPROM(password);
                    
            if (cmpEEPROM(BTClient.acqBody(), password) == 0) {
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
            
        } else if (cmp(BTClient.acqHeader(), "CHANGE_PASSWORD") == 0) {
            changePassword(BTClient.acqBody(), BTClient.acqBody()+8);
                
        } else if (cmp(BTClient.acqHeader(), "EE") == 0) {
            unsigned char password[8];
            readEEPROM(password);
            BTClient.sendPacket("current password: ",password,18,8);
            
        } else if (cmp(BTClient.acqHeader(), "EERESET") == 0) {
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
            
            unsigned char password[8];
            readEEPROM(password);
            BTClient.sendPacket("reset password: ",password,16,8);
        
        } else if (cmp(BTClient.acqHeader(), "LIGHT") == 0) {
            //light = !light;
            //PORTAbits.RA0 = light;
            BTClient.sendPacketAuto("LIGHED", "1");
            
        } else if (cmp(BTClient.acqHeader(), "BUZZ") == 0) {
            //PORTDbits.RD5 = 1;
            BTClient.sendPacketAuto("BUZZED","1");
            
        } else if (cmp(BTClient.acqHeader(), "UNBUZZ") == 0) {
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


