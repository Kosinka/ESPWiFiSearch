#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
//#include <WiFi.h>

IPAddress local_IP (192,168,4,16);
IPAddress gateway (192,168,4,9);
IPAddress subnet (255,255,255,0);
ESP8266WebServer server(80);
//HTTPClient http;
int addres = 0;
bool setFlag = false;
String webPage= "";
String SSIDStr = "";
void handleRoot() {
  server.send(200, "text/html", webPage);
}

void setup() {
  Serial.print("setup");
  Serial.begin(115200);
  Serial.print("Setup ");
  initFunc();
}

void loop() {
  //Serial.print("loop");
  server.handleClient();
  //int numberOfNetworksNear = WiFi.scanNetworks();
  //if (numberOfNetworksNear != 0)
  //{
    
  //}
}

void initFunc(){
  Serial.print("initFunc ");
  char ssid[32], pass[32];
  EEPROM.begin(512);
  EEPROM.get(0,ssid);
  EEPROM.get(0+sizeof(ssid),pass);
  if(strlen(ssid)==0){
    WIFIAPStart();
  } else
  {
    loadWiFiSettings(ssid, pass);
  }
  Serial.print(" SSID: ");
  Serial.print(ssid);
  Serial.print(" PASS: ");
  Serial.print(pass);
  EEPROM.end();
  
}

//void loadWiFiSettings(char ssid[], char pass[]){
void loadWiFiSettings(String ssid, String pass){
  bool flag = false;
  Serial.print("loadWiFiSettings ");
  WiFi.begin(ssid,pass);
  Serial.print("Connecting...");
  for(int i = 0;i<=50;i++){
    if(WiFi.status() != WL_CONNECTED){
      delay(500);
      Serial.print("Connecting...");
    }else{
      flag = 1;
      Serial.print("Connected! to ");
      Serial.print(ssid);
      Serial.print(" , Local IP: ");
      Serial.println(WiFi.localIP());
    }
  }
  if (flag) saveToEEPROM(ssid,pass);
}

void saveToEEPROM(String ssid, String pass){
  char ssidChar[32];
  char passChar[32];
  ssid.toCharArray(ssidChar, ssid.length()+1);
  pass.toCharArray(passChar, pass.length()+1);
  EEPROM.put(0,ssid);
  EEPROM.put(0+sizeof(ssid),pass);
  EEPROM.commit();  
  Serial.println(ssid);
  Serial.println(pass);
}

void WIFIAPStart(){
  Serial.print("WIFIAPStart");
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("NodeMCU","",1);
  delay(100);
  webPage+= "<h1>NodeMCU Wi-Fi_AP</h1><br> Networks<br>";
  int numberOfNetworks = WiFi.scanNetworks();
  if (numberOfNetworks==0){
    webPage += "Networks not found.";
    Serial.print(" Networks not found. ");
  }else
  {
    Serial.print(" Networks near ");
    webPage+="Networks near: <br>";
    for (int i = 0; i < numberOfNetworks; ++i) {
      Serial.print(" webPageConstructor");
      webPage+= String(i + 1);
      webPage+=": ";
      webPage+=String(WiFi.SSID(i));
      webPage+=" (";
      webPage+=String(WiFi.RSSI(i));
      webPage+=")";
      webPage+=String((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      webPage+="<form action=\"WIFIAP\" method=\"post\"";
      webPage+="method=\"post\">";
      webPage+="<input type = \"submit\" name = \"ButtonAP";
      webPage+=String(i);
      webPage+=" value = \"Connect to ";
      webPage+=String(WiFi.SSID(i));
      webPage+="\"></form><br>";
      //delay(10);
      Serial.print(i);
    }
    webPage+="* - password protected AP";
  
  Serial.print("Before server");
  server.on("/", handleRoot);
  server.on("/WIFIAP",handleWIFI);
  server.on("/wifiProcessing",wifiProcessing);
  server.begin(); 
}
}

void handleWIFI(){
  processPostData();
  Serial.print("handleWIFI : ");
  int numberOfRequests = server.args();
  String postRequest = server.arg(numberOfRequests-1);
  int APNumber = (postRequest.charAt(8) - '0');
  //Serial.print(server.arg("WIFIAP"));
  Serial.println();
  Serial.print(postRequest);
  Serial.println();
  Serial.print(APNumber);
  webPage = "Connecting to ";
  SSIDStr = WiFi.SSID(APNumber);
  webPage += SSIDStr;
  webPage += "<br>";
  webPage += "<h1>Enter password:</h1><br><h2>Password: <form action=\"wifiProcessing\" method=\"post\"> <input type=\"text\" name = \"passText\" id = \"WIFI1Pass\" size = \"32\" maxlength = \"32\"><button>Submit</button></form></h2>";
  server.send(200,"text/html",webPage);
  
}

void wifiProcessing(){
  Serial.print("wifiProcessing");
  String password = server.arg("passText");
  Serial.print(password);
  webPage = "<meta http-equiv=\"refresh\" content=\"1;URL=/\" />";
  server.send(200,"text/html",webPage);
  Serial.print("SSID: "); Serial.print(SSIDStr);
  Serial.print(" Pass: "); Serial.print(password);
  loadWiFiSettings(SSIDStr, password);
}

void processPostData(){
 if (server.args() > 0){
    for (int i = 0; i < server.args();i++){
        Serial.print("POST Arguments: " ); Serial.println(server.args());
        Serial.print("Name: "); Serial.println(server.argName(i));
        Serial.print("Value: "); Serial.println(server.arg(i));
   }
}
}
