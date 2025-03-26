#include <Arduino.h>
#include <ESP8266WiFi.h>   // outgoing https connections
#include "base64.h"        // http basic auth

#include "configPortal.h"

/*
[2025-03-26 20:06:38] http://127.0.0.1:8086/write?db=climate_dacha {
[2025-03-26 20:06:38]   timeout: 5000,
[2025-03-26 20:06:38]   headers: { 'content-type': 'application/x-www-form-urlencoded' },
[2025-03-26 20:06:38]   body: 'temperature,sensor=t1 value=34.625\n' +
[2025-03-26 20:06:38]     'temperature,sensor=t2 value=22\n' +
[2025-03-26 20:06:38]     'temperature,sensor=t3 value=31.625\n'
[2025-03-26 20:06:38] }
*/

//------------------------------------------------------------
bool influx_tsdbPut (String metric, String value) {
  auto tsdb = getCurrentTSDBconfig();

  if (tsdb.host == "" || tsdb.port == "") {
    Serial.println("no InfluxDB host or port configured");
    return false;
  }

  String auth = base64::encode(tsdb.login + ":" + tsdb.password);
  WiFiClient client;
//   client.setInsecure();

  Serial.println("connecting to " + tsdb.host + ":" + tsdb.port);
  client.connect(tsdb.host, tsdb.port.toInt());

  if (!client.connected()) {
    Serial.println("could not connect to InfluxDB Server:");
    Serial.println(tsdb.host + ":" + tsdb.port);
    return false;
  }

  String metrics = metric + "," +
    "location=" + tsdb.location + " " +
    "value=" + String(value) + "\n";

  client.println("POST /write?db=meteostation HTTP/1.1");
  client.println("Authorization: Basic " + auth);
  client.println("Host: " + tsdb.host);
  client.println("User-Agent: METEOSTATION/1.1");
  client.println("Connection: close");
  client.println("Content-Type: application/x-www-form-urlencoded");
  client.print("Content-Length: ");
  client.println(metrics.length());
  client.println();
  client.println(metrics);

  Serial.println(">>>>>>>>>>>>>>>> INFLUXDB");
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


