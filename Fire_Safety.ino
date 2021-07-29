#include<LiquidCrystal.h>
#include <SoftwareSerial.h>

// wifi module
int RX=10
int TX=11

String AP = "C_FAT";       
String PASS = "CFAT313"; 
String API = "MBXSPFJ5I58A0LO3";   
String HOST = "api.thingspeak.com";
String PORT = "80";
String field1 = "field1";
String field2 = "field2";
String field3 = "field3";
String field4 = "field4";
String field5 = "field5";

int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;
SoftwareSerial esp8266(RX,TX); 

// lcd screen pin initialization
const int rs = 11, en = 12, d4 = 7, d5 = 8, d6 = 9, d7 = 10; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// sonar pin initialization
int trigOne = 36;
int echoOne = 37;
int trigTwo = 34;
int echoTwo = 35;

int red = 33;
int green = 32;

static int humanCount= 0;

// temp sensor pin initialization
int tempPin = A1;
float temp;

// flame sensor initialization
int flamePin = 31;
int flame;

// gas sensor initialization
int gasPin = A2;
int gas;

// buzzer pin initialization
int buzzPin = 52;

// motor driver initialization
int in3 = 1;
int in4 = 2;
int enb = 3;

int in1 = 4;
int in2 = 5;
int ena = 6;

// light
int greenL = 40;
int redL = 41;

void setup() {
  // put your setup code here, to run once:
  lcd.begin(16, 2);

  // setiing up SONAR sensors
  pinMode(trigOne, OUTPUT);
  pinMode(trigTwo, OUTPUT);
  pinMode(echoOne,INPUT);
  pinMode(echoTwo,INPUT);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);

  // setting up temp sensor
  pinMode(tempPin, INPUT);

  // setting up flame sensor
  pinMode(flamePin, INPUT);

  // setting up gas sensor
  pinMode(gasPin, INPUT);

  // setting up buzzer
  pinMode(buzzPin, OUTPUT);

  // motor driver
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enb, OUTPUT);

  digitalWrite(in3, LOW);
  analogWrite(enb, 210);

  // light set up
  pinMode(redL, OUTPUT);
  pinMode(greenL, OUTPUT);
  
  // set up wifi module
  Serial.begin(9600);
  esp8266.begin(115200);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {
  // put your main code here, to run repeatedly:
  lcd.setCursor(0,0);
  lcd.clear();
  
  // temp calculation
  temp = analogRead(tempPin);
  temp = (temp*350)/1023; // need some adjustment

  lcd.print(temp);
  lcd.print("C ");

  String getData1 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(temp);
  sendCommand("AT+CIPMUX=1",5,"OK");
  sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
  sendCommand("AT+CIPSEND=0," +String(getData1.length()+4),4,">");
  esp8266.println(getData1);
  delay(1500);
  countTrueCommand++;
  sendCommand("AT+CIPCLOSE=0",5,"OK");

  

  // flame detection
  flame = digitalRead(flamePin);
  
  if(flame==1){
    lcd.print("F:N ");
    
    String getData2 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(!flame);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData2.length()+4),4,">");
    esp8266.println(getData2);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }

  else if(flame==0){
    lcd.print("F:Y ");

    String getData2 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(!flame);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData2.length()+4),4,">");
    esp8266.println(getData2);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }

  // gas detection
  gas = analogRead(gasPin);
  
  if(gas<700){
    lcd.print("G:N");

    String getData3 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(0);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData3.length()+4),4,">");
    esp8266.println(getData3);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }

  else if(gas>700){
    lcd.print("G:Y");

    String getData3 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(1);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData3.length()+4),4,">");
    esp8266.println(getData3);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }

  // respone due to flame or gas
  if(flame==0 || gas>700){
    // buzzer
    tone(buzzPin, 1200, 250);
    tone(buzzPin, 800, 250);
    


    // exhaust fan
    digitalWrite(in4, HIGH);

    // indicator light
    digitalWrite(redL, HIGH);
    digitalWrite(greenL, LOW);

    lcd.setCursor(7,1);
    lcd.print("DNE :(");

    String getData5 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(0);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData5.length()+4),4,">");
    esp8266.println(getData5);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }
  
  else if(flame!=0 || gas<700){
    // buzzer
    noTone(buzzPin);

    // exhaust fan
    digitalWrite(in4, LOW);

    // indicator light
    digitalWrite(redL, LOW);
    digitalWrite(greenL, HIGH);

    lcd.setCursor(7,1);
    lcd.print("SAFE :)"); 

    String getData5 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(1);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData5.length()+4),4,">");
    esp8266.println(getData5);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }
  

  // setting sonar trig pins high
  digitalWrite(trigOne, LOW);
  digitalWrite(trigTwo, LOW);
  digitalWrite(red, LOW);
  digitalWrite(green, LOW);
  delay(2);
  
  digitalWrite(trigOne, HIGH);
  digitalWrite(trigTwo, HIGH);
  delay(5);

  // counting humans
  if(digitalRead(echoOne) == LOW){
    digitalWrite(red, HIGH);
    humanCount++;
    delay(1500);
  }

  else if(digitalRead(echoTwo) == LOW){
    digitalWrite(green, HIGH);
    humanCount--;
    delay(1500);
  }

  lcd.setCursor(0,1);
  if(humanCount>=0){
    lcd.print(humanCount); 
    lcd.print(" P(s)");

    String getData4 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(humanCount);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData4.length()+4),4,">");
    esp8266.println(getData4);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }
  else if(humanCount<0){
    lcd.print("Error");
    humanCount++;

    String getData4 = "GET /update?api_key="+ API +"&"+ field1 +"="+String(humanCount);
    sendCommand("AT+CIPMUX=1",5,"OK");
    sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
    sendCommand("AT+CIPSEND=0," +String(getData4.length()+4),4,">");
    esp8266.println(getData4);
    delay(1500);
    countTrueCommand++;
    sendCommand("AT+CIPCLOSE=0",5,"OK");
  }
  
  delay(1000);

}


// function to send data 
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
