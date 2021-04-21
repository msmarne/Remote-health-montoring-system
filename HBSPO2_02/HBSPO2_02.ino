#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

/* Set these to your desired credentials. */
const char *ssid = "Mrunal";  //ENTER YOUR WIFI SETTINGS <<<<<<<<<
const char *password = "mrunal@1234";

//Web address to read from
const char *host = "api.thingspeak.com";
String apiKey = "5S00PH4OPJK0L43H";  //ENTER YOUR API KEY <<<<<<<<<<<

#define REPORTING_PERIOD_MS     1000

// PulseOximeter is the higher level interface to the sensor
// it offers:
//  * beat detection reporting
//  * heart rate calculation
//  * SpO2 (oxidation level) calculation
PulseOximeter pox;
uint32_t tsLastReport = 0;
//7016422408
unsigned char fix_MOB[10]={'7','0','1','6','4','2','2','4','0','8'};

// Callback (registered below) fired when a pulse is detected
void onBeatDetected()
{
    Serial.println("Beat!");
}

void setup()
{
  delay(1000);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
    
  Serial.print("Initializing pulse oximeter..");
  if (!pox.begin()) 
  {
    Serial.println("FAILED");
//    for(;;);
  }
  else 
  {
    Serial.println("SUCCESS");
  }

  // The default current for the IR LED is 50mA and it could be changed
  //   by uncommenting the following line. Check MAX30100_Registers.h for all the
  //   available options.
  // pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

  // Register a callback for the beat detection
  pox.setOnBeatDetectedCallback(onBeatDetected);  
  GSM_Init();
}

void loop()
{
  unsigned char temp;
  int HB, SP;
    // Make sure to call update as fast as possible
  pox.update();
  if (millis() - tsLastReport > REPORTING_PERIOD_MS)
  {
    HB=pox.getHeartRate();
    SP=pox.getSpO2();
    Serial.print("Heart rate:");
    Serial.print(HB);
    Serial.print("bpm / SpO2:");
    Serial.print(SP);
    Serial.println("% ");
    tsLastReport = millis();
  }

  WiFiClient client;          
  const int httpPort = 80; //Port 80 is commonly used for www
 //---------------------------------------------------------------------
 //Connect to host, host(web site) is define at top 
  if(!client.connect(host, httpPort))
  {
    Serial.println("Connection Failed");
    delay(300);
   return; //Keep retrying until we get connected
  }
 
//---------------------------------------------------------------------
  //Make GET request as pet HTTP GET Protocol format
  String ADCData;
  int adcvalue=SP;  //Read Analog value of LDR
  ADCData = String(adcvalue);   //String to interger conversion
  String Link="GET /update?api_key="+apiKey+"&field1=";  //Requeste webpage  
  Link = Link + ADCData;
  Link = Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";                
  client.print(Link);
  delay(100);
  
  //---------------------------------------------------------------------
  //Wait for server to respond with timeout of 5 Seconds
  int timeout=0;
  while((!client.available()) && (timeout < 1000))     //Wait 5 seconds for data
  {
    delay(10);  //Use this with time out
    timeout++;
  }
  //If data is available before time out read it.
  if(timeout < 500)
  {
    while(client.available())
    {
      Serial.println(client.readString()); //Response from ThingSpeak       
    }
  }
  else
  {
     Serial.println("Request timeout..");
  }

  delay(5000);  //Read Web Page every 5 seconds       

 //Make GET request as pet HTTP GET Protocol format
  ADCData="";
  adcvalue=HB;  //Read Analog value of LDR
  ADCData = String(adcvalue);   //String to interger conversion
  Link="";
  Link="GET /update?api_key="+apiKey+"&field4=";  //Requeste webpage  
  Link = Link + ADCData;
  Link = Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";                
  client.print(Link);
  delay(100);
  
  //---------------------------------------------------------------------
  //Wait for server to respond with timeout of 5 Seconds
  timeout=0;
  while((!client.available()) && (timeout < 1000))     //Wait 5 seconds for data
  {
    delay(10);  //Use this with time out
    timeout++;
  }
  //If data is available before time out read it.
  if(timeout < 500)
  {
    while(client.available())
    {
      Serial.println(client.readString()); //Response from ThingSpeak       
    }
  }
  else
  {
     Serial.println("Request timeout..");
  }

  delay(5000);  //Read Web Page every 5 seconds

  if(HB<=60)
  {
    Serial.print("AT+CMGS=");
    Serial.print('"');
    for(temp=0;temp<10;temp++)
      Serial.write(fix_MOB[temp]);
    Serial.println('"');
    delay(2000);
    Serial.println("HeartBeats are low");
    Serial.print("HeartBeats=");
    Serial.print(HB);
    Serial.println(" BPM");
    delay(2000);
    Serial.write(0x1A);
    delay(5000);
  }

  if(SP<=85)
  {
    Serial.print("AT+CMGS=");
    Serial.print('"');
    for(temp=0;temp<10;temp++)
      Serial.write(fix_MOB[temp]);
    Serial.println('"');
    delay(2000);
    Serial.println("SP02 low");
    Serial.print("SPO2 LEVEL=");
    Serial.print(SP);
    Serial.println(" %");
    delay(2000);
    Serial.write(0x1A);
    delay(5000);
  }
}  

void GSM_Init()
{
  char temp;
  Serial.println("AT");
  delay(1000);  
  Serial.println("ATE0");
  delay(1000);
  Serial.println("AT+CLIP=1");
  delay(1000);
  Serial.println("AT+CNMI=2,2,0,0,0");
  delay(1000);
  Serial.println("AT+CMGF=1");
  delay(1000);
  Serial.print("AT+CMGS=");
  Serial.print('"');
  for(temp=0;temp<10;temp++)
    Serial.write(fix_MOB[temp]);
  Serial.println('"');
  delay(2000);
  Serial.println("System is ON");
  Serial.write(0x1A);
  delay(2000);
}
