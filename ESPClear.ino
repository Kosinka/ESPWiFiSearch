#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

IPAddress local_IP (192,168,4,16);
IPAddress gateway (192,168,4,9);
IPAddress subnet (255,255,255,0);
ESP8266WebServer server(80);
int addres = 0;
bool setFlag = false;
String webPage= "";
String SSIDStr = "";
void handleRoot() {
  server.send(200, "text/html", webPage);
}

void setup() {
  Serial.begin(115200);
  initFunc();
}

void loop() {
  server.handleClient();
}

void initFunc(){
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
  EEPROM.end();
  
}

void loadWiFiSettings(String ssid, String pass){
  bool flag = false;
  WiFi.begin(ssid,pass);
  for(int i = 0;i<=50;i++){
    if(WiFi.status() != WL_CONNECTED){
      delay(500);
    }else{
      flag = 1;
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
}

void WIFIAPStart(){  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP("NodeMCU","",1);
  delay(100);
  webPage+= "<h1>NodeMCU Wi-Fi_AP</h1><br> Networks<br>";
  int numberOfNetworks = WiFi.scanNetworks();
  if (numberOfNetworks==0){
    webPage += "Networks not found.";
  }else
  {
    webPage+="Networks near: <br>";
    for (int i = 0; i < numberOfNetworks; ++i) {
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
    }
    webPage+="* - password protected AP";
  server.on("/", handleRoot);
  server.on("/WIFIAP",handleWIFI);
  server.on("/wifiProcessing",wifiProcessing);
  server.begin(); 
}
}

void handleWIFI(){
  processPostData();
  int numberOfRequests = server.args();
  String postRequest = server.arg(numberOfRequests-1);
  int APNumber = (postRequest.charAt(8) - '0');
  webPage = "Connecting to ";
  SSIDStr = WiFi.SSID(APNumber);
  webPage += SSIDStr;
  webPage += "<br>";
  webPage += "<h1>Enter password:</h1><br><h2>Password: <form action=\"wifiProcessing\" method=\"post\"> <input type=\"text\" name = \"passText\" id = \"WIFI1Pass\" size = \"32\" maxlength = \"32\"><button>Submit</button></form></h2>";
  server.send(200,"text/html",webPage);
}

void wifiProcessing(){
  String password = server.arg("passText");
  webPage = "<meta http-equiv=\"refresh\" content=\"1;URL=/\" />";
  server.send(200,"text/html",webPage);
  loadWiFiSettings(SSIDStr, password);
}
