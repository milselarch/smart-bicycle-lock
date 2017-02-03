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

    void sendPacketAuto(
        const unsigned char *header,
        const unsigned char *body
        ) {

        unsigned char headerLength = 0;
        unsigned char bodyLength = 0;

        for (int i = 0; header[i] != 0; i++) { headerLength++; }
        for (int i = 0; body[i] != 0; i++) { bodyLength++; }

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

                recvPacket[0] = int(BT.read());
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
                
                if (recvIndex > int(recvPacket[0])) {
                    headerLength = acqPacketHeader(header);
                    bodyLength = acqPacketBody(body);
                    wipeRecvPacket();

                    pauseTimer();
                    RECV_STATUS = 2;
                
                } else {
                    resetTimer();
                }
            }
        }
    }

    void resetRecv() {
        if (RECV_STATUS == 2) {
            RECV_STATUS = 0;
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

void loop() {
    BTClient.cheak();

    if (BTClient.acqRecvStatus() == 2) {
        Serial.print(charArrayToString(BTClient.acqHeader()));
        Serial.print(charArrayToString(BTClient.acqBody()));
        
        unsigned char mssgHeader1[] = "header_is"; 
        BTClient.sendPacketAuto(mssgHeader1, BTClient.acqHeader());
        unsigned char mssgHeader2[] = "body_is"; 
        BTClient.sendPacketAuto(mssgHeader2, BTClient.acqBody());
    }
    
    delay(100);
}


