#include <IotCommander.h>
#include "Properties.h"
#include "Vars.h"
#include "Device.h"
#include "Led.h"

static char output[IOTC_JSON_BUFFER_SIZE];
static char input[IOTC_JSON_BUFFER_SIZE];
void callback(char *topic, byte *payload, unsigned int length)
{
  if (length >= IOTC_JSON_BUFFER_SIZE)
  {
    Serial.println("Payload too large!");
    return;
  }
  memcpy(input, payload, length);
  input[length] = '\0';

  Serial.println(topic);
  Serial.println(input);
  if (strcmp(mqtt_discovery_request_topic, topic) == 0)
  {
    Serial.println("Discovery called");
    device.discovery(output, IOTC_JSON_BUFFER_SIZE);
    client.publish(mqtt_discovery_response_topic, output);
  }
  else if (strcmp(mqtt_request_topic, topic) == 0)
  {
    Serial.println("Request called");
    device.request(input, output, IOTC_JSON_BUFFER_SIZE);
    Serial.println("Request result: ");
    Serial.println(output);
    client.publish(mqtt_response_topic, output);
  }
}

void reconnectMQTT()
{
  while (!client.connected())
  {
    Serial.print("Connecting to MQTT...");
    if (client.connect(device_id, mqtt_user, mqtt_password))
    {
      Serial.println(" connected!");
      client.subscribe(mqtt_discovery_request_topic);
      client.subscribe(mqtt_request_topic);
    }
    else
    {
      Serial.print(" failed, rc=");
      Serial.println(client.state());
      delay(3000);
    }
  }
}

void setup_ntp()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time");

  time_t now = 0;
  int retries = 0;

  while (now < 100000 && retries < 20)
  {
    delay(500);
    Serial.print(".");
    time(&now);
    retries++;
  }

  Serial.println();
  Serial.println("NTP time synchronized");
}

void setup_wifi()
{
  Serial.println();
  Serial.printf("Connecting to WiFi: %s\n", ssid);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);

  WiFi.begin(ssid, password);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 15000)
  {
    Serial.print(".");
    delay(500);
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("\nWiFi Failed");
    ESP.restart();
  }

  Serial.println("\nWiFi connected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void setup_led()
{
  preferences.begin(PREFERENCES_NAMESAPCE, false);
  auto ledConfig = loadConfig();
  strncpy(currentColor, ledConfig.color, sizeof(currentColor) - 1);
  currentColor[sizeof(currentColor) - 1] = '\0';
  currentBrightness = ledConfig.brightness;
  Serial.println(ledConfig.color);
  Serial.println(ledConfig.brightness);
  setLed(ledConfig.color, ledConfig.brightness);
}

void setup_servo()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
}

void setup()
{
  Serial.begin(115200);
  setup_led();
  setup_servo();
  setup_wifi();
  setup_ntp();
  espClient.setCACert(rootCA);
  httpsClient.setCACert(httpRootCA);
  client.setBufferSize(IOTC_JSON_BUFFER_SIZE);
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("WiFi disconnected! Reconnecting...");
    setup_wifi();
  }

  if (!client.connected())
    reconnectMQTT();
  client.loop();

  checkAutoFeed();
}