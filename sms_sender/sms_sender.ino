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

char enterKey = 13;
String ctrlZKey = "\x1A";
String smsMessage = "";

#pragma region Door Class...

class Door
{
public:
  int Pin;
  int SmsLED = 4;
  int DoorLED = 5;
  char DoorLEDMode = OUTPUT;
  char SmsLEDMode = OUTPUT;
  //State of the door is 0 open or 1 closed
  int State;
  char Mode;
  time_t TimeStampOpen;
  time_t TimeStampClosed;

  Door(){};
  void setPin(int pin)
  {
    Pin = pin;
    State = digitalRead(pin);
    Mode = INPUT;

    if (State == 0)
    {
      digitalWrite(DoorLED, 1);
    }
    if (State == 1)
    {
      digitalWrite(DoorLED, 0);
    }
  }
  void setState(int state)
  {

    if (state == 0)
    {
      State = 0;
      TimeStampOpen = now();
      digitalWrite(DoorLED, 1);
    }
    else if (state == 1)
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

  //Setting up the door
  //door.setPin(3);
  door.setPin(3);
  //door.State = digitalRead(door.Pin);

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
    door.setState(1);
    //1 the door is closed
    //digitalWrite(door.DoorLED, 0);
  }

  //If the door is OPEN
  if (digitalRead(door.Pin) == 0)
  {
    door.setState(0);
    sendSMS(door);
    //digitalWrite(door.DoorLED, 1);
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
      Serial.print("Door is OPEN now : ");
      printTime(door.TimeStampOpen);
    }

    if (door.State == 1)
    {
      door.TimeStampClosed = now();
      Serial.print("Door is CLOSED now : ");
      printTime(door.TimeStampClosed);
    }

    //sendSMS(door);
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
  Serial.println(" /   _____/\__    ___/  _  \\\______   \\__    ___/");
  Serial.println(" \\_____  \\   |    | /  /_\\  \\|       _/ |    |   ");
  Serial.println(" /        \\  |    |/    |    \\    |   \\ |    |   ");
  Serial.println("/_______  /  |____|\\____|__  /____|_  / |____|   ");
  Serial.println("        \\/                 \\/       \\/           ");
  Serial.println();
}


#pragma endregion Print Formatting...

#pragma region SMS...

void setGSMModemToTextMode(){
  //initialize text mode
  //https://www.diafaan.com/sms-tutorials/gsm-modem-tutorial/at-cmgf/
  //AT+CMGF=<mode><CR>
  //Two modes Text or PDU
  serialSIM800.print("AT+CMGF=1");
  serialSIM800.print(enterKey);
}

void sendSMS(Door door)
{
  //Concatination issues in C
  //https://stackoverflow.com/questions/2218290/concatenate-char-array-in-c

  //https://stackoverflow.com/questions/16290981/how-to-transmit-a-string-on-arduino
  //issue with converting string class to char array
  //telephone = "+4591520714";
  
  char *str = "The door is ";
  char msg[20];
  strcpy(msg, str);

  //Set SMS format to ASCII
  //serialSIM800.write("AT+CMGF=1\r\n");
  setGSMModemToTextMode();
  delay(1500);

  //Send new SMS command and message number

  serialSIM800.write("AT+CMGS=\"+4591520714\"\r\n");
  delay(1000);

  if (door.State == 0)
  {

    strcat(msg, "OPEN");
    Serial.println(msg);
  }

  if (door.State == 1)
  {
    strcat(msg, "CLOSED");
    Serial.println(msg);
  }

  //convert intiger to char
  //char toChar[16];
  //sprintf(toChar, "%d", door.State);

  //Send SMS content
  serialSIM800.write(msg);
  delay(1000);

  //Send Ctrl+Z / ESC to denote SMS message is complete
  serialSIM800.write((char)26);
  delay(1000);

  //delay(1000);
  //Serial.println("SMS has been sent");
  //printTime(now());
}
#pragma endregion SMS...
