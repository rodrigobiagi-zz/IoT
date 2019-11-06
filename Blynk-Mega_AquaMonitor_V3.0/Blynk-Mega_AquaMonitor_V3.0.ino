/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <Ethernet.h>
#include <BlynkSimpleEthernet.h>
#include <WidgetRTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>

BlynkTimer timer; // Object BlynkTimer (SimpleTimer)
WidgetRTC rtc;

char auth[] = "8fd1995ccb8a474e93c8b2da35bd6cad"; // Token Blynk Server

#define W5100_CS  10
#define SDCARD_CS 4

// Define Define
const int port01  = 24; // relay WaveMaker 1
const int port02  = 25; // relay WaveMaker 2
const int port03  = 26; // relay UV filter
const int port04  = 27; // relay light on/off
const int control = 28; // relay Control
const int alarm   = 29; // relay Alarm
const int led01   =  2; // led Day (white)
const int led02   =  3; // led Night (blue)

int rele01State = HIGH; // Start Relay 01
int rele02State = HIGH; // Start Relay 02
int rele03State = HIGH; // Start Relay 03
int rele04State = HIGH; // Start Relay 04
int rele05State = HIGH; // Start Relay 05
int rele06State = HIGH; // Start Relay 06

// Temperature Sensor
#define ONE_WIRE_BUS 22   // Define GPIO 22 to Temperature Sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;

void setup(){
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, IPAddress(10,12,25,100), 8080);
  //Blynk.begin(auth, "dux.casadohobby.com.br", 6008);
  while (Blynk.connect() == false){ //Aguardando o Blynk Local Server se conectar
  }

  pinMode(port01,  OUTPUT); // Declares pin 24 as output
  pinMode(port02,  OUTPUT); // Declares pin 25 as output
  pinMode(port03,  OUTPUT); // Declares pin 26 as output
  pinMode(port04,  OUTPUT); // Declares pin 27 as output
  pinMode(control, OUTPUT); // Declares pin 28 as output
  pinMode(alarm,   OUTPUT); // Declares pin 29 as output
  pinMode(led01,    OUTPUT); // Declares pin 2 as output
  pinMode(led02,    OUTPUT); // Declares pin 3 as output
  
  digitalWrite(control, rele01State); //Receives initial status of Relay 01
  digitalWrite(alarm,   rele02State); //Receives initial status of Relay 02
  digitalWrite(port01,  rele03State); //Receives initial status of Relay 03
  digitalWrite(port02,  rele04State); //Receives initial status of Relay 04
  digitalWrite(port03,  rele05State); //Receives initial status of Relay 05
  digitalWrite(port04,  rele06State); //Receives initial status of Relay 06

  pinMode(SDCARD_CS, OUTPUT);
  digitalWrite(SDCARD_CS, HIGH); // Deselect the SD card

  DS18B20.begin(); // Initializes the temperature sensor

  setSyncInterval(30 * 60); // Sets the time synchronization interval
  
  // Schedules
  timer.setInterval(2000L, clockDisplay); // Display digital clock every 5 seconds
  timer.setInterval(5000L, timerControl);
  timer.setInterval(5500L, tempControl);
}

// Synchronize App ports
BLYNK_CONNECTED(){
  Serial.println("CONNECTED");
  Blynk.syncAll();
  rtc.begin(); // Start RTC Widget
}

void loop(){
  Blynk.run();
  timer.run();
}

//=======================================================================================
/********************************
 * RTC
 *********************************/

// Define RTC
const String DayOfWeek[7] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
const String Month[12] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};
String showWeek;
String showMonth;
String currentTime;
String currentDate;

// Digital clock display of the time
String printDigits(int digits){
  String new_digits;
  if (digits < 10) {
    new_digits += "0";
  }
  new_digits += String(digits);
  return new_digits;
}

void clockDisplay(){
  showWeek = DayOfWeek[weekday()-1];
  showMonth = Month[month()-1];

  currentTime = String(printDigits(hour())) + ":" + printDigits(minute()) + ":" + printDigits(second());
  currentDate = String(showWeek) + " - " + printDigits(day()) + "/" + showMonth + "/" + year();
  
  /*
  Serial.print("Current time: ");
  Serial.print(showWeek);
  Serial.print(" ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();
  */
  // Send time to the App
  Blynk.virtualWrite(V1, currentTime);
  // Send date to the App
  Blynk.virtualWrite(V2, currentDate);
}

//=======================================================================================
/********************************
 * Temperature Definitions
 *********************************/
