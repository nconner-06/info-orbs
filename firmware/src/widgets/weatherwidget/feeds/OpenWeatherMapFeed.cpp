#include "OpenWeatherMapFeed.h"
#include "../WeatherTranslations.h"
#include "GlobalTime.h"
#include "TaskFactory.h"
#include "config_helper.h"
#include <unordered_map>

OpenWeatherMapFeed::OpenWeatherMapFeed(const String &apiKey, const float &m_lat_id, const float &m_long_id, const std::string &location)
    : apiKey(apiKey), m_lat_id(m_lat_id), m_long_id(m_long_id), m_weatherLocation(location) {}

void OpenWeatherMapFeed::setupConfig(ConfigManager &config) {
    // Define the configuration for OpenWeatherMap variables
    config.addConfigFloat("WeatherWidget", "openWeatherLat", &m_lat_id,t_openWeatherLat);
    config.addConfigFloat("WeatherWidget", "openWeatherLong", &m_long_id,t_openWeatherLong);

}

bool OpenWeatherMapFeed::getWeatherData(WeatherDataModel &model) {
    String weatherUnits = m_weatherUnits == 0 ? "metric" : "us";
    String lang = I18n::getLanguageString();
    if (lang != "en" && lang != "de" && lang != "fr") {
        // Language is not supported on visualcrossing -> use english
        lang = "en";
    }

    String httpRequestAddress = String(WEATHER_OPENWEATHERMAP_API_URL) + "?lat=" + String(m_lat_id) + "&lon=" + String(m_long_id) +
                                +"&appid=" + apiKey + "&units=" + weatherUnits + "&exclude=minutely,hourly,alerts&lang=" + lang;

    auto task = TaskFactory::createHttpGetTask(
        httpRequestAddress, [this, &model](int httpCode, const String &response) { processResponse(httpCode, response, model); }, [this](int httpCode, String &response) { preProcessResponse(httpCode, response); });

    if (!task) {
        Log.errorln("Failed to create weather task");
        return false;
    }

    bool success = TaskManager::getInstance()->addTask(std::move(task));
    if (!success) {
        Log.errorln("Failed to add weather task");
    }

    return success;
}
void OpenWeatherMapFeed::preProcessResponse(int httpCode, String &response) {
    if (httpCode > 0) {

        JsonDocument filter;
        filter["current"]["temp"] = true;
        filter["daily"][0]["summary"] = true;
        filter["daily"][0]["weather"][0]["icon"] = true;
        filter["daily"][0]["temp"]["max"] = true;
        filter["daily"][0]["temp"]["min"] = true;

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response, DeserializationOption::Filter(filter));

        if (!error) {
            response = doc.as<String>();
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    }
}

void OpenWeatherMapFeed::processResponse(int httpCode, const String &response, WeatherDataModel &model) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            model.setCityName(String(m_weatherLocation.c_str()));
            model.setCurrentTemperature(doc["current"]["temp"].as<float>());
            model.setCurrentText(doc["daily"][0]["summary"].as<String>());

            model.setCurrentIcon(translateIcon(doc["daily"][0]["weather"][0]["icon"]));
            model.setTodayHigh(doc["daily"][0]["temp"]["max"].as<float>());
            model.setTodayLow(doc["daily"][0]["temp"]["min"].as<float>());
            for (int i = 0; i < 3; i++) {
                model.setDayIcon(i, translateIcon(doc["daily"][i + 1]["weather"][0]["icon"]));
                model.setDayHigh(i, doc["daily"][i + 1]["temp"]["max"].as<float>());
                model.setDayLow(i, doc["daily"][i + 1]["temp"]["min"].as<float>());
            }
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    } else {
        Log.errorln("HTTP request failed, error code: %d\n", httpCode);
    }
}
String OpenWeatherMapFeed::translateIcon(const std::string &icon) {
    // Define the mapping of input strings to simplified weather icons
    static const std::unordered_map<std::string, std::string> iconMapping = {
        {"01d", "clear-day"},
        {"01n", "clear-night"},
        {"02d", "partly-cloudy-day"},
        {"02n", "partly-cloudy-night"},
        {"03d", "partly-cloudy-day"},
        {"03n", "partly-cloudy-night"},
        {"04d", "partly-cloudy-day"},
        {"04n", "partly-cloudy-night"},
        {"09d", "rain"},
        {"09n", "rain"},
        {"10d", "rain"},
        {"10n", "rain"},
        {"11d", "rain"},
        {"11n", "rain"},
        {"13d", "snow"},
        {"13n", "snow"},
        {"50d", "fog"},
        {"50n", "fog"}};

    // Find the icon in the mapping and return the corresponding simplified value
    auto it = iconMapping.find(icon);
    if (it != iconMapping.end()) {
        return String(it->second.c_str());
    }
    return "clear-day";
}
