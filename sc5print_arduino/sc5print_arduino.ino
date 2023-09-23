// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

// Network credentials
const char* ssid = "Jasper's Realm";
const char* password = "5541006C9D";
const char* ip = "192.168.50.50";
const int port = 9100;

#include "sc5print.h"
#include <WiFi.h>

int inIndex;

// ESP32 

void setup()
{
  pinMode (PSTB_n, INPUT);
  pinMode (D7, INPUT);
  pinMode (D6, INPUT);
  pinMode (D5, INPUT);
  pinMode (D4, INPUT);
  pinMode (D3, INPUT);
  pinMode (D2, INPUT);
  pinMode (D1, INPUT);
  pinMode (D0, INPUT);

  Serial.begin(9600);
  while (!Serial);
  
  initWiFi();

  inIndex = 0;

  attachInterrupt(PSTB_n, ISR, FALLING); // Interrupt when STROBE signal is low
}

void IRAM_ATTR ISR() // Interrupt Service Routine
{
  // get state of data pins and assemble byte
  // write byte to input buffer 
  // check if input buffer is full 
  // detachInterrupt() to ignore further interrupts
  // convert if full
  // send to printer & reset
}

void loop() {}

void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED);
  Serial.println(WiFi.localIP());
}

void sendWiFi()
{
  WiFiClient client;
  if (client.connect(ip, port))
  {
    if (client.connected())
    {
      client.write(outBuffer, OUTPUT_BUFFER_SIZE);
    }
  }
}