int delta1Max = 0;
int delta2Max = 0;
int delta1Min = 0;
int delta2Min = 0;

BLYNK_WRITE(V12){ // Function Delta 1 Min
  delta1Min = param.asInt(); // Variable receives and stores APP data
}

BLYNK_WRITE(V13){ // Function Delta 2 Min
  delta2Min = param.asInt(); // Variable receives and stores APP data
}

BLYNK_WRITE(V14){ // Function Delta 1 Max
  delta1Max = param.asInt(); // Variable receives and stores APP data
}

BLYNK_WRITE(V15){ // Function Delta 2 Max
  delta2Max = param.asInt(); // Variable receives and stores APP data
}

//=======================================================================================
/********************************
 * Timer Definitions
 *********************************/

// Led Parameters
int pw0  =   0;
int pw1  =   5;
int pw2  =  10;
int pw3  =  25;
int pw4  =  50;
int pw5  =  75;
int pw6  = 100;
int pw7  = 125;
int pw8  = 150;
int pw9  = 175;
int pw10 = 200;
int pw11 = 225;

long int startLedTimer;
long int stopLedTimer;
long int ledOff;

long int SThour;
long int STmin;
long int STsec;
long int SPhour;
long int SPmin;
long int SPsec;

// WaveMaker
long int startTimer1;
long int stopTimer1;
long int startTimer2;
long int stopTimer2;
long int startTimer3;
long int stopTimer3;
long int startTimer4;
long int stopTimer4;
long int startTimer5;
long int stopTimer5;
long int startTimer6;
long int stopTimer6;

BLYNK_WRITE(V21){ // Schedule 1 WaveMaker 1
  TimeInputParam t(param);
  if (t.hasStartTime()){
    startTimer1 = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopTimer1 = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V22){ // Schedule 2 WaveMaker 1
  TimeInputParam t(param);
  if (t.hasStartTime()){
    startTimer2 = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopTimer2 = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V23){ // Schedule 1 WaveMaker 2
  TimeInputParam t(param);
  if (t.hasStartTime()){
    startTimer3 = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopTimer3 = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V24){ // Schedule 2 WaveMaker 2
  TimeInputParam t(param);
  if (t.hasStartTime()){
    startTimer4 = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopTimer4 = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V25){ // Schedule 1 UV Filter
  TimeInputParam t(param);
  if (t.hasStartTime()){
    startTimer5 = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopTimer5 = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V26){ // Schedule 2 UV Filter
  TimeInputParam t(param);
  if (t.hasStartTime()){
    startTimer6 = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopTimer6 = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V27) { // Schedule Leds
  TimeInputParam t(param);
  SThour = t.getStartHour();
  STmin = t.getStartMinute();
  STsec = t.getStartSecond();
  SPhour = t.getStopHour();
  SPmin = t.getStopMinute();
  SPsec = t.getStopSecond();
  
  if (t.hasStartTime()){
    startLedTimer = ((t.getStartHour() * 3600L) + (t.getStartMinute() * 60) + t.getStartSecond());
    Serial.print("LED - Start Timer - ");
    Serial.println(startLedTimer);
  }
  else{
    // Do nothing
  }
  if (t.hasStopTime()){
    stopLedTimer = ((t.getStopHour() * 3600L) + (t.getStopMinute() * 60) + t.getStopSecond());
    Serial.print("LED - Stop Timer - ");
    Serial.println(stopLedTimer);
  }
  else{
    // Do nothing: no stop time was set
  }
}

BLYNK_WRITE(V28){
  ledOff = param[0].asLong();
  Serial.println("ledOff");
  Serial.println(ledOff);
}

//=======================================================================================
/********************************
 * On/Off Definitions
 *********************************/
int onOffLed;
int onOffWaveMaker01;
int onOffWaveMaker02;
int onOffUVFilter;
int onOffTemp;

BLYNK_WRITE(V30){ // On/Off Led
  onOffLed = param.asInt();
}

BLYNK_WRITE(V31){ // On/Off WaveMaker 1
  onOffWaveMaker01 = param.asInt();
}

BLYNK_WRITE(V32){ // On/Off WaveMaker 2
  onOffWaveMaker02 = param.asInt();
}

BLYNK_WRITE(V33){ // On/Off UV Filter
  onOffUVFilter = param.asInt();
}

BLYNK_WRITE(V34){ // On/Off Control
  onOffTemp = param.asInt();
}

//=======================================================================================
/********************************
 * Temperature Function
 *********************************/
void tempControl(){
  DS18B20.requestTemperatures();
  temp = DS18B20.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.println(temp);
  
  Blynk.virtualWrite(V10,temp); // Virtual Port 10 graphic
  Blynk.virtualWrite(V11,temp); // Virtual Port 11 real time

  if(onOffTemp == 0){
    if(temp > delta2Min && temp < delta1Max){
      Blynk.virtualWrite(V6, 0); // Led Control
      Blynk.virtualWrite(V7, 0); // Led Alarm
      Serial.println("Control and Alarm Off");
      digitalWrite(control, HIGH);
      digitalWrite(alarm,   HIGH);
    }
    else if(temp < delta1Min && temp <= delta2Min){
      Blynk.virtualWrite(V6, 1023); // Led Control
      Blynk.virtualWrite(V7, 0); // Led Alarm
      Serial.println("Control On");
      digitalWrite(control, LOW);
      digitalWrite(alarm,   HIGH);
    }
    else if(temp > delta1Max && temp >= delta2Max){
      Blynk.virtualWrite(V7, 1023); // Led Alarm
      Blynk.virtualWrite(V6, 0); // Led Control
      Serial.println("Alarm On");
      digitalWrite(control, HIGH);
      digitalWrite(alarm,   LOW);
    }
  }
  else if(onOffTemp == 1){
    Serial.println("Temp Control Disabled");
    Blynk.virtualWrite(V6, 0); // Led Control
    Blynk.virtualWrite(V7, 0); // Led Alarm
    digitalWrite(control, HIGH);
    digitalWrite(alarm,   HIGH);
  }
}

//=======================================================================================
/********************************
 * Timer Function
 *********************************/
void timerControl(){
  long int nowSeconds = ((hour() * 3600L) + (minute() * 60) + second());
  Serial.println (nowSeconds);
  
  long int sch0MLed  = ((SThour * 3600L) + (STmin * 60) + STsec);
  long int sch1MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 300);
  long int sch2MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 900);
  long int sch3MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 1200);
  long int sch4MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 1500);
  long int sch5MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 1800);
  long int sch6MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 2100);
  long int sch7MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 2400);
  long int sch8MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 2700);
  long int sch9MLed  = ((SThour * 3600L) + (STmin * 60) + STsec + 3000);
  long int sch10MLed = ((SThour * 3600L) + (STmin * 60) + STsec + 3300);

  long int sch0NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec);
  long int sch1NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 300);
  long int sch2NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 900);
  long int sch3NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 1200);
  long int sch4NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 1500);
  long int sch5NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 1800);
  long int sch6NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 2100);
  long int sch7NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 2400);
  long int sch8NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 2700);
  long int sch9NLed  = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 3000);
  long int sch10NLed = ((SPhour * 3600L) + (SPmin * 60) + SPsec + 3300);

