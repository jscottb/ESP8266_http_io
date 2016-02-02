--   ESP8266 http io server.
--
--   Goal in life:
--     Exposes basic wiring functions: pinMode, digitalRead, digitalWrite,
--     analogRead and analogWrite via a http server. Kind of Rest like, but NOT
--     fully Restful. The returned data is in json format.
--
--   Written By - Scott Beasley 2016.
--   Public domain. Free to use or change. Enjoy :)

-- Your Wifi connection data
local SSID = ""
local SSID_PASSWORD = ""

local function connect (conn, data)
   local req

   conn:on ("receive",
      function (cn, req_data)
         local json_ret = ""
         local _, _, method, path, parms = string.find(req_data, "([A-Z]+) (.+)?(.+) HTTP");

         if (request_is (path, "/pinMode")) then
            json_ret = PinMode (parms)
         elseif (request_is (path, "/digitalRead")) then
            json_ret = DigitalRead (parms)
         elseif (request_is (path, "/digitalWrite")) then
            json_ret = DigitalWrite (parms)
         elseif (request_is (path, "/analogRead")) then
            json_ret = AnalogRead (parms)
         elseif (request_is (path, "/analogWrite")) then
            json_ret = AnalogWrite (parms)
         end

         resp = "HTTP/1.1 200 Ok\r\n"
         resp = resp .. "Content-Type: application/json\r\n"
         resp = resp .. "Access-Control-Allow-Origin: *\r\n"
         resp = resp .. "Connection: close\r\n"
         resp = resp .. "Content-Length: " .. string.len (json_ret)
         resp = resp .. "\r\n\r\n" .. json_ret
         cn:send (resp)

         cn:close ( )
         collectgarbage ( )
      end)
end

function PinMode (parms)
   local pin, val, strt_ndx, end_ndx, mode
   local str_pin = ""

   strt_ndx, end_ndx = string.find (parms, "([^,]+)")
   str_pin = string.sub (parms, strt_ndx, end_ndx)
   pin = ArduinoPin2Node (str_pin)
   if pin == -1 then
      return (pin_error ( ))
   end
   val = string.sub (parms, end_ndx + 2)

   if val == "INPUT" then
      mode = gpio.INPUT
   elseif val == "OUTPUT" then
      mode = gpio.OUTPUT
   elseif val == "INPUT_PULLUP" then
      mode = gpio.INPUT_PULLUP
   end

   gpio.mode (pin, mode)

   return ("{\n\t\"return_code\": 0\n}")
end

function DigitalRead (parms)
   local raw_val = 0
   local pin = -1
   local val = ""
   local ret_json = ""

   pin = ArduinoPin2Node (parms)
   if pin == -1 then
      return (pin_error ( ))
   end
   raw_val = gpio.read (pin)

   if raw_val == 0 then
      val = "LOW"
   else
      val = "HIGH"
   end

   ret_json = "{\n\t\"data_value\": \"\"" .. val .. "\""
   ret_json = ret_json .. ",\n\t\"raw_data\": " .. raw_val
   ret_json = ret_json .. ",\n\t\"return_code\": 0\n}"
   return (ret_json)
end

function DigitalWrite (parms)
   local pin, val, strt_ndx, end_ndx, state
   local str_pin = ""

   strt_ndx, end_ndx = string.find (parms, "([^,]+)")
   str_pin = string.sub (parms, strt_ndx, end_ndx)
   pin = ArduinoPin2Node (str_pin)
   if pin == -1 then
      return (pin_error ( ))
   end
   val = string.sub (parms, end_ndx + 2)

   if val == "HIGH" then
      state = gpio.HIGH
   elseif val == "LOW" then
      state = gpio.LOW
   end

   gpio.write (pin, state)

   return ("{\n\t\"return_code\": 0\n}")
end

function AnalogRead (parms)
   local raw_val = 0
   local ret_json = ""

   raw_val = adc.read (0) -- Only one analog pin on the ESP8266

   ret_json = "{\n\t\"data_value\": " .. raw_val
   ret_json = ret_json .. ",\n\t\"return_code\": 0\n}"
   return (ret_json)
end

function AnalogWrite (parms)
   local pin, val, strt_ndx, end_ndx

   strt_ndx, end_ndx = string.find (parms, "([^,]+)")
   str_pin = string.sub (parms, strt_ndx, end_ndx)
   pin = ArduinoPin2Node (str_pin)
   if pin == -1 then
      return (pin_error ( ))
   end
   val = string.sub (parms, end_ndx + 2)

   -- Add code to setup and start PWM here

   return ("{\n\t\"return_code\": 0\n}")
end

function ArduinoPin2Node (mcupin)
   local pins = {["16"] = 0, ["5"] = 1, ["4"] = 2, ["0"] = 3, ["2"] = 4,
                 ["14"] = 5, ["12"] = 6, ["13"] = 7, ["15"] = 8,
                 ["3"] = 9, ["1"] = 10}
   local ret_val = -1

   for key, val in pairs (pins) do
      if key == mcupin then
         ret_val = val
         break
      end
   end

   return (ret_val)
end

function pin_error ( )
   return ("{\n\t\"return_code\": 1\n,\n\t\"error_txt\": \"Pin not defined\"\n}")
end

function wait_for_wifi_conn ( )
   tmr.alarm (1, 1000, 1, function ( )
      if wifi.sta.getip ( ) == nil then
         print ("Waiting for Wifi connection")
      else
         tmr.stop (1)
         print ("ESP8266 mode is: " .. wifi.getmode ( ))
         print ("The module MAC address is: " .. wifi.ap.getmac ( ))
         print ("Config done, IP is " .. wifi.sta.getip ( ))
      end
   end)
end

function request_is (request, uri)
    if request == uri then
       return (true)
    else
       return (false)
    end
end

-- String trim left and right
function trim (s)
  return (s:gsub ("^%s*(.-)%s*$", "%1"))
end

-- Configure the ESP as a station (client)
wifi.setmode (wifi.STATION)
wifi.sta.config (SSID, SSID_PASSWORD)
wifi.sta.autoconnect (1)

-- Hang out until we get a wifi connection before the httpd server is started.
wait_for_wifi_conn ( )

-- Create the httpd server
svr = net.createServer (net.TCP, 30)

-- Server listening on port 80, call connect function if a request is received
svr:listen (80, connect)
