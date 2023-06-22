import paho.mqtt.client as mqtt

# MQTT broker configuration
mqttBroker = "35.244.35.92"
mqttPort = 1883
mqttTopic = "sensor/data"

def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(mqttTopic)
    print("Listening to data .....---.....")
    print("------------------------------------------------")

def on_message(client, userdata, msg):
    print("Received message: " + str(msg.payload))

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

print("Connecting to MQTT broker...")
client.connect(mqttBroker, mqttPort, 60)

client.loop_forever()
