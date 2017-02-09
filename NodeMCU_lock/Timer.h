#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

class Timer {
private:
    long startTime;
    long upkeep;
    
public:
    long timePassed() {
        if (this->upkeep == 0) {
            long duration = millis() - this->startTime;
            if (duration < 0) {
                this->resetTimer();
                return 0; 
            }
            
            return duration;
        
        } else {
            return this->upkeep;
        }
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
};

#endif


