/*
   Kill switch for xbox360 Power supply 

   Code is for ESP32-C3 Miniboard, select ESP32C3 Dev Module in Arduino

   Wiring for ESP32-C3 miniboard:

   5v - Xbox 5v SB (standby) Red cable to XBOX
   GND - Xbox ground Black cable to XBOX
   GPIO 1, Pin 1 - Input from Xbox, blue cable, coming from console
   GPIO 2, Pin 2 - Output to the XBOX PSU blue cable connection into the PSU PCB.
*/

#include <WiFi.h>
#include <WiFiUdp.h>

#include "/home/biff/.wifi_credentials"
/* 
$ cat /home/biff/.wifi_credentials
#define CRED_SSID     "your_ssid"
#define CRED_PASSWORD "your_password"
*/

// Target IP and port
const char* udpAddress = "172.16.16.15";  
const int udpTxPort = 6060;

// String to send in the UDP request packet, in case other things
// are being controlled
#define QUERY_STRING "xbox_360"


WiFiUDP udp;

char rx_buffer[256];
bool output_state = false;
unsigned long last_poll;


#define LED_PIN 8
#define INPUT_PIN 1
#define OUTPUT_PIN 2


void sendPacket()
{
  udp.beginPacket(udpAddress, udpTxPort);
  udp.print(QUERY_STRING);
  udp.endPacket();
  Serial.println("UDP sent");
}


void updateStatus()
{
  // Receiving 'true' string in the UDP response indicates enable
  // Receivnig 'false' string in the UDP response indicates disable

  int packetSize = udp.parsePacket();
  if (!packetSize)
  {
    return;
  }
  Serial.println("Received packet");
  if (packetSize < sizeof(rx_buffer)) {
    int len = udp.read(rx_buffer, sizeof(rx_buffer) - 1);
    rx_buffer[len] = 0;

    if (strcmp(rx_buffer, "true") == 0)
    {
      output_state = true;
      Serial.println(rx_buffer);
    }
    else if (strcmp(rx_buffer, "false") == 0)
    {
      output_state = false;
      Serial.println(rx_buffer);
    }
    else
    {
      Serial.print("Invalid response: '");
      Serial.print(rx_buffer);
      Serial.println("'");
    }

    // Clear any remaining data, in case of jumbo packet?
    while (udp.available()) {
      udp.read(); // Read one byte at a time and discard
    }
  }
}


void setup()
{
  pinMode(INPUT_PIN, INPUT);
  pinMode(OUTPUT_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);

  Serial.begin(115200,SERIAL_8N1);
  Serial.println("");
  Serial.println("setup()");
  WiFi.mode(WIFI_STA);

  WiFi.begin(CRED_SSID, CRED_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // assigns random src port.  Confusingly a local port is always assigned
  // whether this line is here or not, however you won't receive anything
  // on that response port unless you call begin explicitly.
  udp.begin(0);

  Serial.println(WiFi.localIP());

  last_poll = millis();
}


void loop()
{
  updateStatus();
  int input_pin = digitalRead(INPUT_PIN);

  if (output_state)
  {
    digitalWrite(OUTPUT_PIN, input_pin);
  }
  else
  {
    digitalWrite(OUTPUT_PIN, LOW);
  }

  digitalWrite(LED_PIN, input_pin);

  unsigned long time_now = millis();
  // Polling every second is overkill, back this off once it's all tested working.
  if ((time_now - last_poll) > 1000)
  {
    last_poll = time_now;
    sendPacket();
  }
}

