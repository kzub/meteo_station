#include <Arduino.h>
#include <ESP8266WiFi.h>   // outgoing https connections
#include "base64.h"        // http basic auth

#include "configPortal.h"
#include "msTime.h"

//------------------------------------------------------------
bool tsdbPut (String metric, String value) {
  auto tsdb = getCurrentTSDBconfig();

  if (tsdb.host == "" || tsdb.port == "") {
    Serial.println("no openTSDB host or port configured");
    return false;
  }

  String auth = base64::encode(tsdb.login + ":" + tsdb.password);
  BearSSL::WiFiClientSecure client;
  client.setInsecure();

  client.connect(tsdb.host, tsdb.port.toInt());

  if (!client.connected()) {
    Serial.println("could not connect to openTSDB Server:");
    Serial.println(tsdb.host + ":" + tsdb.port);
    return false;
  }

  String timestamp = String(getEpochTimeSec()) + String("000");
  String metrics = "{\"metric\":\"meteostation." + metric + "\"," +
  "\"timestamp\":" + timestamp + "," +
  "\"value\":" + String(value) + "," +
  "\"tags\":{\"location\":\"" + tsdb.location + "\"}}\n";

  client.println("POST /api/put HTTP/1.1");
  client.println("Authorization: Basic " + auth);
  client.println("Host: " + tsdb.host);
  client.println("User-Agent: METEOSTATION/1.1");
  client.println("Connection: close");
  client.println("Content-Type: application/json");
  client.print("Content-Length: ");
  client.println(metrics.length());
  client.println();
  client.println(metrics);

  Serial.println(metrics);
  Serial.println(">>>>>>>>>>>>>>>>");
  while (client.available()) {
    char c = client.read();
    Serial.print(c);
  }
  Serial.println("----------------");

  return true;
}