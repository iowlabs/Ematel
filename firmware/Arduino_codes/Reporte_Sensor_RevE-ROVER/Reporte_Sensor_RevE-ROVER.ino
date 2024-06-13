//Integracion Temporis
#include <usbhid.h>
#include <usbhub.h>
#include <hiduniversal.h>
#include <hidboot.h>
#include <SPI.h>

#include <EEPROM.h> // Almacenar datos en la memoria permanente del Arduino
int Adrr = 0; //Necesaria para indicar la posición en memoria

bool primer_char=true;

class MyParser : public HIDReportParser {
  public:
    MyParser();
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
  protected:
    uint8_t KeyToAscii(bool upper, uint8_t mod, uint8_t key);
    virtual void OnKeyScanned(bool upper, uint8_t mod, uint8_t key);
    virtual void OnScanFinished();
};

MyParser::MyParser() {}

void MyParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
  // If error or empty, return
  if (buf[2] == 1 || buf[2] == 0) return;

  for (uint8_t i = 7; i >= 2; i--) {
    // If empty, skip
    if (buf[i] == 0) continue;

    // If enter signal emitted, scan finished
    if (buf[i] == UHS_HID_BOOT_KEY_ENTER) {
      OnScanFinished();
    }

    // If not, continue normally
    else {
      // If bit position not in 2, it's uppercase words
      OnKeyScanned(i > 2, buf, buf[i]);
    }

    return;
  }
}
uint8_t MyParser::KeyToAscii(bool upper, uint8_t mod, uint8_t key) {
  // Letters
  if (VALUE_WITHIN(key, 0x04, 0x1d)) {
    if (upper) return (key - 4 + 'A');
    else return (key - 4 + 'a');
  }

  // Numbers
  else if (VALUE_WITHIN(key, 0x1e, 0x27)) {
    return ((key == UHS_HID_BOOT_KEY_ZERO) ? '0' : key - 0x1e + '1');
  }

  return 0;
}

void MyParser::OnKeyScanned(bool upper, uint8_t mod, uint8_t key) {
  uint8_t ascii = KeyToAscii(upper, mod, key);

  if (primer_char) {
     Serial.print("??");
     primer_char = false;
  }
 
  Serial.print((char)ascii);
}

void MyParser::OnScanFinished() {
  Serial.println("");
  primer_char=true;
}

USB          Usb;
USBHub       Hub(&Usb);
HIDUniversal Hid(&Usb);
MyParser     Parser;
//-------------------------------------------------------------------------

/*
  MQQT ARDUINO - Servidor TEMPORIS
  VITECH
  Autor: Cristian Villaleiva
  Fuente: https://www.youtube.com/watch?v=VSwu-ZYiTxg
  Versión: A
*/

uint32_t T_S1_UP = 100; // [ms] Tiempo para detección de pieza
uint32_t T_S1_DOWN = 15000; // [ms] Tiempo entre piezas
uint32_t T_S2_UP = 100; // [ms] Tiempo para detección de pieza
uint32_t T_S2_DOWN = 15000; // [ms] Tiempo entre piezas
 // INCLUDE
// #include <SPI.h>
// #include <Ethernet.h>
// #include <PubSubClient.h>
// #include <TimeLib.h>
//Definiciones
#define Sensor1_pin 4
#define Sensor2_pin 5
#define Sensor3_pin 2
#define Sensor4_pin 3
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
unsigned long millis_prev2=0;
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

uint32_t T11_S1 = 0;
uint32_t T12_S1 = 0;
uint32_t T21_S1 = 0;
uint32_t T22_S1 = 0;
bool S1_paso0=true;
bool S1_paso1=false;
bool S1_paso2=false;
bool S1_paso3=false;
bool S1_paso4=false;

bool S1_HIGH=false;
bool S1_LOW=false;

uint32_t T11_S2 = 0;
uint32_t T12_S2 = 0;
uint32_t T21_S2 = 0;
uint32_t T22_S2 = 0;
bool S2_paso0=true;
bool S2_paso1=false;
bool S2_paso2=false;
bool S2_paso3=false;
bool S2_paso4=false;

