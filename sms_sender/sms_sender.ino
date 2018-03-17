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
SoftwareSerial serialSIM800(SIM800_TX_PIN, SIM800_RX_PIN);

//TELEPHONE
//+4591829151 sender
const char* telephone = "+4591520714";

#pragma region Door Class...

class Door
{
  //HIGH and LOW is 1 and 0
public:
  int Pin;
  int SmsLED = 4;
  int DoorLED = 5;
  char DoorLEDMode = OUTPUT;
  char SmsLEDMode = OUTPUT;
  //State of the door is 0 open or 1 closed
  int State;
  char Mode;
  bool SmSent = false;
  time_t TimeStampOpen;
  time_t TimeStampClosed;
  String ErrorMessage;
  String Message;

  Door(){};
  void setPin(int pin)
  {
    this->Pin = pin;
    this->State = getState(pin);
    //digitalRead(pin);
    this->Mode = INPUT;

    if (this->State == 0)
    {
      digitalWrite(DoorLED, 1);
    }
    else if (this->State == 1)
    {
      digitalWrite(DoorLED, 0);
    }else{
      this->ErrorMessage = "Error: Check door state";
    }
  }
  int getState(int pin){
    return digitalRead(pin);
  }
  void setState(int state)
  {

    if (state == 0)
    {
      State = 0;
      TimeStampOpen = now();
      digitalWrite(DoorLED, 1);
    }
    if (state == 1)
    {
      State = 1;
      TimeStampClosed = now();
      digitalWrite(DoorLED, 0);
    }
  }
  void doorOpenTime()
  {
    TimeStampOpen = now();
    Serial.println(TimeStampOpen);
  }
  void doorClosedTime()
  {
    TimeStampClosed = now();
    Serial.println(TimeStampClosed);
  }
};

#pragma endregion Door Class...

//Instance of the door
Door door;

#pragma region SETUP...

void setup()
{

  door.setPin(3);
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while (!Serial);

  printStart();
  
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);

  //---------TIME default Setup----------------------------------------------------------------------
  //setTime(hours, minutes, seconds, days, months, years);
  setTime(15, 59, 3, 16, 3, 18);
  time_t tempTime = now();
  if (timeStatus() == timeNotSet)
  {
    Serial.println("Time SETUP is incorect");
  }
  else
  {
    Serial.print("Time has been set to this : ");
    printTime(tempTime);
  }

  //Array of doors
  Door doors[1] = {door};

  //Print list of doors
  Serial.println("Total doors in the system: " + (String)(sizeof(doors) / (sizeof(*doors))));

  //SETUP all the doors
  doorSetup(doors);

  Serial.println("\t\t\tSetup Complete!");
  Serial.println();
  Serial.println("\t\t     Ready to send SMS's...");
  printDevider();
  
  char* tempPointer = (char *)malloc(200);
  tempPointer = createMessage(door);
  Serial.print(tempPointer);
  free(tempPointer);
  //sendSMS(door);
}

#pragma endregion SETUP...

#pragma region LOOP...

void loop()
{
  if (timeStatus() == timeNotSet)
  {
    Serial.println("Time is not set");
  }

  //If the door is CLOSED
  if (digitalRead(door.Pin) == 1)
  {
    
  }

  //If the door is OPEN
  if (digitalRead(door.Pin) == 0)
  {
    
  }

  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  if (serialSIM800.available())
  {
    Serial.write(serialSIM800.read());
  }
  //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
  if (Serial.available())
  {
    serialSIM800.write(Serial.read());
  }
}

#pragma endregion LOOP...

#pragma region DoorSetup...

