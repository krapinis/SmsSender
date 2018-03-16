//Issue with Serial.begin not being defined corectly
//https://github.com/Microsoft/vscode-cpptools/issues/743

//Debugging the Arduino code
//https://blogs.msdn.microsoft.com/iotdev/2017/05/27/debug-your-arduino-code-with-visual-studio-code/

//this is real deal
//https://github.com/ARMmbed/Debugging-docs/blob/master/Docs/Debugging/vscode.md


//#include <GSMSim.h> - this is the library in question

//https://playground.arduino.cc/Code/DateTime
#include <Time.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
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
  //State of the door is 1 open or 0 closed 
  int State;
  char Mode;
  time_t TimeStampOpen;
  time_t TimeStampClosed;
  //time since the door has been last opened
  //time since the door has benn last closed
  Door(){};
  void setPin(int pin){
    this->Pin = pin;
    this->State = digitalRead(pin);
    this->Mode = INPUT;
  }
  void setState(int state){
    this->State = state;
  }
  void doorOpenTime(){
    this->TimeStampOpen = now();
    Serial.println(TimeStampOpen);
  }
  void doorClosedTime(){
    this->TimeStampClosed = now();
    Serial.println(TimeStampClosed);
  }
};


//Instance of the door
Door door;


void setup() {
 //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
  
  //TIME default Setup
  //setTime(hours, minutes, seconds, days, months, years);
  setTime(15,59,3,16,3,18);
  time_t tempTime = now();
  if(timeStatus() == timeNotSet){
    Serial.println("Time SETUP is incorect");
  }else{
    Serial.print("Time has been set to this : ");
    printTime(tempTime);
  }
  
  //Setting up the door
  door.setPin(3);

  //Array of doors
  Door doors[1] = {door};  

  //Print list of doors
  Serial.println("Total doors in the system: " +(String)(sizeof(doors)/(sizeof(*doors))));

  //SETUP
  doorSetup(doors);

  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);
  delay(1000);
  
  Serial.println("Setup Complete!");
  Serial.println("Ready to send SMS's...");
  Serial.println("-----------------------------------------");
  
  sendSMS(door); 
 
}
 
void loop() {
  if (timeStatus() == timeNotSet){
    Serial.println("Time is not set");
  }

  //if (minute() == 0){
  //  Serial.println("hour just passed");
  //}

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

  int count = (sizeof(doors)/(sizeof(Door *)));
  
  //Checking the status of all doors
    for(int i = 0; i < count; i++){
      Serial.println("----------------Door------------");
      pinMode(door.Pin, door.Mode);
      Serial.println("Door " + (String)door.Pin);
      if(door.Mode == INPUT){
      //Serial.print((uint8_t)door.Mode);
      Serial.println("Sensor is active");  
      }else{
        Serial.println("Sensor is set incorectly");
      }

      if(door.State == 0){
        door.TimeStampClosed = now();
        Serial.print("Door is CLOSED now : ");
        printTime(door.TimeStampClosed);
      }

      if(door.State == 1){
        door.TimeStampOpen = now();
        Serial.print("Door is OPEN now : ");
        printTime(door.TimeStampOpen);
      }

      //sendSMS(door);
      
    }
  Serial.println("------------------------------------");
}

void printTime(time_t tempTime){
  Serial.print(hour(tempTime));
  Serial.print(":");
  Serial.print(minute(tempTime));
  Serial.print(":");
  Serial.print(second(tempTime));
  Serial.print(" ");
  Serial.print(day(tempTime));
  Serial.print("/");
  Serial.print(month(tempTime));
  Serial.print("/");
  Serial.print(year(tempTime));
  Serial.println();
}

void sendSMS(Door door){
  //https://stackoverflow.com/questions/16290981/how-to-transmit-a-string-on-arduino
  //issue with converting string class to char array  
  //telephone = "+4591520714";

  //command example
//  AT+CMGF=1
//> AT+CMGS="+4591520714"
//> 
//+CMGS: 71

//OK
  char toChar[16];
  sprintf(toChar, "%d", door.State);

  //Set SMS format to ASCII
  serialSIM800.write("AT+CMGF=1\r\n");
  delay(1000);
 
  //Send new SMS command and message number
  //This should be a real phone number
  
  serialSIM800.write("AT+CMGS=\"+4591520714\"\r\n");
  delay(1000);

 
  //Send SMS content
  serialSIM800.write(toChar);
  delay(1000);
   
  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);
     
  //delay(1000);
  //Serial.println("SMS has been sent");
  //printTime(now());
}

