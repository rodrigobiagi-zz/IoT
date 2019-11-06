/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
  This example code is in public domain.

 *************************************************************

  Fade using a LED widget on your phone!

  App project setup:
    LED widget on V2
 *************************************************************/

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial


#include <SPI.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "30bf518e8adc40fd95a07b0580554127"; // Token Blynk Server Local
//char auth[] = "f754054a9e1d4417800a2f1959d86b79"; //Token Blynk Server Remoto
char ssid[] = "ELYSIUM"; // SSID da rede WiFi
char pass[] = "Runner#932403@"; // Senha rede WiFi

BlynkTimer timer;
#define led V1;
const int pin1 = D2;


void setup()
{
  // Debug console
  Serial.begin(9600);

  Blynk.begin(auth, ssid, pass, IPAddress(10,12,25,100), 8080);
  while (Blynk.connect() == false){ //Aguardando o Blynk Local Server se conectar
  }

  timer.setInterval(300L, RGBprocess);
}

void loop()
{
  Blynk.run();
  timer.run();
}

