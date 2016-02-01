/*
   ESP8266 http io server.

   Goal in life:
     Exposes basic wiring functions: pinMode, digitalRead, digitalWrite,
     analogRead and analogWrite via a http server. Kind of Rest like, but NOT
     fully Restful. The returned data is in json format.

   Written By - Scott Beasley 2016.
   Public domain. Free to use or change. Enjoy :)
*/

#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <Servo.h>
#include <string.h>
#include <ctype.h>

// Prototype, each and every day...
bool request_is (String, String);
char * getstrfld (char *strbuf, int fldno, int ofset, char *sep, char *retstr);
void serverloop (void);
char * strpbrk (const char *s1, const char *s2);

// Exposed functions
void DigitalRead (char *parms);
void DigitalWrite (char *parms);
void PinMode (char *parms);
void AnalogRead (char *parms);
void AnalogWrite (char *parms);
void ServoOpen (char *parms);
void ServoClose (char *parms);
void ServoSet (char *parms);
void ServoRead (char *parms);

// Define your Wifi info here
#define SSID "YOUR SSID"
#define PASSWD "YOUR PASSWD"

#define BOARDNAME "espio_01"

// Define to get extra info from the Serial port
//#define DEBUG

// Globals
WiFiServer server (80);
// multicast DNS responder
MDNSResponder mdns;
char json_ret[256]; // Increase if you need more than 255 bytes returned.
// Allow for at least 8 servos.
struct usrservos {
   Servo servo;
   int pin;
} userservos[8];

int servosinuse = 0;

void setup ( )
{
    Serial.begin (9600);
    delay (10);

    servosinuse = 0;

    // Mark all servo slots as free on start.
    for (int i = 0; i < 8; i++) {
        userservos[i].pin = -1;
    }

    // Connect to the WiFi network
    Serial.print ("\nConnecting to: ");
    Serial.println (SSID);

    WiFi.begin (SSID, PASSWD);

    while (WiFi.status ( ) != WL_CONNECTED) {
       delay (500);
       Serial.print (".");
    }

    Serial.println ("\nWiFi connected");

    // Set up mDNS responder:
    // - first argument is the domain name, in this example
    //   the fully-qualified domain name is "esp8266.local"
    // - second argument is the IP address to advertise
    //   we send our IP address on the WiFi network
    if (!mdns.begin (BOARDNAME, WiFi.localIP ( ))) {
      Serial.println ("Error setting up MDNS responder!");
      while (1) {
         delay (500);
      }
    }
    Serial.println ("mDNS responder started");

    // Start the tcp listener
    server.begin ( );

    // Print the IP address
    Serial.print ("Use this URL to connect: ");
    Serial.print ("http://");
    Serial.print (WiFi.localIP());
    Serial.println ("/");
}

void loop ( )
{
    serverloop ( );
}

