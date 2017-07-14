#include "ESP8266WiFi.h"

const char mySSID[] = "yourSSIDhere";
const char myPSK[] = "yourPWDhere";
const int IP[4] = {127, 0, 0, 1}; // your Server IP Address here
const int PORT = 4000;

IPAddress serverIpAddress(IP[0], IP[1], IP[2], IP[3]);
ESP8266Client client;

void errorLoop(int error) {
  Serial.print(F("Error: ")); Serial.println(error);
  Serial.println(F("Looping forever."));
  for(;;)
    ;
}

void serialTrigger(String message) {
  Serial.println();
  Serial.println(message);
  Serial.println();
  while(!Serial.available())
    ;
  while(Serial.available())
    Serial.read();
}

void initializeESP8266() {
  int test = esp8266.begin();
  if(test != true) {
    Serial.println(F("Error talking to ESP8266."));
    errorLoop(test);
  }
  Serial.println(F("ESP8266 Shield Present"));
}

void connectESP8266() {
  int retVal = esp8266.getMode();
  if(retVal != ESP8266_MODE_STA) {
    retVal = esp8266.setMode(ESP8266_MODE_STA);
    if(retVal < 0) {
      Serial.println(F("Error setting mode."));
      errorLoop(retVal);
    }
  }
  Serial.println(F("Mode set to station"));

  retVal = esp8266.status();
  if(retVal <= 0) {
    Serial.print(F("Connecting to "));
    Serial.println(mySSID);

    retVal = esp8266.connect(mySSID, myPSK);
    if(retVal < 0) {
      Serial.println(F("Error connecting"));
      errorLoop(retVal);
    }
  }
}

void displayConnectInfo() {
  char connectedSSID[24];
  memset(connectedSSID, 0, 24);
  int retVal = esp8266.getAP(connectedSSID);
  if(retVal > 0) {
    Serial.print(F("Connected to: "));
    Serial.println(connectedSSID);
  }

  IPAddress myIP = esp8266.localIP();
  Serial.print(F("My IP: ")); Serial.println(myIP);
}

void chatToServer() {
  Serial.println("\nTo Server...");
  uint8_t recvMsg, sendMsg;
  size_t recvSize, sendSize;

  while(1) {
    delay(10);
    
    sendMsg = Serial.read();
    sendSize = sizeof(sendMsg);
    if((int)sendMsg >= 32 && sendMsg <= 127) {
      Serial.println((char)sendMsg);
      client.write(&sendMsg, sendSize);
      Serial.flush();
      client.flush();
    }

    recvMsg = client.read();
    if((int)recvMsg >= 32 && recvMsg <= 127) {
      Serial.print((char)recvMsg);
    }
  }

  if(!client.connected()) {
    Serial.println("\ndisconnecting.");
    delay(1000);
    client.stop();
    esp8266.disconnect();
    return;
  }
}

void setup() {
  Serial.begin(115200);

  initializeESP8266();
  connectESP8266();
  displayConnectInfo();

  serialTrigger(F("Press any key to connect client."));

  Serial.println("Start Demo");
  
  int retVal = client.connect(serverIpAddress, PORT, 0);
  if(retVal <= 0) {
    Serial.println(F("Failed to connect to server."));
    client.flush();
  }
  Serial.println("Connection successful");
}

void loop() {
  chatToServer();
}
