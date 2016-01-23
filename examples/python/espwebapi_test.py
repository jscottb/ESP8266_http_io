#   ESP8266 http io server test.
#
#   Gola in life:
#     Tests exposed basic wiring functions: pinMode, digitalRead, digitalWrite,
#     analogRead and analogWrite from the ESP8266 http server.
#
#   Written By - Scott Beasley 2016.
#   Public domain. Free to use or change. Enjoy :)
#
import requests
import time

DEVICE_URL = 'http://192.168.2.8' # IP of your ESP

def digitalWrite (pin, state):
    r = requests.get (DEVICE_URL+'/digitalWrite', params=pin+','+state,
                      stream=False,
                      headers={'Connection':'close','keep_alive': 'False'})
    ret_json = r.json ( )
    r.connection.close()
    return_val = ret_json['return_code']
    return int(return_val)

def digitalRead (pin):
    r = requests.get (DEVICE_URL+'/digitalRead', params=pin, stream=False,
                      headers={'Connection':'close','keep_alive': 'False'})
    ret_json = r.json ( )
    return_val = ret_json['data_value']
    return return_val

def pinMode (pin, state):
    r = requests.get (DEVICE_URL+'/pinMode', params=pin+','+state,
                      stream=False,
                      headers={'Connection':'close','keep_alive': 'False'})
    ret_json = r.json ( )
    r.connection.close()
    return_val = ret_json['return_code']
    return int(return_val)

def analogWrite (pin, state):
    r = requests.get (DEVICE_URL+'/analogWrite', params=pin+','+state,
                      stream=False,
                      headers={'Connection':'close','keep_alive': 'False'})
    ret_json = r.json ( )
    r.connection.close()
    return_val = ret_json['return_code']
    return int(return_val)

def analogRead (pin):
    r = requests.get (DEVICE_URL+'/analogRead', params=pin, stream=False,
                      headers={'Connection':'close','keep_alive': 'False'})
    ret_json = r.json ( )
    return_val = ret_json['data_value']
    return int(return_val)

def main ( ):
    pinMode ("3", "INPUT_PULLUP")
    pinMode ("16", "OUTPUT") # on NODEMCU has an LED on it

    while 1:
        digitalWrite ("16", "LOW")
        time.sleep (1)
        digitalWrite ("16", "HIGH")
        print "A0 = " + str (analogRead ("0"))
        print "D3 = " + digitalRead ("3")
    end

if __name__ == "__main__":
    main ( )
