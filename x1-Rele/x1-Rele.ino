
/*
 * ESP8266 - ESP01
 * Automacao Lampada
 * Descricao:
 * 01 - Rele - GPIO0
 * 01 - push button - GPIO2
 * 
 * Blynk Local Server
 * ip: 10.12.25.100
 * port: 8080
 * 
 */
/* Comente esta linha para desabilitar os prints e economizar espaço */
#define BLYNK_PRINT Serial

/* Bibliotecas utilizadas */
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

BlynkTimer timer; //Objeto do tipo BlynkTimer

const int rele01Pin = 0; // GPIO0
const int pull01Pin = 2; // GPIO2

/* 
 * Auth Token for device 'nodeMCU' : 30bf518e8adc40fd95a07b0580554127 off
 * Auth Token for device 'SalaJantar' : amHgvvirpPa2W8buqqfpHoOPFw6ETUjx ok
 * Auth Token for device 'SalaQuadro' : 0dfc04dbc20e4dacbc0eacf687cf6ff7 ok
 * Auth Token for device 'Abajour' : 84f0e039fb194bb39fbccd46445f0a0d ok
 * Auth Token for device 'AquaMonitor' : 8fd1995ccb8a474e93c8b2da35bd6cad Cabo
 * Auth Token for device 'SalaTv' : 15095d1eec9b4119838249da39db9a29 ok
 * Auth Token for device 'Sacada' : 35b9e7df2e824fb49296760d12c22419 ok
 * Auth Token for device 'Irrigacao' : bca20937d3d14fa69d1aeb56b4ddf7d3 ok
 * Auth Token for device 'QuartoHelena' : a5db2034fc474c1d9cb802f416355300 ok
 * Auth Token for device 'CorredorBanho' : 4eb39f6a325746efbb8e10ffbf94e844 ok
 * Auth Token for device 'Grow' : 151be8454b2a4363a0b3716b47a31a14 ok
 */

char auth[] = "bca20937d3d14fa69d1aeb56b4ddf7d3"; // Token fornecido pelo projeto
char ssid[] = "ELYSIUM_AUT"; // SSID da rede WiFi
char pass[] = "Aut@93240364"; // Senha rede WiFi

void checkPhysicalButton(); //Funcao checkPhysicalButton

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
  
  timer.setInterval(100L, checkPhysicalButton); // determina o intervalo de atualização dos reles para 1seg
}

BLYNK_CONNECTED(){ // Funcao que verifica a conexao com o servidor
  Blynk.syncVirtual(V3); // Solicita o ultimo estado do pino virtual V3
}
 
BLYNK_WRITE(V3){ // Funcao que muda o estado do Rele 01 quando o botao no APP e pressionado
  rele01State = param.asInt(); // Variavel recebe e armazena o dado do APP
  digitalWrite(rele01Pin, rele01State); // Altera o estado do Rele 01
}

void loop(){
  timer.run();
  Blynk.run();
}

// Funcao para controle dos reles
void checkPhysicalButton(){
  if (digitalRead(pull01Pin) == LOW){ // Se a leitura do Pulsador 01 for igual a LOW
    if (pull01State != LOW){ // Se o estado do Pulsador 01 for diferente de LOW
      rele01State = !rele01State; // Estado do Rele 01 recebe o estado atual inverso
      digitalWrite(rele01Pin, rele01State); // Alterna o estado atual do Rele 01
      Blynk.virtualWrite(V3, rele01State); // Atualiza no projeto o estado atual do Rele 01
    }
    pull01State = LOW; // Estado do Pulsador 01 recebe LOW
  }
  else{ // senao faz
    pull01State = HIGH; // Estado do Pulsador 01 HIGH
  }
}
