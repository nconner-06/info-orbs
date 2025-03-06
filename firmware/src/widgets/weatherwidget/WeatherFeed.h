#ifndef WEATHERFEED_H
#define WEATHERFEED_H

#include "WeatherDataModel.h"

class WeatherFeed {
public:
    virtual bool getWeatherData(WeatherDataModel &model) = 0;
};

#endif
