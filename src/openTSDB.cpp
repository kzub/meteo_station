#include <Arduino.h>
#include <ESP8266WiFi.h>   // outgoing https connections
#include "base64.h"        // http basic auth

#include "configPortal.h"
#include "ntpTime.h"

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
  "\"tags\":{\"location\":\"" + tsdb.location + "\"}}";

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

  Serial.println(">>>>>>>>>>>>>>>>");
  Serial.println(metrics);

  uint32_t timeout = millis() + 3000;
  char response[] = "HTTP/1.1 000 NO_RESPONSE";

  while (millis() < timeout) {
    int rlen = client.read((uint8_t*)response, sizeof(response) - 1 /* strings end with null */);
    yield(); // not sure if it needed
    if (rlen > 0) {
      break;
    }
  }
  client.stop();

  bool ok = strncmp(response, "HTTP/1.1 204", 12) == 0;
  Serial.println(ok ? "OK" : "FAILED");
  Serial.println(response);

  return ok;
}