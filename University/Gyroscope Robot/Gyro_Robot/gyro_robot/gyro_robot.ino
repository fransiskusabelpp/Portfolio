#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>


// Definisikan pin untuk motor
int enA = D3;
int in1 = D4;
int in2 = D5;

int enB = D6;
int in3 = D7;
int in4 = D8;

float xValue = 0;
float yValue = 0;

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define ssid "abel"
#define password "solat5waktu"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCK_TGRFgRP17d858b47A6RBXs1cZ2Bb5Y"

// Insert RTDB URL
#define DATABASE_URL "https://dhtkodular-default-rtdb.firebaseio.com/" 

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

bool signupOK = false;

void setup(){
  Serial.begin(9600);
  
  // WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign the API key (required)
  config.api_key = API_KEY;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  // Sign up
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  // Assign the callback function for the long running token generation task
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Set pin modes for motor
  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
}

void loop() {
  if (Firebase.ready() && signupOK) {
    if (Firebase.RTDB.getString(&fbdo, "DHT11/X")) {
      String xStr = fbdo.stringData();
      xValue = xStr.toFloat();
      Serial.print("Nilai X: ");
      Serial.println(xValue);
    } else {
      Serial.print("Failed to get X value: ");
      Serial.println(fbdo.errorReason());
      return; // Keluar dari loop jika gagal mendapatkan nilai X
    }

    if (Firebase.RTDB.getString(&fbdo, "DHT11/Y")) {
      String yStr = fbdo.stringData();
      yValue = yStr.toFloat();
      Serial.print("Nilai Y: ");
      Serial.println(yValue);
    } else {
      Serial.print("Failed to get Y value: ");
      Serial.println(fbdo.errorReason());
      return; // Keluar dari loop jika gagal mendapatkan nilai Y
    }
      
      if (xValue < -40 && yValue > -60 && yValue < 50) {
        // Move forward
        Serial.println("Maju");
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        analogWrite(enA, 100); 
        
        // Motor 2 maju
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        analogWrite(enB, 100); 
        // Add code to move forward here
      } 
      else if (xValue > 50  && yValue > -60 && yValue < 50) {
        // Move backward
        Serial.println("Mundur");
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        analogWrite(enA, 100);
        
        // Motor 2 mundur
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        // Add code to move backward here
      }

      else if (yValue < -60 && xValue > -40 && xValue < 50) {
        // Move left
        Serial.println("Ke kiri");
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        analogWrite(enA, 100);
        
        // Motor 2 mundur
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        analogWrite(enB, 100); 
        // Add code to move left here
      } 
      else if (yValue > 50 && xValue > -40 && xValue < 50) {
        // Move right
        Serial.println("Ke kanan");
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        analogWrite(enA, 100); 
        
        // Motor 2 maju
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW);
        analogWrite(enB, 100);
        // Add code to move right here
      }
      else{
        Serial.println("Diam");
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        analogWrite(enA, 0);

        digitalWrite(in3, LOW);
        digitalWrite(in4, LOW);
        analogWrite(enB, 0);
      }

  }
}