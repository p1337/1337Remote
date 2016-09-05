/*
 * CHANGELOG:
 * 
 * 160709: Serialoutput eingeschaltet, wiederaufnahme Projekt 
 * 
 * 160710: VeryDeepSleep hinzugefügt 
 * 
 * 160720: EEPROM WifiSleep hinzugefügt - ein voller Erfolg!
 *         Performanceverbesserung - von 13 au 5 Sekunden bis Trigger bei Start!
 */

#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266httpUpdate.h>


//AP VARS
int DSState = 0; // 0 = DEEPSLEEP ohne WIFI  -  1 = DEEPSLEEP mit WIFI
const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
const char* ssid = "Remote Setup";
const char* password = "";  // auf "" setzen = kein PW!
WiFiServer server(80);
WiFiClient client;
boolean REMOTESETUP = false;
int counter = 0; //Counter für Verbindungsaufnahme mit Kamera
int nrloops = 0; //loopcounter für deepsleep
String qsid; // VARIABLE FÜR EMPFANG REMOTE SETUP DER SSID
String qpass; // VARIABLE FÜR EMPFANG REMOTE SETUP DER SSID
String rsinfo; // RS INFO FÜR REMOTE SETUP
//VAR CLIENT
boolean willconnect = false;
//VARS TRIGGER
char c;
char recieved;
int camstates;
const char* host = "10.5.5.9";
String response;
boolean pushstate = false;

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssidsav[32] = "";
char passwordsav[32] = "";

//BUTTONS
int BTN_1 = 4;  // REMOTESETUP
int BTN_2 = 14;   // TRIGGER


void setup() 
{
  //DISABLE ACCESSPOINT AT START
  WiFi.disconnect();  
  WiFi.mode(WIFI_STA);

  //Set up RemoteSetupButton Interrupt
  pinMode(BTN_1, INPUT_PULLUP);

  //Set up TriggerButton Interrupt  
  pinMode(BTN_2, INPUT_PULLUP);

   if (digitalRead(BTN_1) == HIGH && digitalRead(BTN_2) == HIGH){  
    deepsleep(); //IS NO BUTTON PRESSED GOTO DEEPSLEEP
   }
   else{              // BUTTON PRESSED - WAKE UP !!!!!
 // // Serial.begin(115200);
     loadDS();                 //load EEPROM STORED DSState

 // // Serial.print("DSState = ");
    // // Serial.println(DSState);

  if (DSState == 0){
    // // Serial.println("REBOOT WIFI ON!");
        DSState = 1;
        saveDS();
        wifisleep();
        } // IF DSSTATE 0
  else {
   
  //   loadCredentials();        //load EEPROM STORED SSID and PW
    if (digitalRead(BTN_2) == LOW){
           
    // // Serial.println("BOOTED WIFI ON!");
       
        delay(100);
        // // Serial.println("Connecting CAM!");
        DSState = 0;
        saveDS();
        // // Serial.println(ssidsav);
        connectWifi();            //Connect to Camera if Trigger is pressed for the first time
            } //BTN 2 
        } // IF DSSTATE 1
    } // ELSE ONE BUTTON PRESSED
} // SETUP


void loop() 
{ ESP.wdtDisable();
 checkifremotesetup();
// Setup? falls ja dann Captive Protal
 loopstodeepsleep(1200); //mit 0.5Sek delay * Anzahl in Funktion (240)

}
