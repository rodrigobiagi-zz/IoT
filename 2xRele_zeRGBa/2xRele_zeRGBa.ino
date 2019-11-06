/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial

#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "30bf518e8adc40fd95a07b0580554127"; // Token Blynk Server Local
//char auth[] = "f754054a9e1d4417800a2f1959d86b79"; //Token Blynk Server Remoto
//char ssid[] = "ELYSIUM"; // SSID da rede WiFi
char ssid[] = "Polotel Ap22";
//char pass[] = "Runner#932403@"; // Senha rede WiFi
char pass[] = "praiaperto2017";

const int redPin = 14;
const int greenPin = 12;
const int bluePin = 13;

#define zeRGBa V0
#define vRed V1
#define vGreen V2
#define vBlue V3
int r, g, b;  // Set RED BLUE GREEN channels

const int rele01Pin = 0; //rele luz solo
const int rele02Pin = 2; //rele luz teto

const int pull01Pin = 5; //switch luz solo
const int pull02Pin = 4; //switch luz teto

BlynkTimer timer;

//estado inicial dos reles
void checkPhysicalButton(); //Funcao checkPhysicalButton
int rele01State = HIGH; // Estado inicial do Rele 01
int rele02State = HIGH; // Estado inicial do Rele 02
int pull01State = HIGH; // Estado inicial do Pulsador 01
int pull02State = HIGH; // Estado inicial do Pulsador 02

void setup()
{
  // Debug console
  Serial.begin(115200);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  pinMode(rele01Pin, OUTPUT); //Declara pino 14 como saida
  pinMode(rele02Pin, OUTPUT); //Declara pino 12 como saida
  pinMode(pull01Pin, INPUT_PULLUP); //Declara pino 5 como entrada e ativa o resistor interno de pull up do esp01
  pinMode(pull02Pin, INPUT_PULLUP); //Declara pino 4 como entrada e ativa o resistor interno de pull up do esp01
  digitalWrite(rele01Pin, rele01State); //Recebe estado inicial do Rele 01
  digitalWrite(rele02Pin, rele02State); //Recebe estado inicial do Rele 02

  //Blynk.begin(auth, ssid, pass, IPAddress(10,12,25,100), 8080);
  Blynk.begin(auth, ssid, pass, "dux.casadohobby.com.br", 6008);
  while (Blynk.connect() == false){ //Aguardando o Blynk Local Server se conectar
  }

  timer.setInterval(100L, checkPhysicalButton); // determina o intervalo de atualização dos reles para 1seg
}

void loop()
{
  Blynk.run();
  timer.run();
}
