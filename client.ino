#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "HOME-4E86"
#define STAPSK "18maloji"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

const char *host = "192.168.1.56";
const uint16_t port = 1337;

int switch0 = D1;
int led = D2;
int sensor = A0;

void setup()
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(switch0, INPUT);
  pinMode(sensor, INPUT);

  // We start by connecting to a WiFi network

  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  static bool exit_com = false;

  Serial.print("connecting to ");
  Serial.print(host);
  Serial.print(':');
  Serial.println(port);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  if (!client.connect(host, port))
  {
    Serial.println("connection failed");
    delay(5000);
    return;
  }

  while (!exit_com)
  {

    // wait for data to be available
    unsigned long timeout = millis();
    while (client.available() == 0)
    {
      if (millis() - timeout > 50000)
      {
        Serial.println(">>> Client Timeout !");
        client.stop();
        delay(30000);
        return;
      }
    }

    // Read all the lines of the reply from server and print them to Serial
    // not testing 'client.connected()' since we do not need to send data here
    while (client.available())
    {
      int command = client.parseInt();
      execCommand(command);
    }
  }

  // Close the connection
  Serial.println();
  Serial.println("closing connection");
  client.stop();
}

void sendData(String data)
{
  // This will send a string to the server
  if (client.connected())
  {
    client.println(data);
  }
}

int readSwitchState()
{
  return digitalRead(switch0);
}

int readSensorState()
{
  return analogRead(sensor);
}

void controlLed(int command)
{
  digitalWrite(led, command);
}

void execCommand(int command)
{

  String out;
  if (command == 0)
  {
    sendData("Exiting");
    exit_com = true;
  }
  if (command == 1 || (command == 2))
  {
    command -= 1;
    out = "Led turns" + command;
    sendData(out);
    controlLed(command);
  }
  if (command == 3)
  {
    out = "Switch state " + readSwitchState();
    sendData(out);
  }
  if (command == 4)
  {
    out = "Sensor signal " + readSensorState();
    sendData(out);
  }
  if (command == -1)
  {
    out = "Invalid command." sendData(out)
  }
}
