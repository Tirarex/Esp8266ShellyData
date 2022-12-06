#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>


//Includes for ESP
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Settings
const char* ssid     = "";
const char* password = "";
const char* shellyAPILink = "http://PUT SHELLY DEVICE IP HERE/rpc/Shelly.GetStatus";

const int updateRateMS = 500; // Data and screen update ratio

//Screen settings
#define SCREEN_ADDRESS 0x3C


Adafruit_SSD1306 display(128, 32, &Wire, -1);
double voltage;
double power;
String sensorReadings;
int httpResponseCode ;



void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  //Setup display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  //Sample text
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Hi! This is shelly to esp8266 sample!");
  display.display();
  delay(2000);

  //Show device ip for a moment
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
}

void loop() {
  GetData();
  delay(updateRateMS);

}

void GetData() {
  bool HasError = false;
  sensorReadings = "{}";

  WiFiClient client;
  HTTPClient http;

  http.begin(client, shellyAPILink);
  //Try get data
  httpResponseCode = http.GET();



  //Write data if OK or throw error
  if (httpResponseCode > 0) {
    sensorReadings = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    HasError = true;
  }
  http.end();

  if (HasError) {
    //Update screen text
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println("Error, cant get ata from shelly");
    display.display();
  }
 
  //Parse json
  DynamicJsonDocument doc(1337);
  deserializeJson(doc, sensorReadings);

  //Get values from json
  DynamicJsonDocument SwitchData  = doc["switch:0"];
  power    = SwitchData["apower"];
  voltage    = SwitchData["voltage"];

  //Update screen text
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(2);
  display.println("V:" + String(voltage));
  display.println("W:" + String(power));
  display.display();

  //Show data in serial
  //Serial.println(power);
  //Serial.println(voltage);

}
