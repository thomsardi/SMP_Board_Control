#include <Arduino.h>
#include <FastLED.h>
#include <ShiftRegister74HC595.h>
#include <AsyncJson.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <DataType.h>
#include <WebServerHandler.h>
#include <DataType.h>
#include <Vector.h>
#include <RelayControl.h>
#include <LedControl.h>

const int numberOfShiftRegister = 4;
const int numberOfLed = 12;
const int din = 18; //data pin
const int stcp = 19; //latch pin
const int shcp = 21; //clock pin

const int ledPin = 22; //led data pin

const int internalLed = 2; //internal led pin

const char *ssid = "RnD_Sundaya";
const char *password = "sundaya22";
const String hostName = "RnD-SMP-Board";

unsigned long lastReconnectMillis = 0;
int reconnectInterval = 5000;

AsyncWebServer server(80);

// create a global shift register object
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<numberOfShiftRegister> sr(din, shcp, stcp);

CRGB leds[numberOfLed];

WebServerHandler webServerHandler;
LineData lineData[64];
Command command[16];
Vector<Command> commandList;

RelayControl relayControl;
LedControl ledControl;

void relayTest1()
{
  for (size_t i = 1; i <= 12; i++)
  {
    int pin = relayControl.write(i, HIGH);
    sr.set(pin, HIGH);
    delay(20);
    sr.set(pin, LOW);
    delay(20);
    ledControl.write(i, HIGH, leds);
    FastLED.show();
    delay(200);
    pin = relayControl.write(i, LOW);
    sr.set(pin, HIGH);
    delay(20);
    sr.set(pin, LOW);
    delay(20);
    ledControl.write(i, LOW, leds);
    FastLED.show();
    delay(200);
  }
  
}

void relayTest2()
{
  int pin;
  for (size_t i = 1; i <= 12; i++)
  {
    if (i%2)
    {
      pin = relayControl.write(i, HIGH);
      sr.set(pin, HIGH);
      delay(20);
      sr.set(pin, LOW);
      delay(20);
      ledControl.write(i, HIGH, leds);
      FastLED.show();
    }
    else
    {
      pin = relayControl.write(i, LOW);
      sr.set(pin, HIGH);
      delay(20);
      sr.set(pin, LOW);
      delay(20);
      ledControl.write(i, LOW, leds);
      FastLED.show();
    }
  }
}

void relayTest3()
{
  int pin;
  for (size_t i = 1; i <= 12; i++)
  {
    if (i%2)
    {
      pin = relayControl.write(i, LOW);
      sr.set(pin, HIGH);
      delay(20);
      sr.set(pin, LOW);
      delay(20);
      ledControl.write(i, LOW, leds);
      FastLED.show();
    }
    else
    {
      pin = relayControl.write(i, HIGH);
      sr.set(pin, HIGH);
      delay(20);
      sr.set(pin, LOW);
      delay(20);
      ledControl.write(i, HIGH, leds);
      FastLED.show();
    }
  }
}

void resetAll()
{
  int pin;
  for (size_t i = 1; i <= 12; i++)
  {
    pin = relayControl.write(i, LOW);
    sr.set(pin, HIGH);
    delay(20);
    sr.set(pin, LOW);
    delay(20);
    ledControl.write(i, LOW, leds);
    FastLED.show();
  }
}

void setAll()
{
  int pin;
  for (size_t i = 1; i <= 12; i++)
  {
    pin = relayControl.write(i, HIGH);
    sr.set(pin, HIGH);
    delay(20);
    sr.set(pin, LOW);
    delay(20);
    ledControl.write(i, HIGH, leds);
    FastLED.show();
  }
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(internalLed, HIGH);
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS 1: ");
  Serial.println(WiFi.dnsIP(0));
  Serial.print("DNS 2: ");
  Serial.println(WiFi.dnsIP(1));
  Serial.print("Hostname: ");
  Serial.println(WiFi.getHostname());
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Wifi Connected");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  digitalWrite(internalLed, LOW);
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
}