// Day Control
  if((nowSeconds >= startLedTimer) && (nowSeconds <= ledOff) && (onOffLed == 0)){
    digitalWrite(port04, LOW);
    if((nowSeconds >= startLedTimer) && (nowSeconds <= stopLedTimer)){
      Blynk.virtualWrite(V8, 1023);
      Blynk.virtualWrite(V9, 0);
      if((nowSeconds >= sch0MLed) && (nowSeconds <= sch1MLed)){
        Serial.print("sch0MLed - ");
        Serial.println(sch0MLed);
        analogWrite(led01, pw2);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch1MLed) && (nowSeconds <= sch2MLed)){
        Serial.print("sch1MLed - ");
        Serial.println(sch1MLed);
        analogWrite(led01, pw3);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch2MLed) && (nowSeconds <= sch3MLed)){
        Serial.print("sch2MLed - ");
        Serial.println(sch2MLed);
        analogWrite(led01, pw4);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch3MLed) && (nowSeconds <= sch4MLed)){
        Serial.print("sch3MLed - ");
        Serial.println(sch3MLed);
        analogWrite(led01, pw5);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch4MLed) && (nowSeconds <= sch5MLed)){
        Serial.print("sch4MLed - ");
        Serial.println(sch4MLed);
        analogWrite(led01, pw6);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch5MLed) && (nowSeconds <= sch6MLed)){
        Serial.print("sch5MLed - ");
        Serial.println(sch5MLed);
        analogWrite(led01, pw7);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch6MLed) && (nowSeconds <= sch7MLed)){
        Serial.print("sch6MLed - ");
        Serial.println(sch6MLed);
        analogWrite(led01, pw8);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch7MLed) && (nowSeconds <= sch8MLed)){
        Serial.print("sch7MLed - ");
        Serial.println(sch7MLed);
        analogWrite(led01, pw9);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch8MLed) && (nowSeconds <= sch9MLed)){
        Serial.print("sch8MLed - ");
        Serial.println(sch8MLed);
        analogWrite(led01, pw10);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch9MLed) && (nowSeconds <= sch10MLed)){
        Serial.print("sch9MLed - ");
        Serial.println(sch9MLed);
        analogWrite(led01, pw10);
        analogWrite(led02, pw0);
      }
      else if((nowSeconds >= sch10MLed) && (nowSeconds <= stopLedTimer)){
        Serial.print("sch10MLed - ");
        Serial.println(sch10MLed);
        analogWrite(led01, pw10);
        analogWrite(led02, pw0);
      }
    }
    // Night Control
    else if((nowSeconds >= stopLedTimer) && (nowSeconds <= ledOff)){
      Blynk.virtualWrite(V8, 0);
      Blynk.virtualWrite(V9, 1023);
      if((nowSeconds >= sch0NLed) && (nowSeconds <= sch1NLed)){
        Serial.print("sch0NLed - ");
        Serial.println(sch0NLed);
        analogWrite(led01, pw9);
        analogWrite(led02, pw2);
      }
      else if((nowSeconds >= sch1NLed) && (nowSeconds <= sch2NLed)){
        Serial.print("sch1NLed - ");
        Serial.println(sch1NLed);
        analogWrite(led01, pw8);
        analogWrite(led02, pw4);
      }
      else if((nowSeconds >= sch2NLed) && (nowSeconds <= sch3NLed)){
        Serial.print("sch2NLed - ");
        Serial.println(sch2NLed);
        analogWrite(led01, pw6);
        analogWrite(led02, pw6);
      }
      else if((nowSeconds >= sch3NLed) && (nowSeconds <= sch4NLed)){
        Serial.print("sch3NLed - ");
        Serial.println(sch3NLed);
        analogWrite(led01, pw4);
        analogWrite(led02, pw7);
      }
      else if((nowSeconds >= sch4NLed) && (nowSeconds <= sch5NLed)){
        Serial.print("sch4NLed - ");
        Serial.println(sch4NLed);
        analogWrite(led01, pw2);
        analogWrite(led02, pw8);
      }
      else if((nowSeconds >= sch5NLed) && (nowSeconds <= sch6NLed)){
        Serial.print("sch5NLed - ");
        Serial.println(sch5NLed);
        analogWrite(led01, pw1);
        analogWrite(led02, pw9);
      }
      else if((nowSeconds >= sch6NLed) && (nowSeconds <= sch7NLed)){
        Serial.print("sch6NLed - ");
        Serial.println(sch6NLed);
        analogWrite(led01, pw0);
        analogWrite(led02, pw9);
      }
      else if((nowSeconds >= sch7NLed) && (nowSeconds <= sch8NLed)){
        Serial.print("sch7NLed - ");
        Serial.println(sch7NLed);
        analogWrite(led01, pw0);
        analogWrite(led02, pw9);
      }
      else if((nowSeconds >= sch8NLed) && (nowSeconds <= sch9NLed)){
        Serial.print("sch8NLed - ");
        Serial.println(sch8NLed);
        analogWrite(led01, pw0);
        analogWrite(led02, pw9);
      }
      else if((nowSeconds >= sch9NLed) && (nowSeconds <= sch10NLed)){
        Serial.print("sch9MLed - ");
        Serial.println(sch9MLed);
        analogWrite(led01, pw0);
        analogWrite(led02, pw9);
      }
      else if((nowSeconds >= sch10NLed) && (nowSeconds <= ledOff)){
        Serial.print("sch10NLed - ");
        Serial.println(sch10NLed);
        analogWrite(led01, pw0);
        analogWrite(led02, pw9);
      }
    }
  }
  else{
    Serial.println("Led Off...");
    Blynk.virtualWrite(V8, 0);
    Blynk.virtualWrite(V9, 0);
    digitalWrite(port04, HIGH);
  }

