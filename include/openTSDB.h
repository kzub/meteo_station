bool influx_tsdbPut (String metric, String value);
bool genuine_tsdbPut (String metric, String value);

bool tsdbPut (String metric, String value) {
    #ifdef USE_INFLUXDB
        return influx_tsdbPut(metric, value);
    #else
        return genuine_tsdbPut(metric, value);
    #endif
}
