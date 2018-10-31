#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiUdp.h>
#include <mDNSResolver.h>

using namespace mDNSResolver;

WiFiUDP udp;
Resolver resolver(udp);
HTTPClient http;


IPAddress hostIp = resolver.search("test.local");
int hostPort = 80;

const char *ssid = "VTR-7236151";
const char *password = "hw9wcXhtHsm4";

bool makeAToggle = false;

void setup() {
	Serial.begin(115200);
	delay(10);
	Serial.println("\r\n");

	pinMode(D3, INPUT);

	http.begin(hostIp.toString(), hostPort, String("/toggle"));

	startWiFi();

	IPAddress ip = resolver.search("test.local");
	if(ip != INADDR_NONE) {
		Serial.print("Resolved: ");
		Serial.println(ip);
	} else {
		Serial.println("Not resolved");
	}
}

void interruption(){
	detachInterrupt(digitalPinToInterrupt(D3));
	Serial.println("button pressed!");
	makeAToggle = true;
}

void loop() {
	do {
		resolver.loop();
		Serial.println("getting wait!");
		http.GET();
		http.end();
		Serial.println("done!");
		makeAToggle = false;
		delay(200);
		attachInterrupt(digitalPinToInterrupt(D3), interruption, FALLING);
	} while(makeAToggle);
}

void startWiFi() {
	WiFi.begin(ssid, password);
	Serial.print("Connecting to ");
	Serial.print(ssid);
	Serial.println(" ...");

	int i = 0;
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print('.');
	}

	Serial.println('\n');
	Serial.println("Connection established!");
	Serial.print("IP address:\t");
	Serial.println(WiFi.localIP());
	resolver.setLocalIP(WiFi.localIP());
}