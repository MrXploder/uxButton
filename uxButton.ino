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

const char *ssid = "Jumpitt Labs";
const char *password = "Jumpitt2015";
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
  if(makeAToggle){
    Serial.println("getting wait!");
    http.GET();
    http.end();
    Serial.println("done!");
    makeAToggle = false;
    delay(200);
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
  Serial.print("starting http at ");
  Serial.println(resolveMDNS());
  http.begin(resolveMDNS());
}

bool resolve_mdns_service(char* service_name, char* protocol, char* desired_host, IPAddress* ip_addr, uint16_t *port_number) {
  Serial.println("Sending mDNS query");
  int n = MDNS.queryService(service_name, protocol);
  Serial.printf("mDNS query got %d results\n", n);

  if(n == 0) {
    Serial.println("no services found");
  } else {
    for (int i = 0; i < n; ++i) {
      if(strcmp(MDNS.hostname(i).c_str(), desired_host) == 0) {
        *ip_addr = MDNS.IP(i);
        *port_number = MDNS.port(i);
        return true;
      }
    }
  }
  return false;
}

void interruption(){
 detachInterrupt(digitalPinToInterrupt(D3));
 Serial.println("button pressed!");
 makeAToggle = true;
}

String IpAddress2String(const IPAddress& ipAddress){
  return String(ipAddress[0]) + String(".") +\
  String(ipAddress[1]) + String(".") +\
  String(ipAddress[2]) + String(".") +\
  String(ipAddress[3])  ;
}