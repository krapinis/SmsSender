#include <Time.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
//#include <Door.h>
#include <gprs.h>
 
GPRS gprs;

//  Sketch: SwitchingThings_02
//
//  An  example of turning an LED on and off using a button switch
//
//  Pins
//  D10 to resister and LED
//  D2 to push button switch
//  
 
// Define the pins being used
int pin_LED = 13; //out
int pin_switch = 4; //in
 
// variables to hold the new and old switch states
boolean oldSwitchState = LOW;
boolean newSwitchState = LOW;
 


long time = 0;         // the last time the output pin was toggled
long debounce = 200;   // the debounce time, increase if the output flickers
char* number = (char*)"+4591520714";
char* number1 = (char*)"+4551222390";
char message[256] = "System has been armed\r\n";
char tempHours[5];
char tempMinutes[5];
char tempSeconds[5];
char timeString[15];
char* smsMessage;
time_t current_time;
 
void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin_switch, INPUT);
  pinMode(pin_LED, OUTPUT);
  Serial.print("SWITCH IN ");
  Serial.println(pin_switch);
  Serial.print("OUT ");
  Serial.println(pin_LED);
  
}

void converTime(){
  //FORMAT TIME
    
    //HOURS
    snprintf(tempHours, 5, "%d", hour(current_time));
    strcat(timeString, tempHours);
    strcat(timeString, ":");
    
    //MINUTES
    snprintf(tempMinutes, 5, "%d", minute(current_time));
    strcat(timeString, tempMinutes);
    strcat(timeString, ":");
    
    //SECONDS
    snprintf(tempSeconds, 5, "%d", second(current_time));
    strcat(timeString, tempSeconds);

    //SMS Message
    strcat(message, timeString);
    smsMessage = message;
    
    Serial.println(smsMessage);

    
}

void loop() {

    if ( newSwitchState != oldSwitchState ) 
    {
       if ( newSwitchState == HIGH )
       {
        Serial.print("SMS Sent... ");
        Serial.println(newSwitchState);
        
        //digitalWrite(pin_LED, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
       }
       else
       {
        Serial.print("SMS Sent... ");
        Serial.println(newSwitchState);
        
        //digitalWrite(pin_LED, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
       }
 
       oldSwitchState = newSwitchState;
    }   

newSwitchState = digitalRead(pin_switch);
  
}

void sendSMS(char *msg){
  Serial.println("GPRS - Send SMS Test ...");
  gprs.preInit();
  delay(1000);
  while(0 != gprs.init()) {
      delay(1000);
      Serial.print("init error\r\n");
  } 
  Serial.println("Init success, start to send SMS message...");
  //char* t = (char*)"TEST";
  gprs.sendSMS(number, msg); //define phone number and text
  Serial.println("SMS has been sent");
}

