#include <WiFi.h>
#include <PubSubClient.h>

#include "config.h"

const int topic_len = strlen(mqtt_topic_template);
char zone_topics[num_zones][topic_len];
bool zone_states[num_zones] = {false};
int num_windows_triggered[num_zones] = {0};

int value = 0;
float voltage;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);

  //Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());

  //Initialize MQTT
  client.setServer(mqtt_server, mqtt_port);

  //Initialize zone topics
  for (int i = 0; i < num_zones; i++) {
    snprintf(zone_topics[i], topic_len, mqtt_topic_template, i);
    Serial.println(zone_topics[i]);

    client.publish(zone_topics[i], "OFF");
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  for (int i = 0; i < num_zones; i++) {
    //Get the voltage
    value = analogRead(analog_pins[i]);
    voltage = value / 238.0;
    /*Serial.print("Voltage from analog pin ");
    Serial.print(i);
    Serial.print(": ");
    Serial.println(voltage);*/

    if (!client.connected()) {
      reconnect();
    }
    client.loop();

    if (zone_states[i]) { //If zone is active
      if (voltage < threshold_volts) {
        num_windows_triggered[i]++;
        if (num_windows_triggered[i] >= alarm_windows) {
          zone_states[i] = false;
          num_windows_triggered[i] = 0;
        }
          Serial.print("Zone ");
          Serial.print(i);
          Serial.println(" turned OFF");
        client.publish(zone_topics[i], "OFF");
      } else if (num_windows_triggered[i] > 0) {
        num_windows_triggered[i] = 0;
      }
    } else { //If zone is inactive
      if (voltage > threshold_volts) {
        num_windows_triggered[i]++;
        if (num_windows_triggered[i] >= alarm_windows) {
          zone_states[i] = true;
          num_windows_triggered[i] = 0;

          Serial.print("Zone ");
          Serial.print(i);
          Serial.println(" turned ON");
          client.publish(zone_topics[i], "ON");
          
        }
      } else if (num_windows_triggered[i] > 0) {
        num_windows_triggered[i] = 0;
      }
    }
    /*Serial.println("Publishing state..");
    client.publish(zone_topics[i], zone_states[i]?"ON":"OFF");
    char voltstr[20];
    gcvt(voltage, 6, voltstr);
    if (voltage > 0) client.publish(zone_topics[i], voltstr);*/
  }
  
  delay(500);
}
