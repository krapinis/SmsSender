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
char *smsMessage;
long time = 0;
long debounce = 200;

#pragma region Door Class...

class Door
{
  //HIGH and LOW is 1 and 0
public:
  int Pin;
  int SmsLED = 4;
  int DoorLED = 5;
  char DoorLEDMode = OUTPUT;
  byte DoorLEDState;
  char SmsLEDMode = OUTPUT;
  //State of the door is 0 open or 1 closed
  byte State;
  byte PreviousState = 0;
  bool InitialState;
  char Mode;
  bool SmSent = false;
  int SmSCount;
  time_t TimeStampOpen;
  time_t TimeStampClosed;
  String ErrorMessage;
  String Message;

  Door(){};
  void setPin(int pin)
  {
    this->Pin = pin;
    this->Mode = INPUT;
    this->InitialState = true;
    this->Message = "Door state is neutral at this moment";
  }
  
  void setState(int state)
  {

    if (state == 0)
    {
      this->State = 0;
      this->PreviousState = 1;
      this->Message = "The door is open";
    }
    if (state == 1)
    {
      this->State = 1;
      this->PreviousState = 0;
      this->Message = "The door is closed";
    }
  }
};

#pragma endregion Door Class...

//Instance of the door
Door door;

#pragma region SETUP...

void setup()
{
    //---------TIME default Setup----------------------//
    
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
  
  door.setPin(3);
  door.State = digitalRead(door.Pin);
  if(door.State == 0){
    digitalWrite(door.DoorLED, 1);
  }
  
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while (!Serial);

  printStart();
  printDivider();
  
  //Being serial communication with Arduino and SIM800
  serialSIM800.begin(9600);

  //Array of doors
  Door doors[1] = {door};

  //Print list of doors
  Serial.println("Total doors in the system: " + (String)(sizeof(doors) / (sizeof(*doors))));

  //SETUP all the doors
  doorSetup(doors);

  Serial.println("\t\t\tSetup Complete!");
  Serial.println();
  Serial.println("\t\t     Ready to send SMS's...");
  printDivider();
  
  //char* tempPointer = (char *)malloc(200);
  smsMessage = createMessage(door);
  Serial.print(smsMessage);
  //free(smsMessage);
  //sendSMS(door);
}

#pragma endregion SETUP...

#pragma region LOOP...

void loop()
{
  door.State = digitalRead(door.Pin);


  //If start ssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

  if(door.State != door.PreviousState){  // && millis() - time > debounce){
    
      if(door.State == 1){
        door.DoorLEDState = 0;

        if(!door.InitialState && !door.SmSent){
            //sendSMS(smsMessage);
        }
      }
        
      if(door.State == 0){
        door.DoorLEDState = 1;
        
        if(door.InitialState){
          door.InitialState = false;
        }

        if(!door.SmSent){
          //sendSMS(smsMessage);
          Serial.println(smsMessage);
        }
      }
      //time = millis();
    }
    
    digitalWrite(door.DoorLED, door.DoorLEDState);
    door.PreviousState = door.State;

  
//  if(door.State != door.PreviousState ){
//    if(door.State == 1){
//      Serial.println("OPEN");
//    }
//      
//    if(door.State == 0){
//      Serial.println("CLOSED");
//    }
//    //delay(50);
//  }
  //Switch state
  //door.PreviousState = door.State;
    
    //char * msg = (char *)malloc(200);
    //msg = createMessage(door);
    //sendSMS(msg);
    //delay(1000);
    //digitalWrite(door.SmsLED, 1);
    //Serial.println("Open");

    //free(msg);
    

  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
  //if (serialSIM800.available())
  //{
  //  Serial.write(serialSIM800.read());
  //}
  //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
  //if (Serial.available())
  //{
  //  serialSIM800.write(Serial.read());
  //}
}

#pragma endregion LOOP...

#pragma region DoorSetup...

