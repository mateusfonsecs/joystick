#include <RF24.h>

#include <Servo.h>

//*************** Controle do RF ***********************
#define radioID 1  //Informar "0" para um dispositivo e "1" para o outro dispositivo

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
int bat1Ant = 0;
int bat1    = 0;


int angulo1 = 0;
int angulo2 = 0;
int angulo3 = 0;
int anguloM = 0;



Servo servo1;
Servo servo2;
Servo servo3;
Servo servoM;


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
  
      pinMode(A7, INPUT);
      servo1.attach(3);
      servo2.attach(5);
      servo3.attach(6);
      servoM.attach(9);
      

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
  bat1 = analogRead(A7);
    if (bat1 <= 9) {
       dadosRF.bateria = bat1;
      alterado = true;  
    }
    bat1Ant = bat1;

  angulo1 = map(dadosRF.potenciometro1, 0, 1023, 0, 180);  
  servo1.write(angulo1);

  angulo2 = map(dadosRF.potenciometro2, 0, 1023, 0, 180);  
  servo2.write(angulo2);

  angulo3 = map(dadosRF.potenciometro3, 0, 1023, 0, 180);  
  servo3.write(angulo3);

  anguloM = map(dadosRF.potenciometro4, 0, 1023, 0, 179);  
  servoM.write(anguloM);
 
 
  delay(10);
}
