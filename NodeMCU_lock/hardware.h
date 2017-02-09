#include <Servo.h>

Servo myservo;

class Hardware {
private:
    bool LOCKED = false;
    
public:
    Hardware () {
        myservo.attach(D0);
        this->lock();
    }

    bool getLockStatus() {
        return this->LOCKED;    
    }

    void setLock() {
        int degree = myservo.read();
        
        if (LOCKED == false) {
            degree--;
        } else {
            degree++;
        }

        if (degree < 0) { degree = 0; }
        else if (degree > 180) { degree = 180; }

        myservo.write(degree);
        delay(15);
    }
    
    void lock() {       
        this->LOCKED = true;
    }

    void unlock() {
        this->LOCKED = false;
    }
};


