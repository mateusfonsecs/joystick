#include <RF24.h>

#include <Servo.h>

//*************** Controle do RF ***********************
#define radioID 0  //Informar "0" para um dispositivo e "1" para o outro dispositivo

struct estruturaDadosRF
{
   boolean ligando = false;   //Esta variavel será usada para solicitar os dados do outro aparelho. Será útil quando o aparelho solicitante esta sendo ligado, para manter os valores do aparelho que já esta ligado.
   boolean botao1 = false;
   boolean botao2 = false;
   boolean botao3 = false;
   boolean botao4 = false;
   int potenciometro1 = 0;
   int potenciometro2 = 0;
   int potenciometro3 = 0;
   int potenciometro4 = 0;
   int bateria = 0;
   

};
typedef struct estruturaDadosRF tipoDadosRF;
tipoDadosRF dadosRF;
tipoDadosRF dadosRecebidos;

boolean transmitido = true;
boolean alterado = false;

RF24 radio(7,8);

byte enderecos[][6] = {"1node","2node"};


//*************** Controle do Projeto LOCAL ************
boolean botao1Ant = HIGH;
boolean botao1    = HIGH;
boolean botao2Ant = HIGH;
boolean botao2    = HIGH;
boolean botao3Ant = HIGH;
boolean botao3    = HIGH;
boolean botao4Ant = HIGH;
boolean botao4    = HIGH;


int pot1Ant = 0;
int pot1    = 0;
int pot2Ant = 0;
int pot2    = 0;
int pot3Ant = 0;
int pot3    = 0;
int pot4Ant = 0;
int pot4    = 0;



void setup() {
  //*************** Controle do RF ***********************
  radio.begin();
  
  #if radioID == 0
      radio.openWritingPipe(enderecos[0]);
      radio.openReadingPipe(1, enderecos[1]);
  #else
      radio.openWritingPipe(enderecos[1]);
      radio.openReadingPipe(1, enderecos[0]);
  #endif

  //Solicita os dados do outro aparelho, se ele estiver ligado. Tenta a comunicação por 2 segundos.
  dadosRF.ligando = true;
  radio.stopListening();                                   
  long tempoInicio = millis();
  while ( !radio.write( &dadosRF, sizeof(tipoDadosRF) ) ) {
     if ((millis() - tempoInicio) > 2000) {
        break;
     }   
  }
  dadosRF.ligando = false; 
  radio.startListening();  


  //*************** Controle do Projeto LOCAL ************
  
      //pinMode(A3, INPUT);
      pinMode(A4, INPUT);
      pinMode(A5, INPUT);
      pinMode(A6, INPUT);
      pinMode(A7, INPUT);
      pinMode(2, INPUT_PULLUP); 
      pinMode(3, INPUT_PULLUP);
      pinMode(4, INPUT_PULLUP);
      pinMode(5, INPUT_PULLUP);
      pinMode(6, OUTPUT);
}


void loop() {
  //*************** Controle do RF ***********************
  // se houve alteração dos dados, envia para o outro radio 
  if (alterado || !transmitido) {
     radio.stopListening();                                   
     transmitido = radio.write( &dadosRF, sizeof(tipoDadosRF) );
     radio.startListening();  
     alterado = false;
  }

  //verifica se esta recebendo mensagem       
  if (radio.available()) {                                   
     radio.read( &dadosRecebidos, sizeof(tipoDadosRF) ); 

     //verifica se houve solicitação de envio dos dados
     if (dadosRecebidos.ligando) {
        alterado = true;
     } else {
        dadosRF = dadosRecebidos;
     }
  }



  //*************** Controle do Projeto LOCAL ************
 
    pot1 = analogRead(A4);
    if (pot1 != pot1Ant) {
       dadosRF.potenciometro1 = pot1;
       alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
    }
    pot1Ant = pot1;
  
    pot2 = analogRead(A5);
    if (pot2 != pot2Ant) {
       dadosRF.potenciometro2 = pot2;
      alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
    }
    pot2Ant = pot2;

     pot3 = analogRead(A6);
    if (pot3 != pot3Ant) {
       dadosRF.potenciometro3 = pot3;
      alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
    }
    pot3Ant = pot3;

     pot4 = analogRead(A7);
    if (pot4 != pot4Ant) {
       dadosRF.potenciometro4 = pot4;
      alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
    }
    pot4Ant = pot4;
    
   botao1 = digitalRead(2);
  if ((botao1 != botao1Ant)) {
     dadosRF.botao1 = !dadosRF.botao1;
     alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
  }
  botao1Ant = botao1;

  
   botao2 = digitalRead(3);
  if ((botao2 != botao2Ant)) {
     dadosRF.botao2 = !dadosRF.botao2;
     alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
  }
  botao2Ant = botao2;
  
   botao3 = digitalRead(4);
  if ((botao3 != botao3Ant)) {
     dadosRF.botao3 = !dadosRF.botao3;
     alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
  }
  botao3Ant = botao3;
   
   botao4 = digitalRead(5);
  if ((botao4 != botao4Ant)) {
     dadosRF.botao4 = !dadosRF.botao4;
     alterado = true;  //esta variavel controla o envio dos dados sempre que houver uma alteração
  }
   botao4Ant = botao4;

   digitalWrite(6, dadosRF.bateria);
  
  
  delay(10);
}
