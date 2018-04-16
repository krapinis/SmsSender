//File Door.h

#include <Door.h>

namespace Door{
//Door constructor
class Door
{
    private:
        int pin;

    public:
        void setPin(int pin);
        int getPin()
            {
                return pin;
            }
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