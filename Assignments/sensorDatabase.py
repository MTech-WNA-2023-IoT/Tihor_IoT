import paho.mqtt.client as paho

#pip3 install paho-mqtt
global mqttclient;
global broker;
global port;


broker = "34.93.150.210";
port = 1883;

client_uniq = "pubclient_123"

mqttclient = paho.Client(client_uniq, True) 
def storeData(mesg):
  #Create a connection to MySQL Database 
  conn =pymysql.connect(database="senorData",user="user",password="pass123",host="localhost")

  #Create a MySQL Cursor to that executes the SQLs
  cur=conn.cursor()
  #Create a dictonary containing the fields, name, age and place
  data={'topic':'IOT/test','data':mesg}
  #Execute the SQL to write data to the database
  cur.execute("INSERT INTO MQTTdata(topic , data)VALUES(%(topic)s,%(data)s);",data)
  print("Data added")
  #Close the cursor
  cur.close()
  #Commit the data to the database
  conn.commit()
  #Close the connection to the database
  conn.close()

def test(client, userdata, message):
  print("client:"+ str(client))
  print("userdata:"+ str(userdata))
  print("message:"+ str(message.payload))
  mesg = str(message.payload)
  storeData(mesg)


def _on_message(client, userdata, msg):
# 	print("Received: Topic: %s Body: %s", msg.topic, msg.payload)
	print(msg.topic+" "+str(msg.payload))
	 
#Subscribed Topics 
def _on_connect(mqttclient, userdata, flags, rc):
# 	print("New Client: "+str(mqttclient)+ " connected")
# 	print(rc)
	mqttclient.subscribe("IOT/#", qos=0)	
  
mqttclient.message_callback_add("IOT/test", test)

mqttclient.connect(broker, port, keepalive=1, bind_address="")
  
mqttclient.on_connect = _on_connect

mqttclient.loop_forever()
