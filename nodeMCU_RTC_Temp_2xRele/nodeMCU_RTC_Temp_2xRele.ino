/*AQUAMONITOR
 * ESP8266 - NODEMCU
 * Automacao Lampada + Nonitoramento da Temperatura
 * Descricao:
 * 01 - push button   D1 - GPIO5
 * 02 - push button   D2 - GPIO4
 * 
 * 01 - RTC PCF8563 - D3 SDA - GPIO18
 *                  - D4 SCL - GPIO17
 *                  
 * 01 - Rele        - D5 - GPIO12 - HIGH
 * 02 - Rele        - D6 - GPIO14 - HIGH
 * 
 * 01 - DS18B20     - D7 - GPIO13
 * 
 * Livres
 * SD2 - GPIO9 - HIGH
 * SD3 - GPIO10 - HIGH
 * D8 - GPIO15 - LOW - oscila
 * RX - GPIO3 - HIGH
 * TX - GPIO1 - High - oscila

 * 
 * V1  - Estado Rele 01
 * V2  - Estado Rele 02
 * V10 - Temperatuda grafico
 * V11 - Temperatura tempo real
 * V20 - Semana
 * V21 - Dia
 * V22 - Hora
 * 
 * Atualiza a Temperatura a cada 1 minuto
 * 
 * Blynk Local Server
 * ip: 10.12.25.100
 * port: 8080
 * 
 */

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include <WidgetRTC.h>

BlynkTimer timer; //Objeto do tipo BlynkTimer (SimpleTimer)
WidgetRTC rtc;

char auth[] = "30bf518e8adc40fd95a07b0580554127"; // Token Blynk Server Local
char ssid[] = "ELYSIUM"; // SSID da rede WiFi
char pass[] = "Runner#932403@"; // Senha rede WiFi

// Configuracao do sensor de temperatura
#define ONE_WIRE_BUS D7   // Define porta D7 para sensor de Temperatura
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float temp;

//=============================================================================
const int rele01Pin = D5; // Porta D5 - 12
const int rele02Pin = D6; // Porta D6 - 14
const int rele03Pin = D3; // Porta D5 - 12
const int rele04Pin = D4; // Porta D6 - 14

int rele01State = HIGH; // Estado inicial do Rele 01
int rele02State = HIGH; // Estado inicial do Rele 02
int rele03State = HIGH; // Estado inicial do Rele 01
int rele04State = HIGH; // Estado inicial do Rele 02

// Definicoes RTC
const String DayOfWeek[7] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};
const String Month[12] = {"Jan", "Fev", "Mar", "Abr", "Mai", "Jun", "Jul", "Ago", "Set", "Out", "Nov", "Dez"};
String showWeek;
String showMonth;
String currentTime;
String currentDate;

void setup()
{
  // Debug console
  Serial.begin(115200);

  Blynk.begin(auth, ssid, pass, IPAddress(10,12,25,100), 8080);
  //Blynk.begin(auth, ssid, pass, "dux.casadohobby.com.br", 6008);
  while (Blynk.connect() == false){ //Aguardando o Blynk Local Server se conectar
  }

  pinMode(rele01Pin, OUTPUT); //Declara pino 14 como saida
  pinMode(rele02Pin, OUTPUT); //Declara pino 12 como saida
  pinMode(rele03Pin, OUTPUT); //Declara pino 14 como saida
  pinMode(rele04Pin, OUTPUT); //Declara pino 12 como saida
  digitalWrite(rele01Pin, rele01State); //Recebe estado inicial do Rele 01
  digitalWrite(rele02Pin, rele02State); //Recebe estado inicial do Rele 02
  digitalWrite(rele03Pin, rele01State); //Recebe estado inicial do Rele 01
  digitalWrite(rele04Pin, rele02State); //Recebe estado inicial do Rele 02
  
  DS18B20.begin(); // Inicia o Sensor de Temperatura

  setSyncInterval(10 * 60); // Define o intervalo de sincronismo de horário
  // Agendamentos
  timer.setInterval(10000L, sendTemp);  // determina intervalo de atualização da temperatura para 1 min
  timer.setInterval(1000L, clockDisplay); // Display digital clock every 10 seconds
}

// Sincroniza as portas do App
BLYNK_CONNECTED(){
  Serial.println("CONNECTED");  Blynk.run();
  Blynk.syncAll(); // Sincroniza todas as portas virtuais
  rtc.begin();
}

void loop()
{
  Blynk.run();
  timer.run();
}
