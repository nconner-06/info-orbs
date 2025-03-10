#ifndef OPENWATHERMAPTFEED_H
#define OPENWATHERMAPTFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

#ifndef WEATHER_OPENWEATHERMAP_API_URL
    #define WEATHER_OPENWEATHERMAP_API_URL "https://api.openweathermap.org/data/3.0/onecall"
#endif

class OpenWeatherMapFeed : public WeatherFeed {
public:
    OpenWeatherMapFeed(const String &apiKey, int units);
    bool getWeatherData(WeatherDataModel &model) override;
    void setupConfig(ConfigManager &config) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);
    String translateIcon(const std::string &icon);

private:
    String apiKey;
    float m_lat_id = WEATHER_OPENWEATHERMAP_LAT;
    float m_long_id = WEATHER_OPENWEATHERMAP_LON;
    std::string m_name = WEATHER_OPENWEATHERMAP_NAME;

#ifdef WEATHER_UNITS_METRIC
    int m_weatherUnits = 0;
#else
    int m_weatherUnits = 1;
#endif
};

#endif
