#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <mDNSResolver.h>

WiFiUDP udp;
mDNSResolver::Resolver resolver(udp);
HTTPClient http;
ESP8266WiFiMulti wifiMulti;

#define TARGET_HOSTNAME "uxrelay1.local"

String targetHostName = "";

const char *mdnsName = "uxbutton1";
String targetIp = "";

bool makeAToggle = false;


void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println("\r\n");

  startWiFi();
  startHttpClient();

  pinMode(D3, INPUT);
  attachInterrupt(digitalPinToInterrupt(D3), interruption, FALLING);

}

void loop() {
  if(makeAToggle && WiFi.status() == WL_CONNECTED){
    HTTPClient http;

    http.begin(targetHostName);
    int httpcode = http.GET();

    Serial.println(httpcode);

    if(httpcode > 0){
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.println(http.errorToString(httpcode).c_str());
    }

    http.end();
    Serial.println("done!");
    makeAToggle = false;
    delay(1000);
    attachInterrupt(digitalPinToInterrupt(D3), interruption, FALLING);
  }
}

String resolveMDNS(){
  IPAddress answer = resolver.search(TARGET_HOSTNAME);
  if(answer == IPADDR_NONE) {
    Serial.println("no answer found");
  } else {
    return String("http://") + IpAddress2String(answer) + String("/toggle");
  }
}

void startWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect (true);
  WiFi.setAutoReconnect (true);
  wifiMulti.addAP("Jumpitt Labs", "Jumpitt2015");
  wifiMulti.addAP("VTR-7236151", "hw9wcXhtHsm4");

  Serial.print("Connecting Wifi...");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void startHttpClient(){
  Serial.print("starting http at =>");
  targetHostName = resolveMDNS();
  Serial.println(targetHostName);
  http.begin(targetHostName);
}

void interruption(){
 detachInterrupt(digitalPinToInterrupt(D3));
 makeAToggle = true;
}

String IpAddress2String(const IPAddress& ipAddress){
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}