void serverloop ( )
{
   char req_parms[20], *req_chararray;

   // Look for connections
   WiFiClient client = server.available ( );
   if (!client) {
      return;
   }

   // Wait for the request data
   #ifdef DEBUG
      Serial.println ("New connection");
   #endif
   while (!client.available ( )) {
      delay (1);
   }

   // Read the request data (first line only for our needs)
   String request = client.readStringUntil ('\n');
   client.flush ( ); // Clear anything left in the request.

   if (request_is (request, "/favicon.ico")) {
      client.println ("HTTP/1.1 200 OK");
      client.println ("Content-Type: image/x-icon");
      client.print ("Content-Length: 0");
      client.println ("Connection: close");
      client.println("");
      return;
   }

   #ifdef DEBUG
      Serial.println (request);
   #endif

   if (request_is (request, "/digitalRead")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      DigitalRead (req_parms);
   }  else if (request_is (request, "/digitalWrite")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
         Serial.println (req_parms);
      #endif
      DigitalWrite (req_parms);
   }  else if (request_is (request, "/pinMode")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      PinMode (req_parms);
   }  else if (request_is (request, "/analogRead")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      AnalogRead (req_parms);
   }  else if (request_is (request, "/analogWrite")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      AnalogWrite (req_parms);
   }  else if (request_is (request, "/servoOpen")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      ServoOpen (req_parms);
   }  else if (request_is (request, "/servoClose")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      ServoClose (req_parms);
   }  else if (request_is (request, "/servoRead")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      ServoRead (req_parms);
   }  else if (request_is (request, "/servoWrite")) {
      // Make a temp array to hold the
      req_chararray = (char *) malloc (request.length ( ) + 1);
      request.toCharArray (req_chararray, request.length ( ));
      getstrfld (req_chararray, 2, 0, (char *)" ?", req_parms);
      free (req_chararray);
      #ifdef DEBUG
          Serial.println (req_parms);
      #endif
      ServoSet (req_parms);
   }

   // Return an HTTP 200 Ok
   client.println ("HTTP/1.1 200 Ok");
   client.println ("Content-Type: application/json");
   client.println ("Access-Control-Allow-Origin: *");
   client.println ("Connection: close");
   client.print ("Content-Length: ");
   client.println (strlen (json_ret));
   client.println ("");
   client.println (json_ret);
   client.flush ( );
   client.stop ( );
   #ifdef DEBUG
       Serial.println (json_ret);
   #endif

   delay (5); // Dwell a bit before next request.
   #ifdef DEBUG
       Serial.println ("Client disconnected\n");
   #endif
}

/*
   Exposed controller functions.
*/
void DigitalRead (char *parms)
{
   int pinNumber;
   pinNumber = atoi (parms);
   int raw_val = 0;

   raw_val = digitalRead (pinNumber);
   sprintf (json_ret, "{\n\t\"data_value\": \"%s\",\n\t\"raw_data\": %d,\n\t\"return_code\": 0\n}",
          (raw_val == HIGH ? "HIGH" : "LOW"), raw_val);
}

void DigitalWrite (char *parms)
{
   bool value = 0;
   int pinNumber;
   char pinStr[3];
   char valueStr[6];
   int return_code = 0;

   getstrfld (parms, 0, 0, (char *)",", pinStr);
   getstrfld (parms, 1, 0, (char *)",", valueStr);
   pinNumber = atoi (pinStr);

   if (!strcmp (valueStr, "HIGH")) value = 1;
   else if (!strcmp (valueStr, "LOW")) value = 0;
   else return_code = 1;

   if (!return_code) {
      digitalWrite (pinNumber, value);
   }

   sprintf (json_ret, "{\n\t\"return_code\": %d\n}", return_code);
}

void PinMode (char *parms)
{
   bool value = 0;
   int pinNumber;
   char pinStr[3];
   char valueStr[17];
   int return_code = 0;

   getstrfld (parms, 0, 0, (char *)",", pinStr);
   getstrfld (parms, 1, 0, (char *)",", valueStr);
   pinNumber = atoi (pinStr);

   if (!strcmp (valueStr, "INPUT")) value = INPUT;
   else if (!strcmp (valueStr, "OUTPUT")) value = OUTPUT;
   else if (!strcmp (valueStr, "INPUT_PULLUP")) value = INPUT_PULLUP;
   else return_code = 1;

   if (!return_code) {
      pinMode (pinNumber, value);
   }

   sprintf (json_ret, "{\n\t\"return_code\": %d\n}", return_code);
}

void AnalogRead (char *parms)
{
   int pinNumber;
   pinNumber = atoi (parms); // Reads pin num, but ignores for now.
   int raw_val = 0;

   raw_val = analogRead (A0); // Only one analog pin on the ESP now.
   sprintf (json_ret, "{\n\t\"data_value\": \"%d\",\n\t\"return_code\": 0\n}",
            raw_val);
}

void AnalogWrite (char *parms)
{
   bool value = 0;
   int pinNumber;
   char pinStr[3];
   char valueStr[7];
   int return_code = 0;

   getstrfld (parms, 0, 0, (char *)",", pinStr);
   getstrfld (parms, 1, 0, (char *)",", valueStr);
   pinNumber = atoi (pinStr);
   value = atoi (valueStr);

   if (!return_code) {
      analogWrite (pinNumber, value);
   }

   sprintf (json_ret, "{\n\t\"return_code\": %d\n}", return_code);
}

