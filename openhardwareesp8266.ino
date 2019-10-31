//  * HTTP Client GET Request
//  * Copyright (c) 2018, circuits4you.com
//  * All rights reserved.
//  * https://circuits4you.com 
//  * Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>

#include <WiFiClient.h>

#include <ESP8266WebServer.h>

#include <ESP8266HTTPClient.h>

#include <LiquidCrystal_I2C.h>

#include <ArduinoJson.h>


/* Set these to your desired credentials. */
const char * ssid = "GlobeAtHome_1BDF6_2.4"; //ENTER YOUR WIFI SETTINGS
const char * password = "b72LPMJ6";

//=======================================================================
//                    Power on setup
//=======================================================================
LiquidCrystal_I2C lcd(0x27, 16, 2);
void setup() {

    //initialize LCD


    lcd.init();
    lcd.clear();
    lcd.backlight();


    delay(1000);
    Serial.begin(230400);
    WiFi.mode(WIFI_OFF); //Prevents reconnection issue (taking too long to connect)
    delay(1000);
    WiFi.mode(WIFI_STA); //This line hides the viewing of ESP as wifi hotspot

    WiFi.begin(ssid, password); //Connect to your WiFi router
    Serial.println("");

    Serial.print("Connecting");
    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Connecting Please Wait");
        lcd.setCursor(0, 1);
        lcd.print("..");
    }

    //If connection successful show IP address in serial monitor
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP()); //IP address assigned to your ESP

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(ssid);
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());

}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
    HTTPClient http; //Declare object of class HTTPClient

    String Link;
    //JSON PARSER



    //GET Data

    Link = "http://192.168.254.104:8080/data.json";

    http.begin(Link); //Specify request destination
    http.addHeader("Content-Type", "text/html");

    int httpCode = http.GET(); //Send the request
    jsonParser(http.getString());

    http.end(); //Close connection

    delay(1000); //GET Data at every 5 seconds
}
//=======================================================================

void jsonParser(String payload) {
    Serial.println("running json parser");
    Serial.println(payload);

    const size_t capacity = 49*JSON_ARRAY_SIZE(0) + 13*JSON_ARRAY_SIZE(1) + 5*JSON_ARRAY_SIZE(2) + 2*JSON_ARRAY_SIZE(3) + 3*JSON_ARRAY_SIZE(4) + 3*JSON_ARRAY_SIZE(5) + 2*JSON_ARRAY_SIZE(6) + JSON_ARRAY_SIZE(9) + 78*JSON_OBJECT_SIZE(7) + 7730;
    DynamicJsonDocument doc(capacity);

    DeserializationError error = deserializeJson(doc, payload, DeserializationOption::NestingLimit(12));
    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println(F("Response:"));
    Serial.println(doc["Children"][0]["Children"][1]["Children"][1]["Children"][4]["Value"].as<char*>());

    String cputemp = doc["Children"][0]["Children"][1]["Children"][1]["Children"][4]["Value"].as<char*>();
    String cpuName = doc["Children"][0]["Children"][1]["Text"].as<char*>();

    String memoryOver = "Ram Usage";
    String memoryUsed = doc["Children"][0]["Children"][2]["Children"][0]["Children"][0]["Value"].as<char*>();

    String gpuTemp = doc["Children"][0]["Children"][3]["Children"][2]["Children"][0]["Value"].as<char*>();
    String gpuName = doc["Children"][0]["Children"][3]["Text"].as<char*>();

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(cpuName);
    lcd.setCursor(0, 1);
    lcd.print(cputemp);

    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(memoryOver);
    lcd.setCursor(0, 1);
    lcd.print(memoryUsed);

    delay(2000);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(gpuName);
    lcd.setCursor(0, 1);
    lcd.print(gpuTemp);



    serializeJson(doc, Serial);
}