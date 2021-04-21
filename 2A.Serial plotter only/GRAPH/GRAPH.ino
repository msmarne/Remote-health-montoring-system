/*
 * HTTP Client
 * Copyright (c) 2015, circuits4you.com
 * All rights reserved.
/* Connects to WiFi HotSpot. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "Mrunal";  //ENTER YOUR WIFI SETTINGS <<<<<<<<<
const char *password = "mrunal@1234";

//Web address to read from
//const char *host = "api.thingspeak.com";
//String apiKey = "5S00PH4OPJK0L43H";  //ENTER YOUR API KEY <<<<<<<<<<<
//=======================================================================
//                    Power on setup
//=======================================================================

void setup()
{
  delay(1000);
  Serial.begin(115200);

  //WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  //WiFi.mode(WIFI_AP_STA);   //Both hotspot and client are enabled
  //WiFi.mode(WIFI_AP);       //Only Access point
  
  //WiFi.begin(ssid, password);     //Connect to your WiFi router
  //Serial.println("");

  //Serial.print("Connecting");
  // Wait for connection
  //while (WiFi.status() != WL_CONNECTED) {
    //delay(500);
    Serial.print(".");
  }



//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop()
{
  Serial.println(analogRead(A0));
  delay(1);
//---------------------------------------------------------------------
}
//=======================================================================
