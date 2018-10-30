#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

HTTPClient http;

const char *ssid = "Jumpitt Labs";
const char *password = "Jumpitt2015";

bool makeAToggle = false;

void setup() {
	Serial.begin(115200);
	delay(10);
	Serial.println("\r\n");

	pinMode(D3, INPUT);

	http.begin("http://192.168.2.226/toggle");

	attachInterrupt(digitalPinToInterrupt(D3), interruption, FALLING);

	startWiFi();
}

void interruption(){
	detachInterrupt(digitalPinToInterrupt(D3));
	Serial.println("button pressed!");
	makeAToggle = true;
}

void loop() {
	if(makeAToggle == true){
		Serial.println("getting wait!");
		http.GET();
		http.end();
		Serial.println("done!");
		makeAToggle = false;
		delay(200);
		attachInterrupt(digitalPinToInterrupt(D3), interruption, FALLING);
	}
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
}