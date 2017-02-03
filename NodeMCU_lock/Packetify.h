#include <SoftwareSerial.h>

SoftwareSerial BT(D7, D8);

void clearArray(char *arr, const short arrLength) {
    for (int i = 0; i < arrLength; i++) {
        arr[i] = 0;
    }
}

class PotatoClient {
private: 
    char recvPacket[257];
    char header[255];
    char body[255];

    short RECV_STATUS = 0;
    short STARTED = 0;
    char recvIndex = 0;

    short headerLength;
    short bodyLength;
    
    long startTime;
    long upkeep;
    
public:
    PotatoClient() {
        this->resetTimer();
    }

    long timePassed() {
        if (this->upkeep == 0) {
            long duration = millis() - this->startTime;
            return duration;
        } else {
            return this->upkeep;
        }
    }

    void writeChar(char character) {
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
        const char *header, String body,
        const short headerLength,
        const short bodyLength
        ) {
        this->sendPacket(header, body.c_str(), headerLength, bodyLength);
    }
    void sendPacket(
        String header, const char *body,
        const short headerLength,
        const short bodyLength
        ) {
        this->sendPacket(header.c_str(), body, headerLength, bodyLength);
    }
    void sendPacket(
        String header, String body,
        const short headerLength,
        const short bodyLength
        ) {
        this->sendPacket(header.c_str(), body.c_str(), headerLength, bodyLength);
    }

    void sendPacket(
        const char *header,
        const char *body,
        const short headerLength,
        const short bodyLength
        ) {

        const char totalLength = headerLength + bodyLength + 1;
        this->writeChar(totalLength);
        this->writeChar(headerLength);

        for (int i = 0; i < headerLength; i++) {
            this->writeChar(header[i]);
        }

        for (int i = 0; i < bodyLength; i++) {
            this->writeChar(body[i]);
        }
    }

    void sendPacketAuto(const char *header, String body) {
        Serial.println("START HEADC SBODY AUTOSEND");
        this->sendPacketAuto(header, body.c_str());
    }
    void sendPacketAuto(String header, const char *body) {
        Serial.println("AA");
        this->sendPacketAuto(header.c_str(), body);
    }
    void sendPacketAuto(String header, String body) {
        Serial.println("START HEADS SBODY AUTOSEND");
        this->sendPacketAuto(header.c_str(), body.c_str());
    }

    void sendPacketAuto(
        const char *header,
        const char *body
        ) {

        char headerLength = 0;
        char bodyLength = 0;

        Serial.println("PRE-LOOP");

        for (int i = 0; header[i] != 0; i++) { headerLength++; }
        for (int i = 0; body[i] != 0; i++) { bodyLength++; }

        Serial.print("HEADERLEN ");
        Serial.println(headerLength);
        Serial.print("BODYLEN ");
        Serial.println(bodyLength);

        const char totalLength = headerLength + bodyLength + 1;
        this->writeChar(totalLength);
        this->writeChar(headerLength);

        for (int i = 0; i < headerLength; i++) {
            this->writeChar(header[i]);
        }

        for (int i = 0; i < bodyLength; i++) {
            this->writeChar(body[i]);
        }
    }

    short acqPacketHeader(char *output) {
        for (int i = 0; i < recvPacket[1]; i++) {
            output[i] = recvPacket[2 + i];
        }

        return recvPacket[1];
    }

    short acqPacketBody(char *output) {
        const short packetLength = recvPacket[0] - recvPacket[1] - 1;
        const short bodyIndex = recvPacket[1] + 2;

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
        char errorHeader[] = "PACKET_TIMEOUT_DATA: ";
        sendPacket(errorHeader, recvPacket, 21, 128);
    }

    void cheak() {
        if (timePassed() > 1500 and RECV_STATUS == 1) {
            char currentIndex[1];
            currentIndex[0] = recvIndex;

            char errorHeader[] = "PACKET_TIMEOUT";
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

    short acqRecvStatus() { return this->RECV_STATUS; }
    short acqHeaderLength() { return this->headerLength; }
    short acqBodyLength() { return this->bodyLength; }
    char *acqRecvPacket() { return this->recvPacket; }
    char *acqHeader() { return this->header; }
    char *acqBody() { return this->body; }
};


