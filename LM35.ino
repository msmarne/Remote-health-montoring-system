#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "Mrunal";  //ENTER YOUR WIFI SETTINGS <<<<<<<<<
const char *password = "mrunal@1234";

//Web address to read from
const char *host = "api.thingspeak.com";
String apiKey = "5S00PH4OPJK0L43H";  //ENTER YOUR API KEY <<<<<<<<<<<
unsigned char fix_MOB[10]={'7','0','1','6','4','2','2','4','0','8'};

void setup()
{ 
  delay(1000);
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);        //This line hides the viewing of ESP as wifi hotspot
  //WiFi.mode(WIFI_AP_STA);   //Both hotspot and client are enabled
  //WiFi.mode(WIFI_AP);       //Only Access point
  
  WiFi.begin(ssid, password);     //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  GSM_Init();

}
void loop() 
{
  float ref_mV=3300,test_mV;
  unsigned  int resolution=1023;
  unsigned int mV, temp_val; 
  unsigned int t1,t2,digval;
  unsigned char temp;

  digval=analogRead(A0);
  Serial.println(digval);
  test_mV=((digval * ref_mV)/resolution);
  mV=(int)test_mV;
  Serial.print("mV=");
  Serial.println(mV);
  t1=mV/10;
  t2=mV%10;
  Serial.print("Temperature=");
  Serial.print(t1);
  Serial.print('.');
  Serial.print(t2);
  Serial.println(" Degree C");
//  delay(3000);
  WiFiClient client;          
  const int httpPort = 80; //Port 80 is commonly used for www
 //---------------------------------------------------------------------
 //Connect to host, host(web site) is define at top 
 if(!client.connect(host, httpPort)){
   Serial.println("Connection Failed");
   delay(300);
   return; //Keep retrying until we get connected
 }

  
  String ADCData;
  ADCData = String(t1);   //String to interger conversion
  String Link="GET /update?api_key="+apiKey+"&field3=";  //Requeste webpage  
  Link = Link + ADCData;
  Link = Link + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n";                
  client.print(Link);
  delay(100);

   //Wait for server to respond with timeout of 5 Seconds
  int timeout=0;
  while((!client.available()) && (timeout < 1000))     //Wait 5 seconds for data
  {
    delay(10);  //Use this with time out
    timeout++;
  }

  //---------------------------------------------------------------------
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

  if(t1>=39)
  {
    Serial.print("AT+CMGS=");
    Serial.print('"');
    for(temp=0;temp<10;temp++)
      Serial.write(fix_MOB[temp]);
    Serial.println('"');
    delay(2000);
    Serial.println("Temperature is High");
    Serial.print("Temperature=");
    Serial.print(t1);
    Serial.print('.');
    Serial.print(t2);
    Serial.println(" Degree Celsius");
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

