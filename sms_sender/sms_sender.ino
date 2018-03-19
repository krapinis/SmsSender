#include <SoftwareSerial.h>
#include <gprs.h>
 
//SIM800 TX is connected to Arduino D8
//#define SIM800_TX_PIN 8
 
//SIM800 RX is connected to Arduino D7
//#define SIM800_RX_PIN 7
 
//Create software serial object to communicate with SIM800
//SoftwareSerial serialSIM800(SIM800_TX_PIN,SIM800_RX_PIN);

GPRS gprs;
 
void setup() {
  //Begin serial comunication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  while(!Serial);

  Serial.println("GPRS - Send SMS Test ...");
  gprs.preInit();
  delay(1000);
  while(0 != gprs.init()) {
      delay(1000);
      Serial.print("init error\r\n");
  } 
  Serial.println("Init success, start to send SMS message...");
  gprs.sendSMS("+4591520714","hello,world"); //define phone number and text

   
  //Being serial communication witj Arduino and SIM800
//  serialSIM800.begin(9600);
//  delay(1000);
//   
//  Serial.println("Setup Complete!");
//
//  //Set SMS format to ASCII
//  serialSIM800.write("AT+CMGF=1\r\n");
//  delay(1000);
// 
//  //Send new SMS command and message number
//  serialSIM800.write("AT+CMGS=\"+4591520714\"\r\n");
//  delay(1000);
//   
//  //Send SMS content
//  serialSIM800.write("TEST\r\nHello");
//  delay(1000);
//   
//  //Send Ctrl+Z / ESC to denote SMS message is complete
//  serialSIM800.write((char)26);
//  delay(1000);
//     
//  Serial.println("SMS Sent!");
}
 
void loop() {
//  //Read SIM800 output (if available) and print it in Arduino IDE Serial Monitor
//  if(serialSIM800.available()){
//    Serial.write(serialSIM800.read());
//  }
//  //Read Arduino IDE Serial Monitor inputs (if available) and send them to SIM800
//  if(Serial.available()){    
//    serialSIM800.write(Serial.read());
//  }
}
