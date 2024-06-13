
/*
  MQQT ARDUINO - Servidor TEMPORIS
  VITECH
  Autor: Cristian Villaleiva
  Fuente: https://www.youtube.com/watch?v=VSwu-ZYiTxg
  Versión: A
*/

uint32_t Tiempo_S1 = 100; // 1 segundo
uint32_t Tiempo_S1_clear = 100; // 1 segundo

 // INCLUDE
// #include <SPI.h>
// #include <Ethernet.h>
// #include <PubSubClient.h>
// #include <TimeLib.h>
//Definiciones
#define Sensor1_pin 2
#define Sensor2_pin 3
#define Sensor3_pin 4
#define Sensor4_pin 5
 //Constantes
 //Dirección MAC de este dispositivo (Normalmente imprese en un sticker en el shield)
 //En caso de no estar, crear un valor unico de 6 hexadecimales
 //const byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
// const byte mac[] = { 0xDE, 0XAD, 0xBE, 0xFE, 0xF0, 0X02}; //(228: 237: 186: 254: 254: 237)
// // Dirección IP unica en caso que falle la asginación automatica (DHCP)
//    IPAddress deviceIP(192, 168, 51 ,37);
//    IPAddress IP_DNS(192, 168, 1, 24);
//    IPAddress gateway(192, 168, 51, 1);
//    IPAddress subnet(255, 255, 255, 0);
// //Dirección IP de la maquina en el red que corre el MQTT broker
// const IPAddress mqttServerIP(192, 168, 51, 10);
// //dirección del broker
//const int mqtt_port = 1883;
// // We will use an NTP server from https://tf.nist.gov/tf-cgi/servers.cgi
// IPAddress timeSrvr(129,6,15,29);
// // define Ethernet UDP object and local port 8888
//EthernetUDP ethernet_UDP;
//unsigned int NTP_Port = 8888;


 //ID unico de este dispositivo, usado como ID de cliente para conectarse al servidor MQTT
 //También ustilizado como Topic name para los mensjaes publicados por este dispositivo
 const char* deviceID = "1343E2B";
 const char* machineID = "3";
 

 //Globales
// //Crear una isntancia de Ethernet client
// EthernetClient ethernetClient; 
// //Crear una instancia de MQTT client basada en el Ethernet client
// PubSubClient MQTTclient(ethernetClient);
 // buffer donde alamcenar el mensaje a ser enviado
 char msg_aux[110];
 //reserva de variable para el Topic a ser utilizado
 char topic[32];
 
 char* TOPIC1 = "CICCHILLAN/SENSORES/FLUJO/1343E2B";


// variable to store previous displayed time
//time_t prevDisplay = 0;
// array to hold incoming/outgoing NTP messages
// NTP time message is 48 bytes long
//byte messageBuffer[48];

//auxiliar milisegundos
unsigned long aux_millis_S1=0;
unsigned long aux_millis_S2=0;
unsigned long millis_prev=0;
//Temporizadores
uint32_t T1=10000;
uint32_t T1_actual=0;
uint32_t T1_anterior=0;

//Temporizadores sensores
uint32_t T_S1=1000; // tiempo minimo entre productos 
uint32_t T_S1_actual=0;
uint32_t T_S1_anterior=0;
uint32_t T_S1_inactividad=30000; //Tiempo para decretar inactividad 30seg

//Temporizadores sensores
uint32_t T_S2=500; // tiempo minimo entre productos
uint32_t T_S2_actual=0;
uint32_t T_S2_anterior=0;
uint32_t T_S2_inactividad=30000; //Tiempo para decretar inactividad 30seg

//Estado Sensores
//Valor 0: sin detección
//valor 1: producto detectado
//valor 5: 30 segundos de inactividad
volatile int estado_S1=0; //
volatile int estado_S2=0;

//Contadores de debugg
volatile int contador_S1_ISR=0;
int contador_S1=0;
volatile int contador_S2_ISR=0;
int contador_S2=0;

uint32_t T_S1_Down = 0;
uint32_t T_S1_Up = 0;
uint32_t T_S1_Down2 = 0;
uint32_t T_S1_Up2 = 0;
bool S1_paso0=true;
bool S1_paso1=false;
bool S1_paso2=false;

