#include <Time.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <gprs.h>
 
GPRS gprs;

char* number = (char*)"+4591520714";
char* message = (char*)"System has been armed\r\n";
char tempHours[5];
char tempMinutes[5];
char tempSeconds[5];
char* timeString = (char*)"";
char* smsMessage = (char*)"";
time_t current_time;
 
void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);
  
  //---------TIME default Setup----------------------//
    
  //setTime(hours, minutes, seconds, days, months, years);
  setTime(15, 59, 3, 16, 3, 18);
  time_t current_time = now();
  if (timeStatus() == timeNotSet)
  {
    error:
    Serial.println("ERROR time set incorrect");
  }
  else
  {
    Serial.println("OK Time set correct");
  }
  
  converTime();
  
  
  //Serial.println("Sending SMS ...");
  //sendSMS(smsMessage);
}

void converTime(){
  //FORMAT TIME
    
    //HOURS
    
    sprintf(tempHours, "%d", hour(current_time));
    strcat(timeString, tempHours);
    strcat(timeString, ":");

    //MINUTES
    sprintf(tempMinutes, "%d",minute(current_time));
    strcat(timeString, tempMinutes);
    strcat(timeString, ":");

    //SECONDS
    sprintf(tempSeconds, "%d", second(current_time));
    strcat(timeString, tempSeconds);
    
    //strcat(message, timeString);
    //String temp = message + String(timeString);
    char* s = (char*)"abc";
    for(int i = 0; i <= strlen(s); i++){
      Serial.println(s[i]);
      
    }
}

void loop() {
  
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
  gprs.sendSMS(number, msg); //define phone number and text
}

