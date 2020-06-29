/*  Penulis: Mohamad Ariffin Zulkifli
*   Myinvent Technologies Sdn Bhd (https://myduino.com)
*
*   Arduino sketch untuk satu device ESP8266 menghantar data
*   kepada kedua2 IoT platform Thingspeak dan Favoriot.
*/

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SimpleDHT.h>

char WIFI_SSID[] = "###";       // Tukarkan dengan WiFi SSID
char WIFI_PASSWORD[] = "###";   // Tukarkan dengan WiFi Password
char FAVORIOT_TOKEN[] = "###";  // Tukarkan dengan Favoriot Device Access Token
char FAVORIOT_DEVICE[] = "###"; // Tukarkan dengan Favoriot Device Developer Id
char THINGSPEAK_API[] = "###";   // Tukarkan dengan Thingspeak Write API Key

SimpleDHT11 dht11(D6);

unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.println("NodeMCU connected to WiFi router!");
  Serial.print("IP Address:");
  Serial.println(WiFi.localIP());
}

void loop() {
  byte temperature = 0;
  byte humidity = 0;
  
  int err = SimpleDHTErrSuccess;
  if ((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    // Serial.print("Read DHT11 failed, err=");
    // Serial.println(err);
    // delay(500);
    
    return;
  }

  if(millis() - previousMillis > 20000){
    
    previousMillis = millis();
    
    HTTPClient httpTS;

    httpTS.begin("http://api.thingspeak.com/update?api_key=" + String(THINGSPEAK_API) + "&field1=" + String(temperature) + "&field2=" + String(humidity));
  
    int httpCodeTS = httpTS.GET();
  
    if(httpCodeTS > 0){
      String response = httpTS.getString();
      Serial.println("ThingSpeak Success: " + response);
    }
    else{
      Serial.println("HTTP Error!");
    }
  
    httpTS.end();

    delay(50);

    String body = "{\"device_developer_id\":\"" + String(FAVORIOT_DEVICE) + "\",\"data\":{";
    body += "\"suhu\":";
    body += temperature;
    body += ",\"kelembapan\":";
    body += humidity;
    body += "}}";

    HTTPClient httpF;

    httpF.begin("http://apiv2.favoriot.com/v2/streams");
    httpF.addHeader("Content-Type", "application/json");
    httpF.addHeader("Apikey", FAVORIOT_TOKEN);
  
    int httpCodeF = httpF.POST(body);
  
    if(httpCodeF > 0){
      String response = httpF.getString();
      Serial.println("Favoriot Success: " + response);
    }
    else{
      Serial.println("HTTP Error!");
    }
  
    httpF.end();

    Serial.println("------------------------------------------------");
    
  }

}
