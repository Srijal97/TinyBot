'''
# Control software for sending commands from the keyboard to the TinyBots using MQTT
#
# Author: Srijal Poojari
'''


import keyboard
import paho.mqtt.client as mqtt


mqtt_broker = '192.168.1.106'  # TODO: Get local IP automatically
mqtt_port = 1883
mqtt_client_name = 'TinyBotController'

mqtt_client = mqtt.Client(mqtt_client_name)

device_id = '001'
key_dict = {
    'space' : '00',  # stop
    'clear' : '00',  # stop (for numpad key '5')
    'up'    : '01',  # forward
    'down'  : '02',  # backward
    'left'  : '03',  # left
    'right' : '04',  # right
}

def connect_mqtt(client, broker_address, port):
    while True:
        try:
            client.connect(broker_address, port=port)
            print('MQTT connected to broker!')
            break
        except ConnectionRefusedError:
            print("MQTT connection refused! Verify that host exists and is open.")
            print('Retrying...')

# Generator function for collecting entered numbers and changing device id when 3 numbers are entered
def change_device_id():
    num = ''
    while True:
        num = num + (yield num)

        if len(num) == 3:
            global device_id
            device_id = num
            print("device id changed to", device_id)
            num = ''

def keyboard_event_callback(keyboardEvent):

    key = keyboardEvent.name
    print(key)

    if key in [str(num) for num in range(10)]:
        device_id_generator.send(key)

    if key in key_dict:
        pub_info = mqtt_client.publish('command', device_id + '-' + key_dict[key])

        if pub_info.rc != mqtt.MQTT_ERR_SUCCESS:
            print('MQTT publish error! MQTTMessageInfo.rc =', pub_info.rc)
            keyboard.unhook_all()
            connect_mqtt(mqtt_client, mqtt_broker, mqtt_port)
            keyboard.on_press(keyboard_event_callback)


device_id_generator = change_device_id()
device_id_generator.send(None)

keyboard.on_press(keyboard_event_callback)
connect_mqtt(mqtt_client, mqtt_broker, mqtt_port)

while True:
    pass