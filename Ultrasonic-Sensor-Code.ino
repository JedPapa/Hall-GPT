#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define WIFI_SSID "WIFINAME"
#define WIFI_PASSWORD "WIFIPASS"

#define FIREBASE_HOST "https://hall-gpt-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "AIzaSyBtL2yoBHcOeQjzZL9hwK629HefsZsL06E"
FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;
const int trigPin = 12; //D6
const int echoPin = 14; //D5
int detection;
#define SOUND_VELOCITY 0.034
#define CM_TO_INCH 0.393701

#define threshold 200
bool cooldown = false;

float distanceCm;
float distanceInch;

void connectToWiFi() {
  Serial.println("Connecting to Wi-Fi");
  WiFi.begin(WIFI_SSID,WIFI_PASSWORD); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("Connected to Wi-Fi");
}

void setup() {
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(echoPin, LOW);
  connectToWiFi();
  config.database_url = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);   
  Firebase.reconnectWiFi(true);
}

void loop() {
  float distance = getDistance();
  
  if (distance <= 5.0) {
    Serial.println("Object detected");
    Serial.println(distance);
    detection = 1;
   
  }else {
    Serial.println("No Detection");
    Serial.println(distance);
    detection = 0;
  }

 if (Firebase.setString(firebaseData, "/Promixity", String(detection))) {
        Serial.print("Data ");
        Serial.print(detection);
        Serial.println("Uploaded Successfully");
    } else {        
        Serial.println(firebaseData.errorReason());
    }


  }

      float getDistance() {
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);

      long duration = pulseIn(echoPin, HIGH);

      float inches = duration / 74.0 / 2.0;

      return inches;
    }