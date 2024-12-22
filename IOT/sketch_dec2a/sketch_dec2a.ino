#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>
#include <Base64.h>
#include<ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>

// WiFi konfiguracija
#define WIFI_SSID "Drpic"
#define WIFI_PASSWORD "43211234"

// Firebase konfiguracija
#define FIREBASE_HOST "iot2024-7ae41-default-rtdb.europe-west1.firebasedatabase.app"
#define FIREBASE_AUTH "AIzaSyDiV5eOacEUuLFG5S4GA6kl0VkaWvrC3YM"

// DHT senzor
#define DHT_PIN D7
#define DHT_TYPE DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Pini za LED diode
#define LED1_PIN D1 // Visoka temperatura
#define LED2_PIN D2 // Visoka CO2 koncentracija
#define LED3_PIN D3 // Umjereno zagađen zrak

// Pin za buzzer
#define BUZZER_PIN D5 

FirebaseConfig config;
FirebaseAuth auth;
FirebaseData firebaseData;

void setup() {
  Serial.begin(115200);
  
  dht.begin();

  pinMode(LED1_PIN, OUTPUT);//CRVENA
  pinMode(LED2_PIN, OUTPUT);//ZUTA
  pinMode(LED3_PIN, OUTPUT);//ZELENA
  pinMode(BUZZER_PIN, OUTPUT); 


  // Povezivanje na WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Povezivanje na WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nPovezano na WiFi!");
  Serial.print("ESP IP adresa: ");
  Serial.println(WiFi.localIP());

 

  
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  
  if (Firebase.ready()) {
    Serial.println("Povezano na Firebase!");
  } else {
    Serial.println("Firebase konekcija nije uspjela.");
    Serial.println(firebaseData.errorReason());
  }
}
void loop() {
  
  float temperature = dht.readTemperature();
  
  int co2 = analogRead(A0);

   
  if (Firebase.ready()) {
    if (Firebase.setFloat(firebaseData, "/sensor/temperature", temperature)) {
      
    } else {
      Serial.print("Greška pri slanju temperature: ");
      Serial.println(firebaseData.errorReason());
    }

    if (Firebase.setInt(firebaseData, "/sensor/co2", co2)) {
      
    } else {
      Serial.print("Greška pri slanju CO2: ");
      Serial.println(firebaseData.errorReason());
    }
  }
  
  if (temperature > 30) { 
    digitalWrite(LED1_PIN, HIGH); 
  } else {
    digitalWrite(LED1_PIN, LOW); 
  }

  if (co2 > 200) { 
    digitalWrite(LED2_PIN, HIGH); 
  } else {
    digitalWrite(LED2_PIN, LOW); 
  }

  if (co2 < 200 && temperature <30) { 
    digitalWrite(LED3_PIN, HIGH); 
  } else {
    digitalWrite(LED3_PIN, LOW); 
  }

  if (temperature > 30 || co2 > 200) { 
    tone(BUZZER_PIN, 1000); 
    
  } else {
    digitalWrite(BUZZER_PIN, LOW); 
  }
  
  
  delay(1000); 
}