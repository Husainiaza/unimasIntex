/*------------------------------------------------------------------
  ===================  Seksyen 1 - HEADER        ===================
  ------------------------------------------------------------------*/
//-------------------------------------------------------------------
//=  A. - Library  include and define  yang diperlukan              =
//-------------------------------------------------------------------
#include <WiFiS3.h>
#include <MQTT.h>

//--------------------------------------------------------------------------
//-----b. - ISTIHAR Sambungan nama Pin dengan Nombor Fizikal PIN ----
//--------------------------------------------------------------------------
#define trigPin        7 
#define echoPin        6 
#define RELAY_PIN      5

// ~~~~~~~ PENTING ~~~~~~~~~
// ------ Sila edit nama atau ID ikut keperluan --------
#define Client_Id   "Arduino_R4_Intex2025aza" 
#define NamaBroker  "broker.emqx.io"
#define MQTTPort    1883
//#define namaSSID    "Ina71"           
//#define SSIDpwd     "11201320"   
#define namaSSID    "Haza@unifi"           
#define SSIDpwd     "0135335045"   


// ~~~~~~~  TAMMAT   ~~~~~~~~~

//-----c.  - ISTIHAR  constant dan pembolehubah------------------------------
const char ssid[] = namaSSID;
const char pass[] = SSIDpwd;
const char* topic_water = "home/waterlevel"; 
const char* topic_valve = "home/solenoid";
const char* topic_PH = "home/PHlevel";

//------ ISTIHAR Pemboleh ubah dengan jenis Data yang digunakan---
unsigned long lastMillis = 0;
int waterLevelThreshold = 20; // Distance threshold in cm 
float duration_us, distance_cm; 

//-----d. - Cipta Objek dari Librari berkaitan----------------------------
WiFiClient net;
MQTTClient client;

//##################  Seksyen 1 - TAMAT #############################

//--------------FUNCTIONS----------------------------

// Function to connect to MQTT broker
void connect() {
  Serial.print("Checking WiFi connection...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("\nconnecting...");
  while (!client.connect(Client_Id)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\nconnected!");
  client.subscribe("home/solenoid");
}

// Function to handle received MQTT messages
void messageReceived(String &topic, String &payload) {
  Serial.println("Received message on topic: " + topic);
  Serial.println("Payload: " + payload);
  
  //==============================================================

  //==========================Relay Control ========================
 if(String(topic) == "home/solenoid") 
  {
      if(payload =="on")
      {
      Serial.println(payload);
      digitalWrite(RELAY_PIN,HIGH);
      Serial.println("Solenoid ON");
      }
      
      else if(payload =="off")
      {
        Serial.println(payload);
        digitalWrite(RELAY_PIN,LOW);
        Serial.println("Solenoid OFF");
        
      }
  } 
}

 

//###################################################
//==================  Seksyen 2 - Fungsi Setup ======================
//-------------------------------------------------------------------
void setup() {
  // Initialize pins for ultrasonic sensor
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  pinMode(RELAY_PIN, OUTPUT); 
 
  // Initialize serial communication
  Serial.begin(115200); 
  delay(2000); // Give serial time to initialize
  
  // Connect to WiFi
  Serial.println("Starting WiFi connection...");
  Serial.print("SSID: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, pass);
  
  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) {
    delay(1000);
    Serial.print(".");
    wifi_attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.println("WiFi connected successfully!");
    Serial.print("IP address: ");

  } else {
    Serial.println();
    Serial.println(" Failed to connect to WiFi!");
    Serial.println("Check your credentials and try again.");
    while(1) {
      delay(1000); // Stop execution if WiFi fails
    }
  }

  // Initialize MQTT client
  Serial.println(" Initializing MQTT client...");
  Serial.print("Broker: ");
  Serial.print(NamaBroker);
  Serial.print(":");
  Serial.println(MQTTPort);
  
  client.begin(NamaBroker, MQTTPort, net);
  client.onMessage(messageReceived);
  client.setTimeout(30000);  // 30 seconds timeout
  
  // Connect to MQTT broker
  connect();

}
//##################  Seksyen 2 - TAMAT #############################

//==============  Seksyen 3 - Fungsi Utama (LOOP) ===================
//-------------------------------------------------------------------
void loop() {
  // Keep MQTT client alive
  client.loop();
  delay(10);  // Small delay for stability

  // Check MQTT connection
  if (!client.connected()) {
    Serial.println("MQTT disconnected! Attempting to reconnect...");
    connect();
    delay(1000);  // Give time after reconnection attempt
  }

  // Measure and publish water level every 5 seconds
  if (millis() - lastMillis > 2000) {
    lastMillis = millis();
    
    Serial.println("--------------------");
    Serial.print("Time: ");
    Serial.print(millis() / 1000);
    Serial.println(" seconds");
    Serial.println("--------------------");
    
    //measureWaterLevel();

    // sensor 1 ================================

    int dataAnalog = analogRead(A0);
    Serial.print("Cahaya: ");
    Serial.println(dataAnalog);  

    client.publish("aza/analog", String(dataAnalog));  

 // sensor 1 ================================
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10); 
  digitalWrite(trigPin, LOW); 

  // Measure echo time and calculate distance
  duration_us = pulseIn(echoPin, HIGH); 
  distance_cm = 0.017 * duration_us; 

  Serial.print("Water Distance: "); 
  Serial.print(distance_cm); 
  Serial.println(" cm"); 

  client.publish("home/waterlevel", String(distance_cm)); 

    
    
  }
}
//##################  Seksyen 3 - TAMAT #############################
