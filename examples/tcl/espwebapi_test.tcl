# ESP8266 http io server test.
#
# Goal in life:
#   Blink an LED every second.
#
# Written By - Scott Beasley 2016.
# Public domain. Free to use or change. Enjoy :)
#

package require http

# Set your device url here
set DEVICE_URL {http://192.168.2.72}

proc PinMode {pin state} {
    global DEVICE_URL

    set token [::http::geturl $DEVICE_URL/pinMode?$pin,$state]
    set ret_json_dict [json2dict [http::data $token]]
    http::cleanup $token
    return [dict get $ret_json_dict {return_code}]
}

proc DigitalRead {pin} {
    global DEVICE_URL

    set token [::http::geturl $DEVICE_URL/digitalRead?$pin]
    set ret_json_dict [json2dict [http::data $token]]
    http::cleanup $token
    return [dict get $ret_json_dict {return_code}]
}

proc DigitalWrite {pin state} {
    global DEVICE_URL

    set token [::http::geturl $DEVICE_URL/digitalWrite?$pin,$state]
    set ret_json_dict [json2dict [http::data $token]]
    http::cleanup $token
    return [dict get $ret_json_dict {return_code}]
}

# From http://wiki.tcl.tk/13419
proc json2dict {JSONtext} {
   string range [
       string trim [string trimleft [
            string map {\t {} \n {} \r {} , { } : { } \[ \{ \] \}} $JSONtext
            ] {\uFEFF}
       ]
   ] 1 end-1
}

# Test it out
PinMode 16 OUTPUT

# Blink the built-in LED on the NodeMcu board 1 per second
while {1} {
   DigitalWrite 16 LOW
   after 1000
   DigitalWrite 16 HIGH
   after 1000
}
