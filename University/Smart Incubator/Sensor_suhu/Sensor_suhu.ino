#include <Arduino.h> 
#if defined(ESP32) 
#include <WiFi.h> 
#elif defined(ESP8266) 
#include <ESP8266WiFi.h> 
#endif 
#include <Firebase_ESP_Client.h> 
#include "HTTPSRedirect.h" 
//Provide the token generation process info. 
#include "addons/TokenHelper.h" 
//Provide the RTDB payload printing info and other helper functions. 

 
#include "addons/RTDBHelper.h" 
 
#include "DHT.h" 
#define DHTPIN 4 //D2 
#define DHTTYPE DHT11 
int relay = 12;//D6 
int relay2= 15;//D8 
 
// Insert your network credentials 
#define WIFI_SSID "PRS" 
#define WIFI_PASSWORD "solat5waktu" 
 
// Enter Google Script Deployment ID: 
const char *GScriptId = "AKfycbwAuWgqI3sqTyMmLiKQIxq5N3aZPwuR_
OisFNZhzORICUtCeaF9_mlqR5-w0YCTJn6"; 
 
// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1): 
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": "; 
String payload = ""; 
 
// Google Sheets setup (do not edit) 
const char* host = "script.google.com"; 
const int httpsPort = 443; 
const char* fingerprint = ""; 
String url = String("/macros/s/") + GScriptId + "/exec"; 
HTTPSRedirect* client = nullptr; 
 
// Insert Firebase project API Key 
#define API_KEY "AIzaSyAXDI3xDSl_uwhpemSHZgy7PTCdV2xT-dI" 
 
// Insert RTDB URLefine the RTDB URL */ 
#define DATABASE_URL "https://dht11-e4e73-default-rtdb.firebaseio.com/"  
 
//Define Firebase Data object 
FirebaseData fbdo; 
FirebaseAuth auth; 
FirebaseConfig config; 
String strValue1,strValue2, lampu1, statelampu2, kondisi_lampu ; 
 
unsigned long sendDataPrevMillis = 0; 
int count = 0; 
bool signupOK = false; 
 
DHT dht(DHTPIN, DHTTYPE); 
 
void setup(){ 
  Serial.begin(115200); 
  pinMode(5, OUTPUT);  
  pinMode(relay, OUTPUT); 
  pinMode(relay2, OUTPUT); 
  Serial.println(F("DHTxx test!")); 
  dht.begin(); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); 
  Serial.print("Connecting to Wi-Fi"); 
  while (WiFi.status() != WL_CONNECTED){ 
    Serial.print("."); 
    delay(300); 
  } 
  Serial.println(); 
  Serial.print("Connected with IP: "); 
  Serial.println(WiFi.localIP()); 
  Serial.println(); 
 
  /* Assign the api key (required) */ 
  config.api_key = API_KEY; 
 
  /* Assign the RTDB URL (required) */ 
  config.database_url = DATABASE_URL; 
 
  /* Sign up */ 
  if (Firebase.signUp(&config, &auth, "", "")){ 
    Serial.println("ok"); 
    signupOK = true; 
  } 
  else{ 
    Serial.printf("%s\n", config.signer.signupError.message.c_str()); 
  } 
 
  /* Assign the callback function for the long running token generation task */ 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h 
   
  Firebase.begin(&config, &auth); 
  Firebase.reconnectWiFi(true); 
 
    // Use HTTPSRedirect class to create a new TLS connection 
  client = new HTTPSRedirect(httpsPort); 
  client->setInsecure(); 
  client->setPrintResponseBody(true); 
  client->setContentTypeHeader("application/json"); 
   
  Serial.print("Connecting to "); 
  Serial.println(host); 
 
    // Try to connect for a maximum of 5 times 
  bool flag = false; 
  for (int i=0; i<5; i++){  
    int retval = client->connect(host, httpsPort); 
    if (retval == 1){ 
       flag = true; 
       Serial.println("Connected"); 
       break; 
    } 
    else 
      Serial.println("Connection failed. Retrying..."); 
  } 
  if (!flag){ 
    Serial.print("Could not connect to server: "); 
    Serial.println(host); 
    return; 
  } 
  delete client;    // delete HTTPSRedirect object 
  client = nullptr; // delete HTTPSRedirect object 
 
} 
 
