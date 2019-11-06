BLYNK_CONNECTED(){ // Funcao que verifica a conexao com o servidor
  Blynk.syncVirtual(V1); // Solicita o ultimo estado do pino virtual V1
}

BLYNK_WRITE(V1){ // Funcao que muda o estado do Rele 01 quando o botao no APP e pressionado
  ledState = param.asInt(); // Variavel recebe e armazena o dado do APP
  analogWrite(pin1, 255);
}

void ledControl(){

  
}

