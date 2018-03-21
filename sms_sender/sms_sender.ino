#include <Time.h>
#include <TimeLib.h>
#include <SoftwareSerial.h>
#include <gprs.h>
 
GPRS gprs;

char* number = (char*)"+4591520714";
char message[200] = "System has been armed";
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
    Serial.println("OK TIME");
  }
  
  converTime();
  
  
  Serial.println("SIM800L sending SMS...");
  sendSMS(smsMessage);
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