void setup(){
   Serial.begin(9600);
//   Serial.println("inicializando");
//  ethernetSetup();
//  mqttSetup();
  pinMode(Sensor1_pin, INPUT_PULLUP);
  pinMode(Sensor2_pin, INPUT_PULLUP);
  delay(1000);
  noInterrupts();
  //attachInterrupt(digitalPinToInterrupt(Sensor1_pin), Deteccion_1, FALLING); //detectar cambio de estado en entrada (high estado normal)
  //attachInterrupt(digitalPinToInterrupt(Sensor2_pin), Deteccion_2, FALLING); //detectar camhio de estado en entrada (high estado normal)
  delay(1);
  interrupts();

  while(!Serial){
    ;
  }
Serial.println("SENSOR/1;CTM_VALOR/5;TOTAL/0");
Serial.println("SENSOR/2;CTM_VALOR/5;TOTAL/0");
//  ethernet_UDP.begin(NTP_Port);
//  setSyncProvider(getTime);
//  setSyncInterval(5000);
//  getTime();
//  digitalClockDisplay();          // display the current date and time
}

void loop(){

  while(!Serial){
    ;
  }
  
//  if (timeStatus() != timeNotSet) {   // check if the time is successfully updated
//    if (now() != prevDisplay) {       // update the display only if time has changed
////      prevDisplay = now();
////      digitalClockDisplay();          // display the current date and time
//    }
//  }
  if (millis()-millis_prev>=1000){
    millis_prev=millis();
    
  }

  if (S1_paso0){
 
  if (digitalRead(Sensor1_pin)==HIGH){
       T_S1_Down2=millis();
      S1_paso0=false;
      S1_paso1=true;  
    //Serial.println("paso 0 ejecutado");
    }
  }
  if (S1_paso1){
    if (digitalRead(Sensor1_pin)==LOW){
      T_S1_Up2=millis();
      S1_paso1=false;
      S1_paso2=true;
    //Serial.println("paso 1 ejecutado");
    }
  }
  if (S1_paso2){
    if (T_S1_Up2-T_S1_Down2>Tiempo_S1){
      estado_S1=1;
      S1_paso2=false;
      S1_paso0=true;
      T_S1_anterior=millis();
      aux_millis_S1=millis();
      contador_S1_ISR++;
    //Serial.println("pieza contada");
    }
    else {
      estado_S1=0;
      S1_paso2=false;
      S1_paso0=true;
      //Serial.println("ruido filtrado");
    }
  }  
//  mqttLoop();
//Transmisión de mensajes para sensor 1  
  if (estado_S1==0 && (millis()-T_S1_anterior>T_S1_inactividad)){
    estado_S1=5;
    T_S1_anterior=millis();
    snprintf(msg_aux,110, "SENSOR/1;CTM_VALOR/%d;TOTAL/%d",estado_S1,contador_S1_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);
    estado_S1=0;

  }
  else if (estado_S1==1){
    snprintf(msg_aux,110, "SENSOR/1;CTM_VALOR/%d;TOTAL/%d",estado_S1,contador_S1_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);
    estado_S1=0;
    contador_S1++;
  }
//Transmisión de mensajes para sensor 2
  if (estado_S2==0 && (millis()-T_S2_anterior>T_S2_inactividad)){
    estado_S2=5;
    T_S2_anterior=millis();
    snprintf(msg_aux,110, "SENSOR/2;CTM_VALOR/%d;TOTAL/%d",estado_S2,contador_S2_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);                            
    estado_S2=0;

  }
  else if (estado_S2==1){
    snprintf(msg_aux,110, "SENSOR/2;CTM_VALOR/%d;TOTAL/%d",estado_S2,contador_S2_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);
    estado_S2=0;
    contador_S2++;
  }

}

//Funciones

void Deteccion_1(){
  T_S1_actual=millis();
  if (T_S1_actual-T_S1_anterior>T_S1){ //si el tiempo entre la lectura actual y la lectura anterior es mayor al tiempo minimo entre productos se contabiliza como valida.
    estado_S1=1;
    T_S1_anterior=T_S1_actual;
    aux_millis_S1=T_S1_actual;
    contador_S1_ISR++;
  }
}
void Deteccion_2(){
  T_S2_actual=millis();
  if (T_S2_actual-T_S2_anterior>T_S2){ //si el tiempo entre la lectura actual y la lectura anterior es mayor al tiempo minimo entre productos se contabiliza como valida.
    estado_S2=1;
    T_S2_anterior=T_S2_actual;
    contador_S2_ISR++;
    aux_millis_S2=T_S2_actual;
  }
}
