#ifndef VISUALCROSSINGFEED_H
#define VISUALCROSSINGFEED_H

#include "../WeatherDataModel.h"
#include "../WeatherFeed.h"
#include <ArduinoJson.h>
#include <TaskManager.h>

class VisualCrossingFeed : public WeatherFeed {
public:
    VisualCrossingFeed(const String &apiKey);
    bool getWeatherData(WeatherDataModel &model) override;
    void setupConfig(ConfigManager &config) override;
    void processResponse(int httpCode, const String &response, WeatherDataModel &model);
    void preProcessResponse(int httpCode, String &response);

private:
    String apiKey;
    // std::string m_weatherLocation;

#ifdef WEATHER_UNITS_METRIC
    int m_weatherUnits = 0;
#else
    int m_weatherUnits = 1;
#endif

// This is a hack to support old config.h files that have WEATHER_LOCAION instead of LOCATION.
#ifndef WEATHER_LOCATION
    #define WEATHER_LOCATION WEATHER_LOCAION
#endif

    std::string m_weatherLocation = WEATHER_LOCATION;
};

#endif
