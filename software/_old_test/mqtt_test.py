import paho.mqtt.client as mqtt
import random, threading, json
from datetime import datetime
import serial
import time

# ====================================================
# MQTT Settings
MQTT_Broker = "192.168.51.10"
MQTT_Port = 1883
Keep_Alive_Interval = 45
RUTA = "CICCHILLAN/SENSORES/FLUJO"
client_id="TECHNO"
# Serial Settings
baud_rate=9600
com_port='COM3'

# Machine Settings
TAG_CODE="30D2B"
MAQ_COD="7"

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
        time.sleep(1)
        if ser.is_open:

            #            if ser.inWaiting() > 0:
            #                time.sleep(1)
            #                for c in ser.read():
            #                    line.append(c)
            try:
                if ser.inWaiting() > 0:
                    rawString = ser.readline()
                    print(rawString)
                    mensaje = "TAG_CODE/" + TAG_CODE + ";MAQ_COD/" + MAQ_COD + ";TIMESTAMP/" + (datetime.today()).strftime(
                        "%d-%b-%Y %H:%M:%S:%f") + ";" + str(rawString)[2:]
                    publish_To_Topic(RUTA, mensaje)
                    contador_sin_mensaje = 0
                else:
                    contador_sin_mensaje += 1
            except:
                ser.close()
                time.sleep(5)
        else:
            try:
                ser.open()
                print("arduino Conectado")
                time.sleep(1)
            except:
                print("arduino NO Conectado")
                time.sleep(5)
        if contador_sin_mensaje > 60:
            print("Sin Mensaje... Cerrando Comunicación...")
            ser.close()
            time.sleep(1)
            contador_sin_mensaje = 0
