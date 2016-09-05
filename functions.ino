void loopstodeepsleep(int loops){
//  //  // Serial.print ("LOOPING-MAIN-LOOP ");
// //  // Serial.print (nrloops);
// //  // Serial.print (" of ");
// //  // Serial.println (loops);

  nrloops++;
   if (digitalRead(BTN_2) == LOW && pushstate == false){            // TRIGGER
    pushstate = true;
    trigger();
    
    }

    if (digitalRead(BTN_1) == LOW && pushstate == false){              // REMOTE SETUP
   
    remotesetup();
    
    }
    
  delay(100);
  if (nrloops >= loops) {       
    deepsleep();
    }
  }

String urldecode(const char *src){ //fix encoding
      String decoded = "";
        char a, b;
        
      while (*src) {     
        if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b))) {      
          if (a >= 'a')
            a -= 'a'-'A';       
          if (a >= 'A')                
            a -= ('A' - 10);                   
          else               
            a -= '0';                  
          if (b >= 'a')                
            b -= 'a'-'A';           
          if (b >= 'A')                
            b -= ('A' - 10);            
          else                
            b -= '0';                        
          decoded += char(16*a+b);            
          src+=3;        
        } 
        else if (*src == '+') {
          decoded += ' ';           
          *src++;       
        }  
        else {
          decoded += *src;           
          *src++;        
        }    
      }
      decoded += '\0';        
      return decoded;
    }


void deepsleep(){

//VeryDeepSleep  START  hinzugefügt am 160710
WiFi.disconnect(); 
WiFi.mode(WIFI_OFF);
WiFi.forceSleepBegin();
delay(1);
//ESP.deepSleep(5000 * 1000, WAKE_RF_DISABLED); // 5SEK
ESP.deepSleep(1000 * 1000, WAKE_RF_DISABLED); // 1SEK
//VeryDeepSleep  ENDE  hinzugefügt am 160710


}

void wifisleep(){

delay(1);
ESP.deepSleep(1, WAKE_NO_RFCAL);


}

void remoteupdate(){
   
        //  // Serial.println("RemoteUpdate");
 t_httpUpdate_return ret = ESPhttpUpdate.update("p1337.synology.me", 80, "/remoteupdate/update.php", "160710");
switch(ret) {
    case HTTP_UPDATE_FAILED:
        //  // Serial.println("[update] Update failed.");
        break;
    case HTTP_UPDATE_NO_UPDATES:
        //  // Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
       //  // Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
 }
  }



/** Is this an IP? */
boolean isIp(String str) {
  for (int i = 0; i < str.length(); i++) {
    int c = str.charAt(i);
    if (c != '.' && (c < '0' || c > '9')) {
      return false;
    }
  }
  return true;
}




/** IP to String? */
String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}


/** Load WLAN credentials from EEPROM */
void loadCredentials() {
  EEPROM.begin(512);
  EEPROM.get(0, ssidsav);
  EEPROM.get(0+sizeof(ssidsav), passwordsav);
  char ok[2+1];
  EEPROM.get(0+sizeof(ssidsav)+sizeof(passwordsav), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    ssidsav[0] = 0;
    passwordsav[0] = 0;
  }
   // Serial.println("Recovered credentials:");
   // Serial.println(ssidsav);
   // Serial.println(strlen(passwordsav)>0?"********":"<no password>");
}

/** Store WLAN credentials to EEPROM */
void saveCredentials() {
  EEPROM.begin(512);
  EEPROM.put(0, ssidsav);
  EEPROM.put(0+sizeof(ssidsav), passwordsav);
  char ok[2+1] = "OK";
  EEPROM.put(0+sizeof(ssidsav)+sizeof(passwordsav), ok);
  EEPROM.commit();
  EEPROM.end();
  delay(500);
  deepsleep();  // NACH DEM SPEICHERN IN WARTESCHLEIFE ZURÜCK
}

/** Store WLAN credentials to EEPROM */
void saveDS() {
  EEPROM.begin(512);
  EEPROM.put(300, DSState);
  char ok[2+1] = "OK";
  EEPROM.put(300+sizeof(DSState), ok);
  EEPROM.commit();
  EEPROM.end();
}

