//File Door.h

#include <Door.h>
#include <Arduino.h>
#include <Time.h>

namespace Door{
//Door constructor
class Door
{
    private:
        int pin;
        byte state;
        int smsLedPin;
        int doorStatusLedPin;
        char message[256] = "System has been armed\r\n";
        char* telephone = (char*)"+4591520714";
        char* smsMessage;
        time_t current_time;


    public:
        void setPin(int pin);
        void setState(int state)
            {
                if(state == 0){
                    this->state = 0;
                    strcat(this->message, "Door is open");
                }
                if(state == 1){
                    this->state = 1;
                    strcat(this->message, "Door is closed");
                }
            }
            
        void setTime(){
            this->current_time = now();
        }

        int getPin()
            {
                return pin;
            }
        //Parameterless constructor
        Door();
        //Paremeterized constructor
        Door(int pin)
            {
                setPin(pin);
            }
        //Destructor
        ~Door();
};

// Door member function

    void Door::setPin(int pin)
    {
        this->pin = pin;
    }
}