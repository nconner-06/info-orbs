#ifndef OPENWATHERMAPTFEED_H
#define OPENWATHERMAPTFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

class OpenWeatherMapFeed : public WeatherFeed {
public:
    OpenWeatherMapFeed(const String &apiKey, const float &lat_id, const float &lon_id, const std::string &location);
    bool getWeatherData(WeatherDataModel &model) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);
    String translateIcon(const std::string &icon);

private:
    String apiKey;
    float lat_id;
    float lon_id;

#ifdef WEATHER_UNITS_METRIC
    int m_weatherUnits = 0;
#else
    int m_weatherUnits = 1;
#endif

    std::string m_weatherLocation;
};

#endif