void loadDS() {
  EEPROM.begin(512);
  EEPROM.get(300, DSState);
  char ok[2+1];
  EEPROM.get(300+sizeof(DSState), ok);
  EEPROM.end();
  if (String(ok) != String("OK")) {
    DSState = 0;
  }
  // // Serial.println("");
  // // Serial.println("DSState: ");
  // Serial.println(DSState);
}

void remotesetup(){              // STARTING ACCESS POINT FOR REMOTE SETUP
  REMOTESETUP = true;
  dnsServer.start(DNS_PORT, "*", apIP);
 // //  // Serial.println("Setting up AP: ");
 // //  // Serial.println(ssid);
   WiFi.mode(WIFI_AP); 
  WiFi.softAP(ssid);
 //  // Serial.println("RemoteSetup started");
  server.begin();
}                                // STARTING ACCESS POINT FOR REMOTE SETUP



void trigger(){                  // TRIGGER START
  
//  // Serial.println("Trigger set");
nrloops = 0;
//  // Serial.println("Reset nrloops");
 
    WiFiClient client;
      const int httpPort = 80;
  //  // Serial.println(host);
pushstate = false;
      if (!client.connect(host, httpPort)) {
        //delay(100);                              //   ----RSDELAY
        //  // Serial.println("GoPro failed - Status");
       remoteupdate(); //-------------------------------------------------------------------------------REMOTEUPDATE!
        
        return;
      }

String url = "/gp/gpControl/status";
  
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");

delay(200);                         //  ----RSDELAY
   response = "a";
    while (client.available()) {
    c = client.read();
    response += c;

  }
  
String searchtext = response.substring(156);
// //  // Serial.println(searchtext);
//"2": RECORING STATUS ---------------------------START-----------------------------Recording STATUS
int batindex = searchtext.indexOf(':',42);
String bat = searchtext.substring(batindex+1, batindex+2);
////  // Serial.println(bat);
           if (bat == "0") {  camstates = 1; }
           if (bat == "1") {  camstates = 0; }

     
   if (bat == "0") {  
    
// delay(500);  // ----RSDELAYMAYBE
     if (!client.connect(host, httpPort)) {
  //      //  // Serial.println("connection failed - BEEPON");
        return;
      }
    //  //  // Serial.println("Töne werden eingeschaltet");
    url = "/gp/gpControl/setting/56/0";
        client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
     
  }

 delay(500);  // ----RSDELAYMAYBE
     if (!client.connect(host, httpPort)) {
        
      //  //  // Serial.println("connection failed - TRIGGER");
        return;
      }
     //   //  // Serial.println("TRIGGER!");
      url = "/gp/gpControl/command/shutter?p=";
      url += camstates;
     client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");


       if (bat == "1")  {
        
 delay(3000);  //----RSDELAYMAYBE
              if (!client.connect(host, httpPort)) {
       // //  // Serial.println("connection failed - BEEP OF");
        
      }
       // //  // Serial.println("Töne werden ausgeschaltet");
        url = "/gp/gpControl/setting/56/2";
       client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                   "Host: " + host + "\r\n" +
                   "Connection: close\r\n\r\n");
  }
// delay(1000); // ----RSDELAYMAYBE
}                                // TRIGGER END


void connectWifi() {
  int counter = 0;
 // delay(100);           //   ----RSDELAYCONN   
  loadCredentials();   
  
  // delay(100);           //   ----RSDELAYCONN 


 WiFi.disconnect();
 WiFi.mode(WIFI_STA);
  WiFi.begin ( ssidsav, passwordsav );
  
  WiFi.mode(WIFI_STA);
  
  //    // Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
  
//  //  // Serial.println("WHILE-SCHLEIFE");
  //  // Serial.print("Wifi-Status ");
  //  // Serial.println(WiFi.status());  
    delay(200);
      // Serial.print(".");
   counter++;
    if (counter >= 120) {
      counter = 0; //sichtheits halber
 deepsleep();
   } 
  } 
  int connRes = WiFi.waitForConnectResult();
  //  // Serial.println(WiFi.localIP());
}

