#ifndef TEMPESTFEED_H
#define TEMPESTFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

#ifndef TEMPEST_PROXY_URL
    #define TEMPEST_PROXY_URL "https://tempestproxy.infoorbs.com/proxy"
#endif
class TempestFeed : public WeatherFeed {
public:
    TempestFeed(const String &apiKey, const String &stationId, int units, const String &stationName);
    bool getWeatherData(WeatherDataModel &model) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);
    String translateIcon(const std::string &icon);

private:
    String apiKey;
    String stationId;
    String stationName;
    std::string m_proxyUrl = TEMPEST_PROXY_URL;
    int units;

#ifdef WEATHER_UNITS_METRIC
    int m_weatherUnits = 0;
#else
    int m_weatherUnits = 1;
#endif

    std::string m_weatherLocation;
};

#endif
