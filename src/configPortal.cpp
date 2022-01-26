#include <ESP8266WiFi.h>   // outgoing https connections
#include <AutoConnect.h>   // wifi auto ap/client setup

#include "configPortal.h"
#include "msTime.h"

#define DEVICE_NAME "METEOSTATION";
#define TSDB_SETTING_URI "/tsdb"

ESP8266WebServer server;
AutoConnect configPortal(server);
AutoConnectConfig acConfig;

//--------------------------------------------------------------------------------
// additional menu for TSDB settings
ACText(header, "Export metrics to OpenTSDB instance:");
AutoConnectElement hSplit("element", "<hr>");

AutoConnectInput host("tsdbHost", "", "hostname", NULL, NULL, AC_Tag_BR, AC_Input_Text);
AutoConnectInput port("tsdbPort", "", "port", NULL, NULL, AC_Tag_BR, AC_Input_Number);
AutoConnectInput login("tsdbLogin", "", "user login", NULL, NULL, AC_Tag_BR, AC_Input_Text);
AutoConnectInput pwd("tsdbPassword", "", "user password", NULL, NULL, AC_Tag_BR, AC_Input_Password);
AutoConnectInput location("tsdbLocation", "", "meteostation location", NULL, NULL, AC_Tag_BR, AC_Input_Text);
AutoConnectSubmit submit("tsdbSubmit", "save", TSDB_SETTING_URI, AC_Tag_BR);

AutoConnectAux auxTSDB(TSDB_SETTING_URI, "OpenTSDB Settings", true, { header, hSplit, host, port, login, pwd, location, submit });

//--------------------------------------------------------------------------------
void handleNotFound() {
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n\n";
  message += "source code: https://github.com/kzub/meteo_station\n\n";
  message += getFormatetTime();
  server.send(200, "text/plain", message);
}

//--------------------------------------------------------------------------------
String savedPassword = "";
tsdbConfig getCurrentTSDBconfig() {
  return {
    auxTSDB["tsdbHost"].value,
    auxTSDB["tsdbPort"].value,
    auxTSDB["tsdbLogin"].value,
    savedPassword,
    auxTSDB["tsdbLocation"].value,
  };
}

//--------------------------------------------------------------------------------
String onTSDB_PageLoad (AutoConnectAux& aux, PageArgument& args) {
  if (aux["tsdbPassword"].value != "******") {
    // do not overwrite real password
    // we have real pasword saving right now
    savedPassword = aux["tsdbPassword"].value;
  }
  // mask real password on page
  if (aux["tsdbPassword"].value != "") {
    aux["tsdbPassword"].value = "******";
  }
  return  {};
}

//--------------------------------------------------------------------------------
// for external usage
void configPortalSetup () {
  acConfig.title = DEVICE_NAME;
  acConfig.apid = DEVICE_NAME + "-" + String(ESP.getChipId(), HEX);
  acConfig.psk = "";
  acConfig.hostName = acConfig.apid;
  acConfig.autoReconnect = true;
  acConfig.reconnectInterval = 10;

  configPortal.join(auxTSDB);
  configPortal.config(acConfig);

  configPortal.on(TSDB_SETTING_URI, onTSDB_PageLoad);
  configPortal.onNotFound(handleNotFound);

  configPortal.begin();
}

//--------------------------------------------------------------------------------
// for external usage
void configPortalHandleClient() {
  if (WiFi.status() == WL_CONNECTED) {
    // Here to do when WiFi is connected.
  }
  else {
    // Here to do when WiFi is not connected.
  }

  configPortal.handleClient();
}