void checkifremotesetup(){           // CHECKIFREMOTESETUP (FOR LOOP)
  
   if (REMOTESETUP == true){         // ONLY IF REMOTESETUP
 dnsServer.processNextRequest();
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }
  unsigned long ultimeout = millis()+250;
  while(!client.available() && (millis()<ultimeout) )
  {
    delay(1);
  }
  if(millis()>ultimeout) 
  { 
    return; 
  }
  String sRequest = client.readStringUntil('\r');
  client.flush();
  if(sRequest=="")
  {
    client.stop();
    return;
  } 
   if(sRequest.startsWith("GET /fwlink/a"))
  {  
    qsid = urldecode(sRequest.substring(17,sRequest.indexOf('&')).c_str()); //correct coding for spaces as "+"
        qpass = urldecode(sRequest.substring(sRequest.lastIndexOf('=')+1,sRequest.lastIndexOf(' ')).c_str());//correct for coding spaces as "+"
    rsinfo = qpass;
        qsid.toCharArray(ssidsav,32);        
        qpass.toCharArray(passwordsav,32);
      saveCredentials();
  } else if(sRequest.startsWith("GET /a"))
   {
        qsid = urldecode(sRequest.substring(9,sRequest.indexOf('&')).c_str()); //correct coding for spaces as "+"
        qpass = urldecode(sRequest.substring(sRequest.lastIndexOf('=')+1,sRequest.lastIndexOf(' ')).c_str());//correct for coding spaces as "+"
        rsinfo = qpass;
        qsid.toCharArray(ssidsav,32);
        qpass.toCharArray(passwordsav,32);
        saveCredentials();
   }
   else if(sRequest.startsWith("GET /generate_204/a"))
   {
        qsid = urldecode(sRequest.substring(23,sRequest.indexOf('&')).c_str()); //correct coding for spaces as "+"
        qpass = urldecode(sRequest.substring(sRequest.lastIndexOf('=')+1,sRequest.lastIndexOf(' ')).c_str());//correct for coding spaces as "+"
        rsinfo = qpass;
        qsid.toCharArray(ssidsav,32);
        qpass.toCharArray(passwordsav,32);
        saveCredentials();
   }


loadCredentials();
   
  String sResponse,sHeader;
  /////////////////////////////////////////    
    sResponse  = "<html><head><meta name='viewport' content='width:600px' /><meta name='viewport' content='initial-scale = 1.0,maximum-scale = 1.0' /></head><body><center><div style='text-shadow:1px 1px 10px rgba(148,148,148,1);font-weight:normal;color:#ffffff;border: 1px solid #b5a759;letter-spacing:2pt;word-spacing:9pt;font-size:15px;text-align:left;font-family:helvetica, sans-serif;line-height:1;";
    sResponse += "outline: 1px solid #999999;outline-offset: 10px;box-shadow: 0px 0px 10px 2px #444444; -moz-box-shadow: 0px 0px 10px 2px #444444;";
    sResponse += "-webkit-box-shadow: 0px 0px 10px 2px #444444;color:#ffffff;background-image:radial-gradient(#2E2E2E,#000000);width:550px;'><center><h1>REMOTE SETUP for ";
    sResponse += ssidsav;
    sResponse += " </h1>" ;
    sResponse += "\r\n<br /><form method='get' action='a'><h4>CONNECT TO NETWORK:</h4>";
    sResponse += "<input type='text' value='";
    sResponse += ssidsav;
    sResponse += "' name='n'/>";
    sResponse += "<br /><input type='password' placeholder='password' name='p'/><br>";
    sResponse += "<br /><input type='submit' value='Save GoPro Wifi'/></form><br>";
    sResponse += "<br><a href='http://euerdesign.de/' target='_blank' style='color:#ffffff;'><b>euerdesign.de</b></a><br><br> ";
    sResponse += "</center></div></center><br><br></body></html>";
  /////////////////////////////////////////    
    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  /////////////////////////////////////////    
  client.print(sHeader);
  client.print(sResponse);
  client.stop();
  } // ONLY IF REMOTESETUP
} // CHECKIFREMOTESETUP