void doorSetup(Door doors[])
{

  int count = (sizeof(doors) / (sizeof(Door *)));

  //Checking the status of all doors
  for (int i = 0; i < count; i++)
  {
    Serial.println("\t-------------------Door----------------");
    pinMode(door.Pin, door.Mode);
    pinMode(door.DoorLED, door.DoorLEDMode);
    Serial.println("\tDoor " + (String)door.Pin);
    if (door.Mode == INPUT)
    {
      Serial.println("\tSensor is active");
    }
    else
    {
      door.ErrorMessage = "Error: sensor is setup incorect";
      Serial.println(door.ErrorMessage);
    }

    if (door.State == 0)
    {
      door.TimeStampOpen = now();
      //LED should be off
      door.setState(digitalRead(door.Pin));
      door.Message = "Door is OPEN ";
      Serial.print(door.Message);
      printTime(door.TimeStampOpen);
    }

    if (door.State == 1)
    {
      door.TimeStampClosed = now();
      door.setState(digitalRead(door.Pin));
      door.Message = "Door is CLOSED ";
      Serial.print(door.Message);
      printTime(door.TimeStampClosed);
    }
  }
  printDivider();
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

void printDivider(){
  Serial.println();
  Serial.println("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--");
  Serial.println(" o | o   o | o   o | o   o | o   o | o   o | o   o | o   o | o   o | o");
  Serial.println("---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+--");
  Serial.println();
}

void printStart(){
  Serial.println("\t  _________________________ ____________________");
  Serial.println("\t /   _____/\\__    ___/  _  \\______   \\__    ___/");
  Serial.println("\t \\_____  \\   |    | /  /_\\  \\|       _/ |    |   ");
  Serial.println("\t /        \\  |    |/    |    \\    |   \\ |    |   ");
  Serial.println("\t/_______  /  |____|\\____|__  /____|_  / |____|   ");
  Serial.println("\t        \\/                 \\/       \\/           ");
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
  char buffTime[20];
  
  //time_t now = time(NULL);

  //strftime(buff, 20, "%Y-%m-%d %H:%M:%S",localtime(&now));
  
  sprintf(tempDoorNumber, "%d", tDoor.Pin); 
  strcpy(msg, "Door number ");
  strcat(msg, tempDoorNumber);
  
  if(tDoor.State == 0)
  {
    strcat(msg, " is OPEN");
    strcat(msg, "\r");
    strcat(msg, "\n");
    strcat(msg, "this a new line");
    return msg;
  }
  if(tDoor.State == 1)
  {
    strcat(msg, " is CLOSED");
    strcat(msg, "\r");
    strcat(msg, "\n");
    
    //Time conversion to const char*
    char tempHour[3];
    sprintf(tempHour , "%d", hour(door.TimeStampClosed));
    char tempMinute[3];
    sprintf(tempMinute , "%d", minute(door.TimeStampClosed));
    char tempSecond[3];
    sprintf(tempSecond , "%d", second(door.TimeStampClosed));
    char tempDay[2];
    sprintf(tempDay , "%d", day(door.TimeStampClosed));
    char tempMonth[2];
    sprintf(tempMonth , "%d", month(door.TimeStampClosed));
    char tempYear[4];
    sprintf(tempYear , "%d", year(door.TimeStampClosed));
    
    
    //Time HH:MM:SS
    strcat(msg, tempHour);
    strcat(msg, ":");
    strcat(msg, tempMinute);
    strcat(msg, ":");
    strcat(msg, tempSecond);
    strcat(msg, "\t");
    
    //Date DD/MM/YYYYY
    strcat(msg, tempDay);
    strcat(msg, "/");
    strcat(msg, tempMonth);
    strcat(msg, "/");
    strcat(msg, tempYear);
    strcat(msg, "\r");
    strcat(msg, "\n");
    //FORMAT the time here
    
    return msg;
  }
  else
  {
    door.ErrorMessage = "ERROR: Major error";
    strcat(msg, "ERROR");
    return msg;
  }
}

void sendSMS(char *smsMessage)
{

  smsMessage = createMessage(door);
    
    //At this point module is ready to send SMS
    initializeSMSend();
    
    //Serial.println(smsMessage);
    serialSIM800.write(smsMessage);
    
    //Ctrl+Z termination char
    serialSIM800.write((char)26);

    door.SmSent = true;
    
    //free(message);
}

#pragma endregion SMS...
