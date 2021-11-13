#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include "DHT.h"
#include <ESP32Servo.h>
#include <analogWrite.h>
#include <ESP32PWM.h>
#include <ESP32Tone.h>
#define DHTPIN 23
#define DHTTYPE DHT11

#define FIREBASE_HOST "https://fir-71190448-default-rtdb.asia-southeast1.firebasedatabase.app/"
#define FIREBASE_AUTH "Wp0AsAIURN0eJVM3XEhBhZYQbDN78jLdOKX2Ebe9"
#define WIFI_SSID "Redmi Note 9 Pro"
#define WIFI_PASSWORD "jamafohei"

static const int servoPin = 13;
DHT dht(DHTPIN, DHTTYPE);
Servo servo;

void setup() {
  Serial.begin(115200);
  servo.attach(servoPin);
  dht.begin();
  
  // mengkoneksikan ke wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

}

void loop() {

  //Baca sensor suhu dan kelembaban
  delay(2000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  
  //jika gagal membaca
    if (isnan(h) || isnan(t)) {
    Serial.println("Gagal membaca dari sensor DHT!");
    return;
  }
  
  //untuk print ke serial
  Serial.print("Kelembaban : ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Suhu : ");
  Serial.print(t);
  Serial.println(" *C ");
  
  // set hasil temperatur
  Firebase.setFloat("Suhu", t);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  delay(100);

  //set hasil kelembaban
  Firebase.setFloat("Kelembaban", h);
  // handle error
  if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
  }
  servo.write(180);
  delay(10);
  servo.write(-180);
  
  if (t >= 29.0){
    Firebase.setString("Ventilasi","TERBUKA");
    servo.write(180);
    delay(20);
  }else{
    Firebase.setString("Ventilasi","TERTUTUP");
    servo.write(-180);
    delay(20);
  }
}
