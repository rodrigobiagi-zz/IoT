#define BLYNK_PRINT Serial

/* Bibliotecas utilizadas */
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

// RTC sincronizado com o Blynk Server, nocaso utilizo um server local em uma Rasp
BlynkTimer timer; //Objeto do tipo BlynkTimer
WidgetRTC rtc;

const int rele01Pin = 0; // GPIO0
const int pull01Pin = 2; // GPIO2

char auth[] = "151be8454b2a4363a0b3716b47a31a14"; // Token fornecido pelo projeto
char ssid[] = "ELYSIUM_AUT"; // SSID da rede WiFi
char pass[] = "Aut@93240364"; // Senha rede WiFi

int rele01State = HIGH; // Estado inicial do Rele 01
int pull01State = HIGH; // Estado inicial do Pulsador 01

void setup(){
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, IPAddress(10,12,25,100), 8080);
  //Blynk.begin(auth, ssid, pass, "dux.casadohobby.com.br", 6008);

  pinMode(rele01Pin, OUTPUT); //Declara GPIO0 como saida
  pinMode(pull01Pin, INPUT_PULLUP); //Declara GPIO2 como entrada e ativa o resistor interno de pull up do esp01
  digitalWrite(rele01Pin, rele01State); //Recebe estado inicial do Rele 01

  setSyncInterval(30 * 60); // Sets the time synchronization interval

  timer.setInterval(5000L, timerControl);
}

BLYNK_CONNECTED(){ // Funcao que verifica a conexao com o servidor
  Serial.println("CONNECTED");
  Blynk.syncAll();
  rtc.begin(); // Start RTC Widget
}

void loop(){
  timer.run();
  Blynk.run();
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

//=======================================================================================
/********************************
 * Timer Definitions
 *********************************/

// Aqui comeca a brincadeira, converter o tempo para inteiro... 
// StartTime e StopTimer é o Time Imput do Blynk
long int startTimer1;
long int stopTimer1;

BLYNK_WRITE(V0){ // Schedule 1
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

//=======================================================================================
/********************************
 * On/Off Definitions
 *********************************/

int onOffRele;

 BLYNK_WRITE(V1){ // On/Off Rele 1
  onOffRele = param.asInt();
}

//=======================================================================================
/********************************
 * Timer Function
 *********************************/

void timerControl(){
  long int nowSeconds = ((hour() * 3600L) + (minute() * 60) + second());
  Serial.println (nowSeconds);

// aqui eu tenho o nowSeconds que e a hr que peguei no RTC e o startTimer1 e stopTimer1 que são os Timers do Blynk convertidos
// e ai faco um if comparando se estiver dentro do timer definido e estado do botão no blynk for 0 então seta o rele como low
  long int sch1 = ((nowSeconds >= startTimer1) && (nowSeconds <= stopTimer1));
  if((sch1) && (onOffRele == 0)){
    Serial.print("Schedule 1 - Started - ");
    Serial.print("Now Seconds");
    Serial.print(" - ");
    Serial.print(nowSeconds);
    Serial.print(" - ");
    Serial.print("Start Timer");
    Serial.print(" - ");
    Serial.println(startTimer1);
      
    Blynk.virtualWrite(V2, 1023);
    digitalWrite(rele01Pin, LOW);
  }
  else{
      Serial.println("Rele 1 Off...");
      Blynk.virtualWrite(V2, 0);
      digitalWrite(rele01Pin, HIGH);
  }
}
