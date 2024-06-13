import paho.mqtt.client as mqtt
import random, threading, json
from datetime import datetime
import serial
import time

# ====================================================
# PEKEN Settings
import requests

NPID="303"              #ID UNICO
KEY="QL3RJbvwsvAttZnr"  #KEY UNICO

oldTime = time.time()   #TIMER PARA SUBIR DATOS
timeToUpload = 3599     #CADA CUANTO SUBIR LOS DATOS EN SEGUNDOS, DE MOMENTO SE ACEPTA CADA 1 HORA, SE RESTA 1 AL TOTAL DE SEGUNDOS
first_upload = 0        #SUBIR LA PRIMERA VEZ QUE SE EJECUTA EL PROGRAMA Y NO ESPERAR 1 HORA (EL LIMITE DEL SERVIDOR SIGUE SIENDO 1 CADA HORA), 1 ACTIVADO 0 DESACTIVADO
uploadOnData = 1        #SI SE SUBE CADA VEZ QUE SE RECIBEN DATOS, SOBRE ESCRIBE LA CONFIGURACION ANTERIOR, 1 ACTIVADO 0 DESACTIVADO

                        #EL SERVIDOR DEVOLVERA RESPUESTA ANQUE SE HAGAN MAS REQUEST PERO SOLO SE GUARDARAN LOS DATOS CADA 1 HORA

# ====================================================

# ====================================================
# MQTT Settings
MQTT_Broker = "192.168.51.10"
MQTT_Port = 1883
Keep_Alive_Interval = 45
RUTA = "CICCHILLAN/SENSORES/FLUJO"
RUTA2 = "CICCHILLAN/SENSORES/PISTOLA"
client_id="ENCHAPADORA"
# Serial Settings
baud_rate=9600
com_port='COM3'

# Machine Settings
TAG_CODE="30D2B"
MAQ_COD="40"
# ====================================================
def on_log(client, userdata, level, buf):
    print(buf)
def on_connect(client, userdata,flags, rc):
    if rc == 0:
        mqttc.connected_flag=True #SET FLAG
        print("Connected with MQTT Broker: " + str(MQTT_Broker))
    else:
        print("Unable to connect to MQTT Broker...")
        client.loop_stop()

def on_publish(client, userdata, mid):
    pass


def on_disconnect(client, userdata, rc):
    print("Client Disconnected ok")
    pass


mqtt.Client.connected_flag=False #Create flag in class
mqttc = mqtt.Client(client_id)
mqttc.on_log=on_log
mqttc.on_connect = on_connect
mqttc.on_disconnect = on_disconnect
mqttc.on_publish = on_publish
mqttc.connect(MQTT_Broker, MQTT_Port)
mqttc.loop_start()
while not mqttc.connected_flag: #wait in loop
	print("In wait loop")
	time.sleep(1)
print("Servidor Conectado")
time.sleep(3)
print ("publicando")


def publish_To_Topic(topic, message):
    mqttc.publish(topic, message)
    print("Published: " + str(message) + " " + "on MQTT Topic: " + str(topic))
    print("")


# ====================================================
# Serial Settings

try:
    ser = serial.Serial()
    ser.baudrate = baud_rate
    ser.port = com_port
    ser.open()
    # isConnected=ser.is_open()
    print("arduino Conectado")
#	t = threading.Thread(target=self.keepAlive)
#	t.daemon = True	 # Run in background
#	t.start()
except:
    print("arduino No Conectado")

# ====================================================

toggle = 0
contador_sin_mensaje = 0
mqttc.loop_start()
# ====================================================
if __name__ == '__main__':
    while True:  # Event Loop
        ##mqttc.loop()
        ##Comunicación Serial
        ##Comunicación Serial
        time.sleep(0.1)
        if ser.is_open:

            #            if ser.inWaiting() > 0:
            #                time.sleep(1)
            #                for c in ser.read():
            #                    line.append(c)
            try:
                if ser.inWaiting() > 0:
                    rawString = ser.readline()
                    print(rawString)
                    contador_sin_mensaje = 0
                    if rawString[0]==63:
                        if rawString[1]==63:
                            mensaje = "MAQ_COD/" + MAQ_COD + ";TIMESTAMP/" + (datetime.today()).strftime(
                            "%d-%b-%Y %H:%M:%S:%f") + ";" + str(rawString)[4:].replace("\x00","?")
                            publish_To_Topic(RUTA2, mensaje)
                            contador_sin_mensaje = 0
                            # ====================================================
                            if time.time() - oldTime > timeToUpload or first_upload == 0 or uploadOnData == 1:
                                payload = {
                                    'npi': NPID,             
                                    'key': KEY,
                                    'data': [
                                                {
                                                    'cpi': '001',                       #CANAL 001 DE MOMENTO, A FUTURO SE PUEDEN PONER MAS CANALES POR PLACA (MAS SENSORES Y GRAFICOS)
                                                    'value': int(str(rawString)[2:])    #LECTURA DEL SENSOR COMO NUMERO
                                                }	
                                            ]
                                }
                                try:
                                    r = requests.post("https://data-cube-wjeuzuajoa-uc.a.run.app/api/v1/network/ingestion/", json=payload)
                                    print(r.text)                   #RESPUESTA DEL SERVIDOR, CADA VEZ QUE SE HACE UN POST, PERO SOLO GUARDA LOS DATOS CADA 1 HORA
                                    first_upload = 1
                                    oldTime = time.time()
                                except:
                                    print("error al subir a Peken")
                            # ====================================================

                        else:
                            mensaje = "TAG_CODE/" + TAG_CODE + ";MAQ_COD/" + MAQ_COD + ";TIMESTAMP/" + (datetime.today()).strftime(
                            "%d-%b-%Y %H:%M:%S:%f") + ";" + str(rawString)[3:]
                            publish_To_Topic(RUTA, mensaje)
                            contador_sin_mensaje = 0
                else:
                    contador_sin_mensaje += 1
            except:
                #ser.close()
                time.sleep(0.1)
        else:
            try:
                ser.open()
                print("arduino Conectado")
                #time.sleep(1)
            except:
                print("arduino NO Conectado")
                time.sleep(5)
        if contador_sin_mensaje > 600:
            print("Sin Mensaje... Cerrando Comunicación...")
            ser.close()
            time.sleep(1)
            contador_sin_mensaje = 0