void ServoOpen (char *parms)
{
   int i;
   int pinNumber;
   int return_code = 0;

   pinNumber = atoi (parms);

   servosinuse++;
   if (servosinuse >= 8) {
      return_code = 1; // All servos inuse.
   } else {
      for (i = 0; i < 8; i++) {
         if (userservos[i].pin == -1) {
            // Zap any old attachment.
            userservos[i].servo.detach ( );
            userservos[i].pin = pinNumber;
            userservos[i].servo.attach (pinNumber);
            return_code = 0;
            break;
         }
      }
   }

   // Return the servo array element index used.
   sprintf (json_ret, "{\n\t\"data_value\": \"%d\",\n\t\"return_code\": %d\n}",
            i, return_code);
}

void ServoClose (char *parms)
{
   int indexNumber;
   int return_code = 0;

   indexNumber = atoi (parms);

   if (indexNumber < 0 || indexNumber > 8) {
      return_code = 1;
   } else {
      userservos[indexNumber].pin = -1;
      servosinuse--;
      userservos[indexNumber].servo.detach ( );
   }

   sprintf (json_ret, "{\n\t\"return_code\": %d\n}", return_code);
}

void ServoSet (char *parms)
{
   int indexNumber, value;
   int return_code = 0;
   char indexStr[3];
   char valueStr[6];

   getstrfld (parms, 0, 0, (char *)",", indexStr);
   getstrfld (parms, 1, 0, (char *)",", valueStr);
   indexNumber = atoi (indexStr);
   value = atoi (valueStr);

   if (userservos[indexNumber].pin != -1)
      if (value > 180) // Write MS to servo
         userservos[indexNumber].servo.writeMicroseconds (value);
      else // Write angle to servo
         userservos[indexNumber].servo.write (value);
   else
      return_code = 1;

   sprintf (json_ret, "{\n\t\"return_code\": %d\n}", return_code);
}

void ServoRead (char *parms)
{
   int indexNumber, return_code = 0, raw_val = 0;
   char indexStr[3];

   getstrfld (parms, 0, 0, (char *)",", indexStr);
   indexNumber = atoi (indexStr);

   if (userservos[indexNumber].pin != -1)
      raw_val = (userservos[indexNumber].servo.read ( ));
   else
      return_code = 1;

   sprintf (json_ret, "{\n\t\"data_value\": \"%d\",\n\t\"return_code\": 0\n}",
            raw_val);
}

bool request_is (String request, String reqdata)
{
   return (request.indexOf (reqdata) != -1 ? 1 : 0);
}

// My old stand-by to break delimited strings up.
char * getstrfld (char *strbuf, int fldno, int ofset, char *sep, char *retstr)
{
   char *offset, *strptr;
   int curfld;

   offset = strptr = (char *)NULL;
   curfld = 0;

   strbuf += ofset;

   while (*strbuf) {
       strptr = !offset ? strbuf : offset;
       offset = strpbrk ((!offset ? strbuf : offset), sep);

       if (offset) {
          offset++;
       } else if (curfld != fldno) {
          *retstr = 0;
          break;
       }

       if (curfld == fldno) {
           strncpy (retstr, strptr,
              (int)(!offset ? strlen (strptr)+ 1 :
              (int)(offset - strptr)));
           if (offset)
              retstr[offset - strptr - 1] = 0;

           break;
       }

       curfld++;
   }

   return retstr;
}

// Included because strpbrk is not in the esp arduino gcc/g++ libraries
// Or I just could not find it :)
char * strpbrk (const char *s1, const char *s2)
{
   const  char *c = s2;
   if (!*s1) {
      return (char *) NULL;
   }

   while (*s1) {
      for (c = s2; *c; c++) {
          if (*s1 == *c)
              break;
      }
      if (*c)
          break;
      s1++;
   }

   if (*c == '\0')
      s1 = NULL;

   return (char *) s1;
}