void setup() {
  commandList.setStorage(command);
  pinMode(internalLed, OUTPUT);
  // put your setup code here, to run once:
  FastLED.addLeds<WS2812, ledPin, GRB>(leds, numberOfLed);
  FastLED.setBrightness(20);
  Serial.begin(115200);
  WiFi.disconnect(true);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.mode(WIFI_MODE_NULL);
  delay(100);
  WiFi.setHostname(hostName.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiStationConnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_GOT_IP);
  // WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.begin(ssid, password);
  int timeout = 0;
  while(timeout < 10)
  {
    if (WiFi.status() != WL_CONNECTED)
    {
        delay(100);
        timeout++;
    }
    else
    {
        break;
    }
  }

  if (timeout < 10)
  {
    digitalWrite(internalLed, HIGH);
  }
  else
  {
    digitalWrite(internalLed, LOW);
  }

  server.on("/get-line-data", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    Serial.println("GET Request Line Data");
    size_t arrSize = sizeof(lineData) / sizeof(lineData[0]);
    String response;
    if(webServerHandler.processLineDataRequest(request, response, lineData, arrSize) > 0)
    {
      request->send(200, "application/json", response);
      Serial.println(response);
    }
    else
    {
      request->send(400);
    }
  });
  
  AsyncCallbackJsonWebHandler *setRelayHandler = new AsyncCallbackJsonWebHandler("/relay", [](AsyncWebServerRequest *request, JsonVariant &json)
  {
    Serial.println("POST Request Line Data");
    String response;
    String input = json.as<String>();
    if (webServerHandler.processRelayRequest(input, response, commandList) > 0)
    {    
      request->send(200, "application/json", response);
    }
    else
    {
      request->send(400);
    }
    });

  server.addHandler(setRelayHandler);
  server.begin();
}

void loop() {
  // put your main code here, to run repeatedly:

  if ((WiFi.status() != WL_CONNECTED) && (millis() - lastReconnectMillis >= reconnectInterval)) {
    Serial.println("WiFi Disconnected");    
    digitalWrite(internalLed, LOW);
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    lastReconnectMillis = millis();
  }

  if(commandList.size() > 0)
  {
    Command c = commandList.front();
    Serial.println("==================");
    switch (c.type)
    {
      case RELAY:
        // Serial.println("Command Type : RELAY");
        for(int i = 0; i < c.relayData.number; i++)
        {
          int pin = relayControl.write(c.relayData.lineList[i], c.relayData.valueList[i]);
          Serial.println("Pin : " + String(pin));
          sr.setNoUpdate(pin, HIGH);
          ledControl.write(c.relayData.lineList[i], c.relayData.valueList[i], leds);
        }
        sr.updateRegisters();
        // Serial.println("Set To High");
        delay(20);
        for(int i = 0; i < c.relayData.number; i++)
        {
          int pin = relayControl.write(c.relayData.lineList[i], c.relayData.valueList[i]);
          sr.setNoUpdate(pin, LOW);
        }
        sr.updateRegisters();
        delay(20);
        // Serial.println("Set To Low");
        FastLED.show();
      break;
    }
    // Serial.println("Line : " + String(c.line));
    // Serial.println("Value : " + String(c.value));    
    commandList.remove(0);
    // digitalWrite(internalLed, c.value);
  }

  relayTest1();
  delay(1000);
  relayTest2();
  delay(1000);
  relayTest3();
  delay(1000);
  resetAll();
  delay(1000);
  setAll();
  delay(1000);
  resetAll();
  delay(1000);
  // resetAll();
  // delay(1000);
  // sr.set(7, HIGH);
  // delay(20);
  // sr.set(7, LOW);
  // delay(2000);

  // sr.set(6, HIGH);
  // delay(20);
  // sr.set(6, LOW);
  // delay(2000);

  // sr.set(11, HIGH);
  // delay(20);
  // sr.set(11, LOW);
  // delay(2000);

  // sr.set(10, HIGH);
  // delay(20);
  // sr.set(10, LOW);
  // delay(2000);

  // sr.set(23, HIGH);
  // delay(20);
  // sr.set(23, LOW);
  // delay(2000);

  // sr.set(22, HIGH);
  // delay(20);
  // sr.set(22, LOW);
  // delay(2000);

  // sr.set(27, HIGH);
  // delay(20);
  // sr.set(27, LOW);
  // delay(2000);

  // sr.set(26, HIGH);
  // delay(20);
  // sr.set(26, LOW);
  // delay(2000);

  // sr.set(28, 1);
  // delay(20);
  // sr.set(28, 0);
  // delay(1000);

  // sr.set(27, 1);
  // delay(20);
  // sr.set(27, 0);
  // delay(1000);

  /**
   * Test 1
   * Relay will on one by one so do led according to relay state
  */
  // relayTest1();
  // delay(1000);
  
  /**
   * Test 2
   * All relay and led will on
  */
  // relayTest2();
  // delay(1000);
  
  /**
   * Test 3
   * All relay and led will off
  */
  // relayTest3();
  // delay(1000);

}