bool S2_HIGH=false;
bool S2_LOW=false;
void setup(){
   Serial.begin(9600);

 //Obtener datos almacenado en memoria estática
   Adrr = 0;
   EEPROM.get( Adrr, contador_S1_ISR ); //Obtener total de piezas producidas.
   delay(10);


   //---------------------------------------------------
//   Integracion TEMPORIS
if (Usb.Init() == -1) {
    Serial.println("OSC did not start.");
  }

  delay( 200 );
  
  Hid.SetReportParser(0, &Parser);
//----------------------------------------------------------

//   Serial.println("inicializando");
//  ethernetSetup();
//  mqttSetup();
  pinMode(Sensor1_pin, INPUT_PULLUP);
  pinMode(Sensor2_pin, INPUT_PULLUP);
  pinMode(Sensor3_pin, INPUT_PULLUP);
  pinMode(Sensor4_pin, INPUT_PULLUP);
  delay(1000);
  noInterrupts();
  //attachInterrupt(digitalPinToInterrupt(Sensor1_pin), Deteccion_1, FALLING); //detectar cambio de estado en entrada (high estado normal)
  //attachInterrupt(digitalPinToInterrupt(Sensor2_pin), Deteccion_2, FALLING); //detectar camhio de estado en entrada (high estado normal)
  delay(1);
  interrupts();

  while(!Serial){
    ;
  }
snprintf(msg_aux,110, "?SENSOR/1;CTM_VALOR/-1;TOTAL/%d",contador_S1_ISR);
Serial.println(msg_aux);
Serial.println("?SENSOR/2;CTM_VALOR/-1;TOTAL/0");
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

  //----------------------------------
//Integracion TEMPORIS
  Usb.Task();
//-----------------------------------

  //SENSOR 1 //Pulsador izquierdo
    if (digitalRead(Sensor1_pin)==LOW && S1_LOW==false){
    millis_prev=millis();
    while(digitalRead(Sensor1_pin)==LOW){
      if (millis()-millis_prev>50){
        S1_HIGH=false;
        S1_LOW=true;
        Serial.println("Estado S1 LOW");
        break;
      }
    }
  }
  
 
 if(digitalRead(Sensor1_pin)==HIGH && S1_HIGH==false){
        S1_HIGH=true;
        S1_LOW=false;
        Serial.println("Estado S1 HIGH");
     }



  //SENSOR 2 //Pulsador Derecho

  if (digitalRead(Sensor2_pin)==LOW && S2_LOW==false){
    millis_prev2=millis();
    while(digitalRead(Sensor2_pin)==LOW){
      if (millis()-millis_prev2>50){
        S2_HIGH=false;
        S2_LOW=true;
        Serial.println("Estado S2 LOW");
        break;
      }
    }
  }
  
  if (digitalRead(Sensor2_pin)==HIGH && S2_HIGH==false){
        S2_HIGH=true;
        S2_LOW=false;
        Serial.println("Estado S2 HIGH");
     }



  // Lado Izquierdo
  if (S1_paso0){
  if (S1_LOW){
    T11_S1=millis();
    S1_paso0=false;
    S1_paso1=true;  
    Serial.println("paso 0 izquierdo");
    }
  }
  if (S1_paso1){
      T12_S1=millis();
      if (T12_S1-T11_S1>T_S1_DOWN){
        S1_paso1=false;
        S1_paso2=true;
       Serial.println("paso 1 izquierdo");
      }
    }
  
  if (S1_paso2){
        Serial.println("pieza contada izquierdo");
        estado_S1=1;
        S1_paso2=false;
        S1_paso0=true;
        T_S1_anterior=millis();
        aux_millis_S1=millis();
        contador_S1_ISR++;
     }
  

 //Lado Derecho
  if (S2_paso0){
  if (S2_LOW){
    T11_S2=millis();
    S2_paso0=false;
    S2_paso1=true;  
    Serial.println("paso 0 derecho");
    }
  }
  if (S2_paso1){
      T12_S2=millis();
      if (T12_S2-T11_S2>T_S2_DOWN){
        S2_paso1=false;
        S2_paso2=true;
       Serial.println("paso 1 derecho");
        
      }
    }
  
  if (S2_paso2){
         Serial.println("pieza contada derecho");
        estado_S1=1;
        S2_paso2=false;
        S2_paso0=true;
        T_S1_anterior=millis();
        aux_millis_S1=millis();
        contador_S1_ISR++;
  
     }
  
 if (contador_S1_ISR>32000 || contador_S1_ISR<0 ) contador_S1_ISR=0;
  if (contador_S2_ISR>32000 || contador_S2_ISR<0 ) contador_S2_ISR=0;
//  mqttLoop();
//Transmisión de mensajes para sensor 1  
  if (estado_S1==0 && (millis()-T_S1_anterior>T_S1_inactividad)){
    estado_S1=-1;
    T_S1_anterior=millis();
    snprintf(msg_aux,110, "?SENSOR/1;CTM_VALOR/%d;TOTAL/%d",estado_S1,contador_S1_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);
    Adrr = 0;
    EEPROM.put( Adrr, contador_S1_ISR ); //Obtener total de piezas producidas.
    estado_S1=0;
    Serial.println("??KEEPALIVE");

  }
  else if (estado_S1==1){
    snprintf(msg_aux,110, "?SENSOR/1;CTM_VALOR/%d;TOTAL/%d",estado_S1,contador_S1_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);
    estado_S1=0;
    contador_S1++;
    Serial.println("??KEEPALIVE");
  }
//Transmisión de mensajes para sensor 2
  if (estado_S2==0 && (millis()-T_S2_anterior>T_S2_inactividad)){
    estado_S2=-1;
    T_S2_anterior=millis();
    snprintf(msg_aux,110, "?SENSOR/2;CTM_VALOR/%d;TOTAL/%d",estado_S2,contador_S2_ISR);
    //publish(msg_aux);
    Serial.println(msg_aux);                            
    estado_S2=0;

  }
  else if (estado_S2==1){
    snprintf(msg_aux,110, "?SENSOR/2;CTM_VALOR/%d;TOTAL/%d",estado_S2,contador_S2_ISR);
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
