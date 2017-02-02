#include <SoftwareSerial.h>

// creates a "virtual" serial port/UART
// connect BT module TX to D13
// connect BT module RX to D15
// connect BT Vcc to 5V, GND to GND

SoftwareSerial BT(D7, D8);

class PotatoClient {
private:  
    String data = "";
    String header = "";
    String body = "";
    
public:
    void cheak() {
       while (BT.available()) {
            // if text arrived in from BT serial...
            Serial.print(char(BT.read()));
            BT.print("h1");
            data = "";

            /*
            while (data.length() > data[0]) {
                BT.print("data-raw: ");
                BT.println(data);
                
                header = data.substring(1, data[1]);
                body = data.substring(data[1], data[0]);
                data = data.substring(data[0], data.length());
                
                BT.print("header: ");
                BT.println(header);

                BT.print("body: ");
                BT.println(body);

                BT.print("data-left: ");
                BT.println(data);
            }
            */
        }
    }
};

PotatoClient BTClient;

void setup() {
  	// set digital pin to control as an output
	//pinMode(13, OUTPUT);
	// set the data rate for the SoftwareSerial port
    Serial.begin(115200);
    
	BT.begin(9600);
	// Send test message to other device
	BT.print("Hello from Arduino");
}


void loop() {
    BTClient.cheak();
    delay(100);
}