//=======================================================================================
 
  long int sch1 = ((nowSeconds >= startTimer1) && (nowSeconds <= stopTimer1));
  long int sch2 = ((nowSeconds >= startTimer2) && (nowSeconds <= stopTimer2));
  long int sch3 = ((nowSeconds >= startTimer3) && (nowSeconds <= stopTimer3));
  long int sch4 = ((nowSeconds >= startTimer4) && (nowSeconds <= stopTimer4));
  long int sch5 = ((nowSeconds >= startTimer5) && (nowSeconds <= stopTimer5));
  long int sch6 = ((nowSeconds >= startTimer6) && (nowSeconds <= stopTimer6));

  //if(sch1 || sch2 || sch3 || sch4 || sch5 || sch6){
  if((sch1) || (sch2) && (onOffWaveMaker01 == 0)){
    Serial.println("Schedule 1 or 2 Active");
// Schedule 1 e 2 WaveMaker 1
    if((sch1) && (onOffWaveMaker01 == 0)){
      Serial.print("Schedule 1 - Started - ");
      Serial.print("Now Seconds");
      Serial.print(" - ");
      Serial.print(nowSeconds);
      Serial.print(" - ");
      Serial.print("Start Timer");
      Serial.print(" - ");
      Serial.println(startTimer1);
      
      Blynk.virtualWrite(V3, 1023);
      digitalWrite(port01, LOW);
    }
  
    else if((sch2) && (onOffWaveMaker01 == 0)){
      Serial.print("Schedule 2 - Started - ");
      Serial.print("Now Seconds");
      Serial.print(" - ");
      Serial.print(nowSeconds);
      Serial.print(" - ");
      Serial.print("Start Timer");
      Serial.print(" - ");
      Serial.println(startTimer2);
      
      Blynk.virtualWrite(V3, 1023);
      digitalWrite(port01, LOW);
    }
    else{
      Serial.println("WaveMaker 1 Off...");
      Blynk.virtualWrite(V3, 0);
      digitalWrite(port01, HIGH);
    }
  }
  else{
    Serial.println("WaveMaker 1 Off...");
    Blynk.virtualWrite(V3, 0);
    digitalWrite(port01, HIGH);
  }

