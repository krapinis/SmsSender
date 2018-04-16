#ifdef DOOR_H
#define DOOR_H

namespace Door{
        class Door
        {
            private:
                int m_pin;
            
            public:
                Door(int pin);
                void setPin(int pin);
                int getPin(){return m_pin;}
        }
}

#endif