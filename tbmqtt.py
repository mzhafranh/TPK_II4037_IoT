import paho.mqtt.client as mqtt
from tb_device_mqtt import TBDeviceMqttClient

# Define the MQTT broker address and port
mqtt_broker_address = ""
mqtt_broker_port = 1883

# Define the MQTT topics you want to subscribe to
mqtt_topic_1 = "esp32/phKolam"
mqtt_topic_2 = "esp32/sisaAsam"
mqtt_topic_3 = "esp32/sisaBasa"
mqtt_topic_4 = "esp32/avgAsam"
mqtt_topic_5 = "esp32/avgBasa"

# Define ThingsBoard parameters
tb_server = ""
tb_token = ""
tb_device = ""

# Define a callback function to handle incoming MQTT messages
def on_message(client, userdata, message):
    payload = message.payload.decode("utf-8")
    print(f"Received message on topic '{message.topic}': {payload}")
    
    # Send the received value to ThingsBoard
    if (message.topic == "esp32/phKolam"):
        telemetry = {"ph": payload}
    elif (message.topic == "esp32/sisaAsam"):
        telemetry = {"sisaAsam": payload}
    elif (message.topic == "esp32/sisaBasa"):
        telemetry = {"sisaBasa": payload}
    elif (message.topic == "esp32/avgAsam"):
        telemetry = {"avgAsam": payload}
    elif (message.topic == "esp32/avgBasa"):
        telemetry = {"avgBasa": payload}

    # tb_client.send_attributes({tb_device: {message.topic: payload}})
    tb_client.send_telemetry(telemetry)

# Create an MQTT client
mqtt_client = mqtt.Client()
mqtt_client.username_pw_set("pi","raspberry")

# Set the callback function for message reception
mqtt_client.on_message = on_message

# Connect to the MQTT broker
mqtt_client.connect(mqtt_broker_address, mqtt_broker_port)

# Subscribe to the MQTT topic
mqtt_client.subscribe(mqtt_topic_1)
mqtt_client.subscribe(mqtt_topic_2)
mqtt_client.subscribe(mqtt_topic_3)
mqtt_client.subscribe(mqtt_topic_4)
mqtt_client.subscribe(mqtt_topic_5)

# Create a ThingsBoard Gateway client
tb_client = TBDeviceMqttClient("", 1883, tb_token)
tb_client.connect()

print("Starting")
# Start the MQTT client's network loop
mqtt_client.loop_forever()