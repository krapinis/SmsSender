

//Issue with Serial.begin not being defined corectly
//https://github.com/Microsoft/vscode-cpptools/issues/743

//Debugging the Arduino code
//https://blogs.msdn.microsoft.com/iotdev/2017/05/27/debug-your-arduino-code-with-visual-studio-code/

//this is real deal
//https://github.com/ARMmbed/Debugging-docs/blob/master/Docs/Debugging/vscode.md

#include <SoftwareSerial.h>
#include <HardwareSerial.h>
#include <Arduino.h>

//SIM800 TX is connected to Arduino D8
#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

class Door{
  public:
  int Pin;
  int State;
  Door(){};
  void setPin(int pin){
    this->Pin = pin;
    this->State = digitalRead(pin);
  }
  void setState(int state){
    this->State = state;
  }
};


//Instance of the door
Door door;


void setup() {

  //Setting up the door
  door.setPin(3);

  //Array of doors
  Door doors[1] = {door};  
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);

  //Print list of doors
  Serial.println("Array size is: " +(String)(sizeof(doors)/(sizeof(*doors))));

  //SETUP
  doorSetup(doors);
  
  Serial.println("Setup Complete!");
  Serial.println("Sending SMS...");
   
  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  //This should be a real phone number
  serialSIM800.write("AT+CMGS=\"+xxxxxxxxxx\"\r\n");
  delay(1000);
   
  //Send SMS content
  serialSIM800.write("TEST");
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  Serial.println("SMS Sent!");
}
 
void loop() {
  

  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  if(serialSIM800.available()){
    Serial.write(serialSIM800.read());
  }
  //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
  if(Serial.available()){  
    serialSIM800.write(Serial.read());
  }
}

void doorSetup(Door doors[]){

  int size = (sizeof(doors)/(sizeof(*doors)));
  
  //Checking the status of all doors
    for(int i = 0; i < size; i++){
      Serial.println("----------------Door------------");
      Serial.println("Door PIN: " + (String)door.Pin);
      Serial.println("Door State: " + (String)door.State);
    }
  Serial.println("-----------------------------------------------------------------");
}