// Schedule 3 e 4 WaveMaker 2
  if((sch3) || (sch4) && (onOffWaveMaker02 == 0)){
    Serial.println("Schedule 3 or 4 Active");
    if((sch3) && (onOffWaveMaker02 == 0)){
      Serial.print("Schedule 3 - Started - ");
      Serial.print("Now Seconds");
      Serial.print(" - ");
      Serial.print(nowSeconds);
      Serial.print(" - ");
      Serial.print("Start Timer");
      Serial.print(" - ");
      Serial.println(startTimer3);
      
      Blynk.virtualWrite(V4, 1023);
      digitalWrite(port02, LOW);
    }
    else if((sch4) && (onOffWaveMaker02 == 0)){
      Serial.print("Schedule 4 - Started - ");
      Serial.print("Now Seconds");
      Serial.print(" - ");
      Serial.print(nowSeconds);
      Serial.print(" - ");
      Serial.print("Start Timer");
      Serial.print(" - ");
      Serial.println(startTimer4);
  
      Blynk.virtualWrite(V4, 1023);
      digitalWrite(port02, LOW);
    }
    else{
      Serial.println("WaveMaker 2 Off...");
      Blynk.virtualWrite(V4, 0);
      digitalWrite(port02, HIGH);
    }
  }
  else{
    Serial.println("WaveMaker 2 Off...");
    Blynk.virtualWrite(V4, 0);
    digitalWrite(port02, HIGH);
  }

// Schedule 5 - 6 UV Filter
  if((sch5) || (sch6) && (onOffUVFilter == 0)){
    if((sch5) && (onOffUVFilter == 0)){
      Serial.print("Schedule 5 - Started - ");
      Serial.print("Now Seconds");
      Serial.print(" - ");
      Serial.print(nowSeconds);
      Serial.print(" - ");
      Serial.print("Start Timer");
      Serial.print(" - ");
      Serial.println(startTimer5);
    
      Blynk.virtualWrite(V5, 1023);
      digitalWrite(port03, LOW);
    }
    
    else if((sch6) && (onOffUVFilter == 0)){
      Serial.print("Schedule 6 - Started - ");
      Serial.print("Now Seconds");
      Serial.print(" - ");
      Serial.print(nowSeconds);
      Serial.print(" - ");
      Serial.print("Start Timer");
      Serial.print(" - ");
      Serial.println(startTimer6);
    
      Blynk.virtualWrite(V5, 1023);
      digitalWrite(port03, LOW);
    }
    else{
      Serial.println("UV Filter Off...");
      Blynk.virtualWrite(V5, 0);
      digitalWrite(port03, HIGH);
    }
  }
  else{
    Serial.println("UV Filter Off...");
    Blynk.virtualWrite(V5, 0);
    digitalWrite(port03, HIGH);
  }
}