void loop(){ 
  delay(1000); 
  float h = dht.readHumidity(); 
  // Read temperature as Celsius (the default) 
  float t = dht.readTemperature(); 
  // Read temperature as Fahrenheit (isFahrenheit = true) 
  float f = dht.readTemperature(true); 
 
  if (isnan(h) || isnan(t) || isnan(f)) { 
    Serial.println(F("Failed to read from DHT sensor!")); 
    return; 
  } 
 
  float hif = dht.computeHeatIndex(f, h); 
  // Compute heat index in Celsius (isFahreheit = false) 
  float hic = dht.computeHeatIndex(t, h, false); 
 
  if (Firebase.ready() && signupOK){ 
    sendDataPrevMillis = millis(); 
    // Write an Int number on the database path test/int 
    if (Firebase.RTDB.setFloat(&fbdo, "DHT11/Suhu", t)){ 
      Serial.println("PASSED"); 
      Serial.print(F("Suhu: ")); 
      Serial.print(t); 
      Serial.println(F(" °C")); 
      Serial.println("PATH: " + fbdo.dataPath()); 
      Serial.println("TYPE: " + fbdo.dataType()); 
    } 
    else { 
      Serial.println("FAILED"); 
      // Serial.println("REASON: " + fbdo.errorReason()); 
    } 
     
    // Write an Float number on the database path test/float 
    if (Firebase.RTDB.setFloat(&fbdo, "DHT11/Kelembapan", h)){ 
      Serial.println("PASSED"); 
      Serial.print(F("Kelembapan: ")); 
      Serial.print(h); 
      Serial.println(F(" %")); 
      Serial.println("PATH: " + fbdo.dataPath()); 
      Serial.println("TYPE: " + fbdo.dataType()); 
    } 
    else { 
      Serial.println("FAILED"); 
      // Serial.println("REASON: " + fbdo.errorReason()); 
    } 
     
      if (Firebase.RTDB.getString(&fbdo, "/DHT11/Lampu1")) { 
      if (fbdo.dataType() == "string") { 
        strValue1 = fbdo.stringData(); 
        Serial.println(strValue1); 
            if (strValue1 == "1"){ 
            Serial.println("HAI"); 
            lampu1="ON"; 
            digitalWrite(relay, HIGH); 
            digitalWrite(5, HIGH); 
          } 
          else { 
            Serial.println("Yahh"); 
            digitalWrite(relay, LOW);  
            lampu1="OFF"; 
            digitalWrite(5, LOW);     
          } 
      } 
    } 
    else { 
      Serial.println(fbdo.errorReason()); 
    } 
 
      if (Firebase.RTDB.getString(&fbdo, "/DHT11/Lampu2")) { 
      if (fbdo.dataType() == "string") { 
        strValue2 = fbdo.stringData(); 
        Serial.println(strValue2); 
            if (strValue2 == "1"){ 
            Serial.println("HAI"); 
            statelampu2="ON"; 
            digitalWrite(relay2, HIGH); 
            digitalWrite(5, HIGH); 
          } 
          else { 
            Serial.println("Yahh"); 
            digitalWrite(relay2, LOW); 
            statelampu2="OFF";  
            digitalWrite(5, LOW);     
          } 
      } 
    } 
    else { 
      Serial.println(fbdo.errorReason()); 
    } 
 
    kondisi_lampu = ("Lampu 1 : " + lampu1 +" & "+ "Lampu 2 : " + statelampu2); 
 
    static bool flag = false; 
    if (!flag){ 
      client = new HTTPSRedirect(httpsPort); 
      client->setInsecure(); 
      flag = true; 
      client->setPrintResponseBody(true); 
      client->setContentTypeHeader("application/json"); 
    } 
    if (client != nullptr){ 
      if (!client->connected()){ 
        client->connect(host, httpsPort); 
      } 
    } 
    else{ 
      Serial.println("Error creating client object!"); 
    } 
     
    // Create json object string to send to Google Sheets 
    payload = payload_base + "\"" + h + "," + t + "," + kondisi_lampu + "\"}"; 
     
    // Publish data to Google Sheets 
    Serial.println("Publishing data..."); 
    Serial.println(payload); 
    if(client->POST(url, host, payload)){  
      // do stuff here if publish was successful 
    } 
    else{ 
      // do stuff here if publish was not successful 
      Serial.println("Error while connecting"); 
    } 
 
    // a delay of several seconds is required before publishing again     
  } 
}