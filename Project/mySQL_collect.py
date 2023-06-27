import paho.mqtt.client as mqtt
import json
import pymysql

# MQTT broker configuration
mqttBroker = "34.100.247.89"
mqttPort = 1883
mqttTopic = "sensor/data"

# MySQL database configuration
mysqlHost = "localhost"
mysqlUser = "user"
mysqlPassword = "pass123"
mysqlDatabase = "senorData"

# Connect to the MySQL database
db = pymysql.connect(
    host=mysqlHost,
    user=mysqlUser,
    password=mysqlPassword,
    database=mysqlDatabase
)

# Create a cursor object to interact with the database
cursor = db.cursor()

# Define the on_connect callback function
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(mqttTopic)
    print("Listening to data .....---.....")
    print("------------------------------------------------")

# Define the on_message callback function
def on_message(client, userdata, msg):
    try:
        # Decode the received JSON data
        data = json.loads(msg.payload)
        
        # Extract the required values from the JSON data
        acceleration_x = data["acceleration"]["x"]
        acceleration_y = data["acceleration"]["y"]
        acceleration_z = data["acceleration"]["z"]
        gyroscope_x = data["gyroscope"]["x"]
        gyroscope_y = data["gyroscope"]["y"]
        gyroscope_z = data["gyroscope"]["z"]
        ppm = data["ppm"]
        flame_value = data["flameValue"]

        # Print the values in a formatted form
        print("Sensor Data:")
        print("-------------")
        print(f"Acceleration X: {acceleration_x}")
        print(f"Acceleration Y: {acceleration_y}")
        print(f"Acceleration Z: {acceleration_z}")
        print(f"Gyroscope X: {gyroscope_x}")
        print(f"Gyroscope Y: {gyroscope_y}")
        print(f"Gyroscope Z: {gyroscope_z}")
        print(f"PPM: {ppm}")
        print(f"Flame Value: {flame_value}")
        print()

        # Check if the extracted values are not empty
        if acceleration_x or acceleration_y or acceleration_z or gyroscope_x or gyroscope_y or gyroscope_z or ppm or flame_value:
            # Store the values in the MySQL database
            insert_query = "INSERT INTO building_sensor_data (acceleration_x, acceleration_y, acceleration_z, gyroscope_x, gyroscope_y, gyroscope_z, ppm, flame_value) VALUES (%s, %s, %s, %s, %s, %s, %s, %s)"
            values = (acceleration_x, acceleration_y, acceleration_z, gyroscope_x, gyroscope_y, gyroscope_z, ppm, flame_value)
            cursor.execute(insert_query, values)
            cursor.close()
            db.commit()
            db.close()
            print("Data stored in MySQL database")
        else:
            print("One or more values are empty. Skipping database insertion.")

    except Exception as e:
        print("Error occurred while processing and storing the data:", str(e))


    

# Create an MQTT client instance and assign the callback functions
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

# Connect to the MQTT broker and start the MQTT client loop
print("Connecting to MQTT broker...")
client.connect(mqttBroker, mqttPort, 60)
client.loop_forever()
