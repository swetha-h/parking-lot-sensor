#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

#define WIFI_SSID "Mela"                                             // input your home or public wifi name 
#define WIFI_PASSWORD "25232333"                                    //password for Wifi

int TRIG = D7;                  //ultrasonic trig  pin
int ECHO = D8;                // ultrasonic echo pin
int led = D3;                 // spot occupancy signal
long duration, distance, occupied;

String serverName = "http://192.168.1.43:8000";
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

void setup() {
  delay(1000);
  Serial.begin (9600);     // serial debugging
  Serial.print("hi");

  pinMode(TRIG, OUTPUT);      // trig pin as output
  pinMode(ECHO, INPUT);         // echo pin as input
  pinMode(led, OUTPUT);         // spot indicationy

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);                                     //try to connect with wifi

  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);                          // display ssid
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");                          // if not connected print this
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is : ");
  Serial.println(WiFi.localIP());                                            //print local IP address
  Serial.println(F("Connected!"));
}
void loop() {

  digitalWrite(TRIG, LOW);         // make trig pin low
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);        // make trig pin high
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration / 2) / 29.1;      // take distance in cm
  Serial.print("Centimeter: ");
  Serial.println(distance);
  if (distance < 6) {                      //if distance is less than 6cm then on led
    Serial.println("Occupied ");
    occupied = 1;
    digitalWrite(led, HIGH);
  }
  if (distance > 6) {                        //if distance is greater than 6cm then off led
    Serial.println("Available ");
    occupied = 0;
    digitalWrite(led, LOW);
  }

  if ((millis() - lastTime) > timerDelay) {
    Serial.println("inside if");
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;

      String serverPath = serverName + "/update-spot";

      // Your Domain name with URL path or IP address with path
      http.begin(client, serverPath.c_str());

      String httpPayload = String("{\"id\":1,\"occupied\":" + String(occupied) + "}");

      // Send HTTP GET request
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(httpPayload);

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
