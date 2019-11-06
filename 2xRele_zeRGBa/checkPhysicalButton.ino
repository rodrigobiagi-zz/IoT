BLYNK_CONNECTED(){ // Funcao que verifica a conexao com o servidor
  Blynk.syncAll();
  //Blynk.syncVirtual(V10); // Solicita o ultimo estado do pino virtual V10
  //Blynk.syncVirtual(V11); // Solicita o ultimo estado do pino virtual V11
}

BLYNK_WRITE(V10){ // Funcao que muda o estado do Rele 01 quando o botao no APP e pressionado
  rele01State = param.asInt(); // Variavel recebe e armazena o dado do APP
  digitalWrite(rele01Pin, rele01State); // Altera o estado do Rele 01
}

BLYNK_WRITE(V11){ // Funcao que muda o estado do Rele 02 quando o botao no APP e pressionado
  rele02State = param.asInt(); // Variavel recebe e armazena o dado do APP
  digitalWrite(rele02Pin, rele02State); // Altera o estado do Rele 01
}

// Funcao para controle dos reles
void checkPhysicalButton(){

//Rele 01 - Pulsador 01
  if (digitalRead(pull01Pin) == LOW){ // Se a leitura do Pulsador 01 for igual a LOW
    if (pull01State != LOW){ // Se o estado do Pulsador 01 for diferente de LOW
      rele01State = !rele01State; // Estado do Rele 01 recebe o estado atual inverso
      digitalWrite(rele01Pin, rele01State); // Alterna o estado atual do Rele 01
      Blynk.virtualWrite(V10, rele01State); // Atualiza no projeto o estado atual do Rele 01
    }
    pull01State = LOW; // Estado do Pulsador 01 recebe LOW
  }
  else{ // senao faz
    pull01State = HIGH; // Estado do Pulsador 01 HIGH
  }


//Rele 02 - Pulsador 02
  if (digitalRead(pull02Pin) == LOW){ // Se a leitura do Pulsador 02 for igual a LOW
    if (pull02State != LOW){ // Se o estado do Pulsador 02 for diferente de LOW
      rele02State = !rele02State; // Estado do Rele 02 recebe o estado atual inverso
      digitalWrite(rele02Pin, rele02State); // Alterna o estado atual do Rele 02
      Blynk.virtualWrite(V10, rele02State); // Atualiza no projeto o estado atual do Rele 02
    }
    pull02State = LOW; // Estado do Pulsador 02 recebe LOW
  }
  else{ // senao faz
    pull02State = HIGH; // Estado do Pulsador 02 HIGH
  }
}
