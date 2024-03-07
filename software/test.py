import paho.mqtt.client as mqtt
import mysql.connector
from SerialCom.sc import sc
from datetime import datetime
import time
import json


# Machine Settings
COM = "/dev/ttyACM0"
ID 	= "MQ1"
TAG_CODE	="30D2B"
MAQ_COD		="7"

# MQTT Settings
MQTT_BROKER_1 	= "127.0.0.1"
MQTT_PORT_1 	= 1884
TOPIC_1 		= "CICCHILLAN/SENSORES/FLUJO"

# MQTT Settings
MQTT_BROKER_2 	= "127.0.0.1"
MQTT_PORT_2 	= 1885
TOPIC_2 		= '/'.join([ID,MAQ_COD,TAG_CODE])



# Local Data base
mydb = mysql.connector.connect(
	host		="localhost",
	user		="root",
	password	="hola1234",
	database 	= ID
)

#mycursor.execute("CREATE TABLE sensors (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), value FLOAT, time int UNSIGNED) ")


def on_message(client, userdata, message):
    print("message received " ,str(message.payload.decode("utf-8")))
    #print("message topic=",message.topic)
    #print("message qos=",message.qos)
    #print("message retain flag=",message.retain)



def proccesPayloadCliente(dic):
	temporal = "TAG_CODE/" + TAG_CODE + ";MAQ_COD/" + MAQ_COD + ";"
	temporal += "TIMESTAMP/" + dic["time"]+";"
	for x in dic["data"]:
		temporal += ''.join([x,"/",str(dic["data"][x]),";"])
	return temporal

def processSQL(dic):
	for x in dic["data"]:
		_sql = "INSERT INTO sensors (name, value, time) VALUES (%s, %s, %s)"
		_val = (x, dic["data"][x], time.mktime(datetime.now().timetuple()))
		mycursor.execute(_sql, _val)
		mydb.commit()


class MqttClient:
	def __init__(self, id, broker, port, topic):
		self.MQTT_ID 		= id
		self.MQTT_Broker 	= broker
		self.MQTT_Port		= port
		self.MQTT_pubTopic  	= topic
		self.connected    	= False  #false = not connected,  true = connected
		self.incomming_msg  	= ""
		self.clientMqtt 	= None

		self.test_start_time   = 0
		self.test_actual_time  = 0
		self.test_elapsed_time = 0

		self.actual_value = 0.0
		self.val_mv = 0.0
		self.temp = 0.0

	def connect(self):
		self.clientMqtt 			= mqtt.Client(self.MQTT_ID)
		#self.clientMqtt.on_log 		= on_log
		#self.clientMqtt.on_connect 	= on_connect
		#self.clientMqtt.on_disconnect = on_disconnect
		#self.clientMqtt.on_publish 	= on_publish
		#self.clientMqtt.on_message 	= on_message
		self.clientMqtt.connect(self.MQTT_Broker, self.MQTT_Port)
		time.sleep(3)
		self.clientMqtt.loop_start()
		self.connected = self.clientMqtt.is_connected()
		while not self.connected:
			print(".",end = "")
			time.sleep(1)
			self.connected = self.clientMqtt.is_connected()
		print("")
		print(f"MQTT connected to broker : {self.MQTT_Broker} on port : {self.MQTT_Port}")

	def publishMqtt(self,msg):
		if self.connected:
			self.clientMqtt.publish(self.MQTT_pubTopic, msg)
			print("Published: " + str(msg) +" on MQTT Topic: " + str(self.MQTT_pubTopic))


if __name__ == "__main__":

	count = 0

	c1 = MqttClient(ID,MQTT_BROKER_1,MQTT_PORT_1,TOPIC_1)
	c2 = MqttClient(ID,MQTT_BROKER_2,MQTT_PORT_2,TOPIC_2)

	c1.connect()
	c2.connect()

	nodo = sc(ID,COM)
	nodo.connectSerialPort()

	mycursor = mydb.cursor()


	while True:
		if nodo.msg_flag:
			#new data arrival
			print("new data : ",end = "")
			print(json.dumps(nodo.msg))

			c2.publishMqtt(json.dumps(nodo.msg)) #publish to diagnostic server

			payload = proccesPayloadCliente(nodo.msg) #publish to client
			c1.publishMqtt(payload)

			print("data saved to local SQL. ",end = "")
			#processSQL(nodo.msg)	#save to local
			for x in nodo.msg["data"]:
				_sql = "INSERT INTO sensors (name, value, time) VALUES (%s, %s, %s)"
				_val = (x, nodo.msg["data"][x], time.mktime(datetime.now().timetuple()))
				mycursor.execute(_sql, _val)
				mydb.commit()

			mycursor.execute("SELECT COUNT(*) FROM sensors")

			print(mycursor.fetchone()[0], "record inserted.")

			nodo.msg_flag = False
			print("")

	time.sleep(1)

	nodo.stopRcvr()
	client_2.loop_stop()
	client_1.loop_stop()
