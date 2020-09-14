#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>

extern "C" {
#include <osapi.h>
#include <os_type.h>
}

#include "config.h"

static os_timer_t intervalTimer;
static os_timer_t intervalTimer1;
char newcommand[200];
char receivedmsg[100];
char tosend[30];
WiFiClient clientz;
static void replyToServer(void* arg) {
	AsyncClient* client = reinterpret_cast<AsyncClient*>(arg);

	// send reply
	if (client->space() > 32 && client->canSend()) {
		char message[32];
		sprintf(message, "*99*1##");
		client->add(message, strlen(message));
		client->send();
	}
}

/* event callbacks */
static void handleData(void* arg, AsyncClient* client, void *data, size_t len) {
  strncpy(newcommand, (char*)data, len);
  newcommand[len+1] = '\0';
  int j = 0;
  for(int i = 0; i<=len;i++){
    receivedmsg[j] = newcommand[i];
    if(newcommand[i] == '#' && newcommand[i-1] == '#' && i>0){
      receivedmsg[j+1] = '\0';
      Serial.println(receivedmsg);
      if(String(receivedmsg) == "*1*1*44##") digitalWrite(2, LOW);
      if(String(receivedmsg) == "*1*0*44##") digitalWrite(2, HIGH);
      j = -1;
    }    
    j++;
  }

	os_timer_arm(&intervalTimer, 2000, true); // schedule for reply to server at next 2s
}

void onConnect(void* arg, AsyncClient* client) {
	Serial.printf("\n client has been connected to %s on port %d \n", SERVER_HOST_NAME, TCP_PORT);
	replyToServer(client);
}

static void handleDisconnect(void* arg, AsyncClient* client) {
  Serial.printf("\n client %s disconnected \n", client->remoteIP().toString().c_str());
  ESP.restart();
}
void setup() {
	Serial.begin(115200);
	delay(20);
  
	// connects to access point
	WiFi.mode(WIFI_STA);
	WiFi.begin(SSID, PASSWORD);
  int i = 0;
	while (WiFi.status() != WL_CONNECTED) {
		Serial.print('.');
		delay(500);
    if(i==30) ESP.restart();
    i++;
	}
  pinMode(2, OUTPUT);
  pinMode(0, INPUT_PULLUP);
	AsyncClient* EventSession = new AsyncClient;
	EventSession->onData(&handleData, EventSession);
	EventSession->onConnect(&onConnect, EventSession);
  EventSession->onDisconnect(&handleDisconnect, NULL);
	EventSession->connect(SERVER_HOST_NAME, TCP_PORT);

	os_timer_disarm(&intervalTimer);
	os_timer_setfn(&intervalTimer, &replyToServer, EventSession);
  if (!clientz.connect(SERVER_HOST_NAME, TCP_PORT)) {
      Serial.println("connection failed");
      delay(5000);
      ESP.restart();
  }
  if (clientz.connected()) {
      clientz.println("*#1*0##");
      clientz.stop();
  }
}
bool lastState = HIGH;
void loop() {
  bool buttonState = digitalRead(0);
  if (buttonState != lastState) {
    if(buttonState == LOW) strcpy(tosend,"*1*1*44##");
    if(buttonState == HIGH) strcpy(tosend,"*1*0*44##");
    lastState = buttonState;
    if (!clientz.connect(SERVER_HOST_NAME, TCP_PORT)) {
      Serial.println("connection failed");
      delay(1000);
      return;
    }
    if (clientz.connected()) {
      clientz.println(tosend);
      clientz.stop();
    }
  }
}
