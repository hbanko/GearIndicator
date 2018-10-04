#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ArduinoOTA.h>
#include "LedControl.h"
#include "FS.h"
#include "wifi.h"

const int led = 13;
const int brightnessPin = 0; 
const int menuPin = 4; 

int connected=1;
volatile int buttonState = 0;         // variable for reading the pushbutton st
int brightness=12;

ESP8266WebServer server(80);

/*
 ***** These pin numbers are working for ESP8266 *****
 pin 7 is connected to the DataIn GPIO13
 pin 5 is connected to the CLK GPIO14
 pin 8 is connected to CS GPIO15
 Last paramater for number of displays. We have only a single MAX72XX.
 */
LedControl lc=LedControl(13,14,15,1);

void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      lc.setLed(0,i,j, bitRead(row, j));
    }
  }
}

void showCharacter(int c) {

  // Definitions for the numbers and signs
  // https://xantorohara.github.io/led-matrix-editor/
  
  const uint64_t IMAGES[] = {
  0x6363737b6f676300,
  0x7e1818181c181800,
  0x7e060c3060663c00,
  0x3c66603860663c00,
  0x30307e3234383000,
  0x3c6660603e067e00,
  0x3c66663e06663c00,
  0x3c66666e76663c00,
  0x081c361b6c361c08,
  0x3c72f1f18f8f4e3c,
  0x81c3e7f7f772200,
  0x007e425a5a427e00,
  0x0000001818000000,
  };
  
  lc.clearDisplay(0);
  displayImage(IMAGES[c]);
 
}

// This function reads 180 values in 3 minutes
// One LED flash for one value read

void learn() {
  int adcvalue=0;
  int adc_average=0;
  showCharacter(12);
  File f = SPIFFS.open("/gears.txt", "w");
  if (!f) {
      Serial.println("file open failed");
  }
  Serial.println("====== Writing to SPIFFS file =========");
  for(int i = 0; i < 180; i++) {
    digitalWrite(led, 0);
    for(int x = 0; x < 5; x++) {
      adcvalue = analogRead(A0);
      delay(40);
      adc_average = adc_average + adcvalue;
    }
    adc_average = adc_average / 5;
    f.println(adc_average);
    digitalWrite(led, 1);
    Serial.println(adc_average);
    adc_average = 0;
  }
  f.close();
  lc.clearDisplay(0);
}

void show_gear() {
  int adcvalue=0;
  int adc_average=0;
 for(int x = 0; x < 10; x++) {
    adcvalue = analogRead(A0);
    delay(40);
    adc_average = adc_average + adcvalue;
  }
   adcvalue = adc_average / 10;
   
  if(adcvalue >= 299 && adcvalue<= 305) {  // Neutral
    showCharacter(0);
  } else if (adcvalue >= 99 && adcvalue<= 105 ) { // 1st Gear
    showCharacter(1);
  } else if (adcvalue >= 118 && adcvalue<= 129) { // 2nd Gear
    showCharacter(2);
  } else if (adcvalue >= 159 && adcvalue<= 170) { // 3rd Gear
    showCharacter(3);
  } else if (adcvalue >= 203 && adcvalue<= 208) { // 4th Gear
    showCharacter(4);
  } else if (adcvalue >= 250 && adcvalue<= 265) {
    showCharacter(5);
  } else if (adcvalue >= 270 && adcvalue<= 285) {
    showCharacter(6);
  } 
}
String show () {
  String gears;
  showCharacter(12);
   File f = SPIFFS.open("/gears.txt", "r");
   if (!f) {
      Serial.println("file open failed");
  }  Serial.println("====== Reading from SPIFFS file =======");
   for (int i=1; i<=180; i++){
    String s=f.readStringUntil('\n');
    Serial.print(i);
    Serial.print(":");
    Serial.println(s);
    gears = gears + String(i) + ": "+s+"\n";
  }
   lc.clearDisplay(0);
  return(gears);
}

