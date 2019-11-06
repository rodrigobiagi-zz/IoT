int maxTemp1 = 0;
int maxTemp2 = 0;
int minTemp1 = 0;
int minTemp2 = 0;

BLYNK_WRITE(V12){ // Funcao que muda o estado do Rele 01 quando o botao no APP e pressionado
  minTemp1 = param.asInt(); // Variavel recebe e armazena o dado do APP
}

BLYNK_WRITE(V13){ // Funcao que muda o estado do Rele 01 quando o botao no APP e pressionado
  minTemp2 = param.asInt(); // Variavel recebe e armazena o dado do APP
}

BLYNK_WRITE(V14){ // Funcao que muda o estado do Rele 01 (controle) 
  maxTemp1 = param.asInt(); // Variavel recebe e armazena o dado do APP
}

BLYNK_WRITE(V15){ // Funcao que muda o estado do Rele 01 quando o botao no APP e pressionado
  maxTemp2 = param.asInt(); // Variavel recebe e armazena o dado do APP
}

// Instrucao para envio dos dados de Temperatura
void sendTemp(){
  DS18B20.requestTemperatures();
  temp = DS18B20.getTempCByIndex(0);

  Serial.print("Temperature: ");
  Serial.println(temp);
  
  Blynk.virtualWrite(V10,temp); // Porta Virtual 10 para grafico
  Blynk.virtualWrite(V11,temp); // Porta Virtual 11 para real time

  if(temp >= minTemp2 && temp <= maxTemp1){
    Serial.println("Controle e Alarme Desligados");
    digitalWrite(rele01Pin, HIGH);
    digitalWrite(rele02Pin, HIGH);
  }
  else if(temp < minTemp1 && temp < minTemp2){
    Serial.println("Controle Ligado");
    digitalWrite(rele01Pin, LOW);
    digitalWrite(rele02Pin, HIGH);
  }
  else if(temp > maxTemp1 && temp > maxTemp2){
    Serial.println("Alarme Ligado");
    digitalWrite(rele01Pin, HIGH);
    digitalWrite(rele02Pin, LOW);
  }
}
