// MSX SC5 file input to PCL5 WiFi printer output for ESP32-WROOM-DA
// tangent.space 2023

// Network credentials
const char* ssid = "Jasper's Realm";
const char* password = "5541006C9D";
const char* ip = "192.168.50.50";
const int port = 9100;

extern "C"
{
  #include "sc5print.h"
}

#include <WiFi.h>

void ISR();

char inBuffer[INPUT_BUFFER_SIZE] = {};
int inIndex;

// ESP32

void setup()
{
  pinMode(OE_n, GPIO_MODE_OUTPUT_OD); // 0 = Hi-Z, 1 = open drain to GND
  pinMode(PSTB_n, INPUT); // MSX Strobe signal

  for (int i = 0; i < 8; i++) // D0-D7 data bus
  {
    pinMode(D0 + i, INPUT);
  }

  REG_WRITE(GPIO_ENABLE_W1TC_REG, 0xFF << D0); // 8-bit data input register

  Serial.begin(9600);
  while (!Serial);

  initWiFi(); // Connect to specified WLAN

  digitalWrite(OE_n, 1); // Enable /OE on SN74LVC245

  reset(); // Start waiting for data from MSX
}

void reset()
{
  inIndex = 0;
  attachInterrupt(PSTB_n, ISR, FALLING); // Interrupt when STROBE signal is low
}

void IRAM_ATTR ISR() // Interrupt Service Routine
{
  inBuffer[inIndex] = REG_READ(GPIO_IN_REG) >> D0; // Read state of MSX printer parallel data ???
  inIndex++;
}

void loop()
{
  if (inBuffer[0] != 0xFE) // Try to ignore data that does not come from SC5 transfer
  {
    reset();
  }

  if (inIndex >= sizeof inBuffer) // Process contents of input buffer when full
  {
    detachInterrupt(digitalPinToInterrupt(PSTB_n)); // Stop monitoring Strobe signal
    convert(); // Convert contents of input buffer to SC5
    sendWiFi(); // Write converted PCL to IP and port
    reset(); // Resume waiting for data from MSX
  }
}

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