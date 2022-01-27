#include <ESP8266WiFi.h>   // outgoing https connections
#include <AutoConnect.h>   // wifi auto ap/client setup

#include "configPortal.h"
#include "ntpTime.h"

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
AutoConnectInput pwd("tsdbPassword", "", "user password", NULL, "required every save", AC_Tag_BR, AC_Input_Text);
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
// CONFIG STORING
//
const auto zfs = &AUTOCONNECT_APPLIED_FILESYSTEM;

bool isFsMounted() {
  bool mounted = AutoConnectFS::_isMounted(zfs);
  if (!mounted) {
    Serial.println("FS is not mounted, try to mount...");
    if (!zfs->begin(AUTOCONECT_FS_INITIALIZATION)) {
      Serial.println("mount failed");
      return false;
    }
  }
  return true;
};
//---------------------------------------------------------------------------
bool storeVariableToDisk (String& name) {
  if (name == "tsdbHost") { return true; }
  if (name == "tsdbPort") { return true; }
  if (name == "tsdbLogin") { return true; }
  if (name == "tsdbPassword") { return true; }
  if (name == "tsdbLocation") { return true; }
  return false;
}
//---------------------------------------------------------------------------
void readTSDBcfg (AutoConnectAux& aux) {
  if (!isFsMounted()) {
    return;
  }

  for (AutoConnectElement& element : aux.getElements()) {
    if (!storeVariableToDisk(element.name)) {
      continue;
    }
    auto file = zfs->open(element.name, "r");
    if (file == -1) {
      Serial.print("[fs-read] error opening file: "); Serial.println(element.name);
    } else {
      String data = "";
      Serial.print("read["); Serial.print(element.name); Serial.print("] (");
      char c[] = "x";
      while (file.available()) {
        c[0] = file.read();
        data += String(c);
        Serial.print(c);
      }
      file.close();
      Serial.println(")");
      element.value = data;
    }
  }
}
//---------------------------------------------------------------------------
void writeTSDBcfg (AutoConnectAux& aux) {
  if (!isFsMounted()) {
    return;
  }

  for (AutoConnectElement& element : aux.getElements()) {
    if (!storeVariableToDisk(element.name)) {
      continue;
    }
    auto file = zfs->open(element.name, "w");
    if (file == -1) {
      Serial.print("[fs-write] error opening file: "); Serial.println(element.name);
    } else {
      auto size = file.write(element.value.c_str(), element.value.length());
      Serial.print("write["); Serial.print(element.name); Serial.print("]"); Serial.println(size);
      file.close();
    }
  }
}
//--------------------------------------------------------------------------------
String savedPassword = "";

tsdbConfig getCurrentTSDBconfig() {
  if (savedPassword == ""){
    // read settings on boot
    readTSDBcfg(auxTSDB);
    savedPassword = auxTSDB["tsdbPassword"].value;
    auxTSDB["tsdbPassword"].value = "";
  }

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
  // save real passowrds on POST, and don't erase in on GET
  if (aux["tsdbPassword"].value != "") {
    savedPassword = aux["tsdbPassword"].value;
    writeTSDBcfg(aux);
  }
  // do not send real password to UI
  aux["tsdbPassword"].value = "";
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

  configPortal.config(acConfig);
  configPortal.join(auxTSDB);
  configPortal.on(TSDB_SETTING_URI, onTSDB_PageLoad);
  configPortal.onNotFound(handleNotFound);

  configPortal.begin();
}

//--------------------------------------------------------------------------------
// for external usage
void configPortalHandleClient() {
  configPortal.handleClient();
}
