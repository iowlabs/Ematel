import mysql.connector


# Machine Settings
COM 		= "/dev/ttyACM0"
ID 			= "MQ1"
TAG_CODE	="30D2B"
MAQ_COD		="7"

mydb = mysql.connector.connect(
	host		="localhost",
	user		="root",
	password	="hola1234",
	database 	= ID
)

mycursor = mydb.cursor()
#mycursor.execute("CREATE TABLE sensors (id INT AUTO_INCREMENT PRIMARY KEY, name VARCHAR(255), value FLOAT, time int UNSIGNED) ")


sql = "INSERT INTO sensors (name, value, time) VALUES (%s, %s, %s)"
val = ("temperatura", 25.5, 1000)
mycursor.execute(sql, val)

mydb.commit()

print(mycursor.rowcount, "record inserted.")