void doorSetup(Door doors[])
{

  int count = (sizeof(doors) / (sizeof(Door *)));

  //Checking the status of all doors
  for (int i = 0; i < count; i++)
  {
    Serial.println("----------------Door------------");
    pinMode(door.Pin, door.Mode);
    pinMode(door.DoorLED, door.DoorLEDMode);
    Serial.println("Door " + (String)door.Pin);
    if (door.Mode == INPUT)
    {
      Serial.println("Sensor is active");
    }
    else
    {
      Serial.println("Sensor is set incorectly");
    }

    if (door.State == 0)
    {
      door.TimeStampOpen = now();
      //LED should be off
      door.setState(digitalRead(door.Pin));
      Serial.print("Door is OPEN now : ");
      printTime(door.TimeStampOpen);
    }

    if (door.State == 1)
    {
      door.TimeStampClosed = now();
      door.setState(digitalRead(door.Pin));
      Serial.print("Door is CLOSED now : ");
      printTime(door.TimeStampClosed);
    }
  }
  printDevider();
}

#pragma endregion DoorSetup...

#pragma region Time...

void printTime(time_t tempTime)
{
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

#pragma endregion Time...

#pragma region Print Formatting...

void printDevider(){
  Serial.println();
  Serial.println("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--");
  Serial.println(" o | o   o | o   o | o   o | o   o | o   o | o   o | o   o | o   o | o");
  Serial.println("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--");
  Serial.println();
}

void printStart(){
  Serial.println("  _________________________ ____________________");
  Serial.println(" /   _____/\\__    ___/  _  \\______   \\__    ___/");
  Serial.println(" \\_____  \\   |    | /  /_\\  \\|       _/ |    |   ");
  Serial.println(" /        \\  |    |/    |    \\    |   \\ |    |   ");
  Serial.println("/_______  /  |____|\\____|__  /____|_  / |____|   ");
  Serial.println("        \\/                 \\/       \\/           ");
  Serial.println();
}


#pragma endregion Print Formatting...

#pragma region SMS...


void setGSMModemToTextMode(){
  char cmd[12];
  strcpy(cmd, "AT+CMGF=1");
  strcat(cmd,"\r");
  strcat(cmd,"\n");
  serialSIM800.write(cmd);
  delay(50);
}

void initializeSMSend(){
  setGSMModemToTextMode();
  char cmd[23];
  strcpy(cmd, "AT+CMGS=\"");
  strcat(cmd, telephone);
  strcat(cmd, "\"");
  strcat(cmd, "\r");
  strcat(cmd, "\n");
  serialSIM800.write(cmd);
  delay(100);
}

char* createMessage(Door tDoor){

  //Memory allocation for the message
  char *msg = (char *)malloc(200);
  char tempDoorNumber[1];
  sprintf(tempDoorNumber, "%d", tDoor.Pin); 
  strcpy(msg, "Door number ");
  strcat(msg, tempDoorNumber);
  
  if(tDoor.State == 0)
  {
    strcat(msg, " is OPEN");
    return msg;
  }
  if(tDoor.State == 1)
  {
    strcat(msg, " is CLOSED");  
    return msg;
  }
  else
  {
    return NULL;
  }
}

void sendSMS()
{

  char* smsMessage = (char *)malloc(200);
  
  if (door.State == 0)
  {
    
    
    smsMessage = createMessage(door);
    
    //At this point module is ready to send SMS
    //initializeSMSend();
    
    //Serial.println(smsMessage);
    //serialSIM800.write(smsMessage);
    
    //Ctrl+Z termination char
    //serialSIM800.write((char)26);
    free(smsMessage);
  }

  if (door.State == 1)
  {
    //At this point module is ready to send SMS
//    initializeSMSend();
//    strcat(smsMessage, " is CLOSED");
//    strcat(smsMessage, "\r");
//    strcat(smsMessage, "\n");
//    strcat(smsMessage, "now you should open it");
//    strcat(smsMessage, "\r");
//    strcat(smsMessage, "\n");
    
    
    //serialSIM800.write(smsMessage);
    //serialSIM800.write((char)26);
  }
  //smsMessage[0]=0;
}

#pragma endregion SMS...