void displayDemo() {
  for (int i = 0; i < 13; i++) {
  showCharacter(i);
  delay(2000); }  
  lc.clearDisplay(0);
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", "<html><head><title>Gear Indicator</title></head><body><h1>Hello from GearIndicator!</h1>Please use the following options: <br><br>/learn : Reads the voltages from the\\ 
  input line and stores them<br> /show : Display the values that were read in learn <br> /demo : Shows all characters and signs <br> /reset : Deletes the learned values</body></html>");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup() {
  int conn_attempt = 0;
  Serial.begin(115200);
  Serial.println("Booting");

   // Initializing the display and set it to medium brighntess
  lc.shutdown(0,false);
  lc.setIntensity(0,brightness);
  lc.clearDisplay(0);
  Serial.println("Display initialized");

  
  Serial.println("Attempting WiFi connect");
  showCharacter(12);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  delay(500);
  
  if (WiFi.waitForConnectResult() == WL_CONNECTED)
  {
    Serial.println(F("WiFi connected"));
    connected=1;
  } else
    connected=0;
  
  lc.clearDisplay(0);
    
  if(connected == 1) {
     Serial.println("We are online. Activating OTA.");
    ArduinoOTA.onStart([]() {
      Serial.println("Start");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\nEnd");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
    ArduinoOTA.begin();
    Serial.println("Ready");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  
    if (MDNS.begin("esp8266")) {
      Serial.println("MDNS responder started");
    }
  
    server.on("/", handleRoot);
  
    server.on("/demo", [](){
      displayDemo();
    });
  
    server.on("/learn", [](){
      server.send(200, "text/plain", "Learning mode started.");
      learn();
    });
    server.on("/show", [](){
      // server.send(200, "text/plain", "Recorded ADC values below:");
      server.send(200, "text/plain", show());
    });
     server.on("/reset", [](){
      server.send(200, "text/plain", "Deleting file with ADC values.");
      SPIFFS.remove("/gears.txt");
      server.send(200, "text/plain", "Done");
    });
    server.onNotFound(handleNotFound);
    server.serveStatic("/gears", SPIFFS, "/gears.txt");
    server.begin();
    Serial.println("HTTP server started");
  }
  SPIFFS.begin();

  if (!SPIFFS.exists("/formatComplete.txt")) {
    Serial.println("Please wait 30 secs for SPIFFS to be formatted");
    SPIFFS.format();
    Serial.println("Spiffs formatted");
   
    File f = SPIFFS.open("/formatComplete.txt", "w");
    if (!f) {
        Serial.println("file open failed");
    } else {
        f.println("Format Complete");
    }
  } else {
    Serial.println("SPIFFS is formatted. Moving along...");
  }

  // Interrupt setup for brightness button
  pinMode(brightnessPin, INPUT);
  attachInterrupt(0, brightness_btn, RISING);

  // Interrupt setup for menu button
  pinMode(menuPin, INPUT);
  attachInterrupt(4, menu_btn, RISING);
  // Startup Animation
  for(int i=0; i < 5;i++) {
    showCharacter(12);
    delay(250);
    showCharacter(11);
    delay(250);
     lc.clearDisplay(0);
    delay(250);
  }
  
  // Display Suzuki Logo
  showCharacter(8);
  delay(250);
}

void brightness_btn() {
  buttonState = digitalRead(brightnessPin);
    if(buttonState == HIGH) {
    
    if(brightness == 12)
      brightness=5;
    else
      brightness=12;   
  }
  lc.setIntensity(0,brightness);
}

void menu_btn() {
  // test - just clear screen
  lc.clearDisplay(0);
}

void loop() {
  // While riding, we might not have wireless
  if(connected == 1) {
    ArduinoOTA.handle();
    server.handleClient();
  } 
  // Main function - reading ADC and displaying gear number
  show_gear();
}
