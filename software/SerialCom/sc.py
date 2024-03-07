# -*- coding: utf-8 -*-
import time
from datetime import datetime
import serial
import json
import threading
import sys, getopt
import re


class sc:
	def __init__(self,id,com):
		self.id  			=  id
		self.debug 			= False
		self.port_name 		= com
		self.baud_rate 		= 9600
		self.arduino_com  	= None    #serial com
		self.arduino_thread = None
		self.connected    	= False  #false = not connected,  true = connected
		self.incomming_msg  = ""

		self.test_start_time   = 0
		self.test_actual_time  = 0
		self.test_elapsed_time = 0

		self.val_mv 	= 0.0
		self.temp 		= 0.0
		self.msg 		= ""
		self.msg_flag 	= False

	def printStatus(self):
		print(f'{self.id},{self.port_name},{self.connected}')

	def getmV(self):
		return self.val_mv

	def getTemp(self):
		return self.temp

	def connectSerialPort(self):
		try:
			self.arduino_com = serial.Serial(self.port_name,self.baud_rate,timeout=0)
			self.connected = True
			self.arduino_thread = threading.Thread(target = self.arduinoRCV,)
			self.arduino_thread.deamon = True
			self.arduino_thread.start()
			self.arduino_thread.join(0.1)
			print(f"station  {self.id} connected to port {self.port_name}")
		except Exception as e:
			self.connected = False
			print(f"failed to connect {self.id} to port {self.port_name}")
			print(e)

	def disconnectSerialPort(self):
		try:
			self.arduino_com.close()
			self.connected = False
		except Exception as e:
			print("Not COM available")

	def sendMsg(self, cmd, arg):
		msg = json.dumps({"cmd":cmd,"arg":arg})
		print(f'sending msg :{msg}')
		if self.arduino_com.isOpen():
			self.arduino_com.write(msg.encode('ascii'))
			self.arduino_com.flush()

	def processFormat(self, strmsg)
		_d = {}
		_str = strmsg.strip(";")
		_list = str.split(";")
		for element in _list:
			_temp = element.split("/")
			_d.update({_temp[0]:_temp[1]})
		return _d


	def arduinoRCV(self):
		while self.connected:
			if self.arduino_com.isOpen():
				if self.arduino_com.inWaiting() > 0:
					self.incomming_msg += self.arduino_com.readline().decode("utf-8")
					try:
						#if the data comes on json format use this:
						data =  json.loads(self.incomming_msg)
						#else try this:
						#data = self.processFormat(self.incoming_msg)
						self.incomming_msg =""
						#print(f"data recived : {data}")
						self.upDateValues(data)
					except json.JSONDecodeError:
						pass
				else:
					pass

	def upDateValues(self,d):
		self.msg = d.copy()
		self.msg.update({"time":(datetime.today()).strftime("%d-%b-%Y %H:%M:%S:%f")})#add time
		self.msg_flag = True

		#self.val_mv = d["mv"]
        #self.actual_value = d["val"]
        #self.temp = d["temp"]
        ##print(f"new values updateds {self.val_mv},{self.actual_value},{self.temp}")

	def stopRcvr(self):
		print("Closing")
		self.connected = False
		self.arduino_com.close()

if __name__ == "__main__":
    COM = "/dev/ttyACM0"

    try:
        opts,args = getopt.getopt(sys.argv[1:],"hp:i:")
    except:
        print("Comando no valido pruebe -h por ayuda")
        sys.exit(2)

    for opt ,arg in opts:
        if opt == '-h':
            print("main.py -h for help -p <com> for port to connect")
            sys.exit()
        elif opt =='-p':
            COM = arg
            print(f"trying to connect to port {COM}")
        elif opt == '-i':
            print("option i")

    nodo = sc("b1",COM)
    nodo.connectSerialPort()
    time.sleep(30)
    nodo.stopRcvr()
