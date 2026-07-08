#include "GlobalTime.h"

#include "ConfigManager.h"
#include "Translations.h"
#include "config_helper.h"
#include "esp_sntp.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>

GlobalTime *GlobalTime::m_instance = nullptr;

GlobalTime::GlobalTime() {
    ConfigManager *cm = ConfigManager::getInstance();
    m_timezone = cm->getConfigInt("timezone", m_timezone);
    int clockFormat = cm->getConfigInt("clockFormat", CLOCK_FORMAT); // config added in ClockWidget
    m_ntpServer = cm->getConfigString("ntpServer", m_ntpServer); // config added in MainHelper
    Log.infoln("GlobalTime initialized, tzLoc=%s, clockFormat=%d, ntpServer=%s", tz_list[m_timezone].c_str(), clockFormat, m_ntpServer.c_str());
    m_format24hour = (clockFormat == CLOCK_FORMAT_24_HOUR);

    //    sntp_set_sync_interval(12 * 60 * 60 * 1000UL); // 12 hours
    struct tm timeinfo;
    configTzTime("etc/utc", m_ntpServer.c_str());
    if (!getLocalTime(&timeinfo))
        Log.errorln("Error getting time from NTP server (UTC)");
}

GlobalTime::~GlobalTime() {}

GlobalTime *GlobalTime::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new GlobalTime();
    }
    return m_instance;
}

time_t GlobalTime::getUnixEpochIfAvailable() {
    return m_instance ? m_instance->getUnixEpoch() : 0;
}

void GlobalTime::updateTime(bool force) {
    struct tm tm_UTC;
    struct tm tm_local;
    if (force || millis() - m_updateRTC > m_oneHour) {
        m_updateRTC = millis();
        setenv("TZ", "UTC0", 1);
        tzset();
        if (!getLocalTime(&tm_UTC))
            Log.errorln("Error getting time for UTC");
        rtc.setTimeStruct(tm_UTC);
        const int l_utc = (tm_UTC.tm_hour * 3600) + (tm_UTC.tm_min * 60);

        // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
        m_timezonePosix = tz_info[m_timezone];
        setenv("TZ", m_timezonePosix.c_str(), 1);
        tzset();
        if (!getLocalTime(&tm_local))
            Log.errorln("Error getting for time Local");
        const int l_loc = (tm_local.tm_hour * 3600) + (tm_local.tm_min * 60);

#ifdef GT_DEBUG
        Log.infoln("GT : %i, %i", l_utc, l_loc);
#endif
        m_timeZoneOffset = l_loc - l_utc;
    }
}

int GlobalTime::getDayOfWeek() {
    return rtc.getDayofWeek();
}

void GlobalTime::getHourAndMinute(int &hour, int &minute) {
    hour = (m_format24hour ? rtc.getHour(true) : rtc.getHour());
    minute = rtc.getMinute();
}

int GlobalTime::getHour() {
    return (m_format24hour ? rtc.getHour(true) : rtc.getHour());
}

int GlobalTime::getHour24() {
    return rtc.getHour(true);
}

String GlobalTime::getHourPadded() {
    if (rtc.getHour(true) < 10) {
        return "0" + String(rtc.getHour(true));
    } else {
        return String(rtc.getHour(true));
    }
}

int GlobalTime::getMinute() {
    return rtc.getMinute();
}

String GlobalTime::getMinutePadded() {
    if (rtc.getMinute() < 10) {
        return "0" + String(rtc.getMinute());
    } else {
        return String(rtc.getMinute());
    }
}

String GlobalTime::getSecondPadded() {
    if (rtc.getSecond() < 10) {
        return "0" + String(rtc.getSecond());
    } else {
        return String(rtc.getSecond());
    }
}

int GlobalTime::getSecond() {
    return rtc.getSecond();
}

time_t GlobalTime::getUnixEpoch() {
    return rtc.getEpoch();
}

int GlobalTime::getDay() {
    return rtc.getDay();
}

int GlobalTime::getMonth() {
    return rtc.getMonth();
}

String GlobalTime::getMonthName() {
    return i18n(t_months, rtc.getMonth());
}

int GlobalTime::getYear() {
    return rtc.getYear();
}

String GlobalTime::getTime() {
    return String(m_format24hour ? rtc.getHour(true) : rtc.getHour()) + ":" + (rtc.getMinute() < 10 ? "0" + String(rtc.getMinute()) : String(rtc.getMinute()));
}

String GlobalTime::getWeekday() {
    return i18n(t_weekdays, rtc.getDayofWeek());
}

String GlobalTime::getDayAndMonth() {
#ifdef WEATHER_UNITS_METRIC
    String retVal = i18n(t_dayMonthFormat);
    retVal.replace("%d", String(rtc.getDay()));
    retVal.replace("%B", i18n(t_months, rtc.getMonth()));
    return retVal;
#else
    return i18n(t_months, rtc.getMonth());
    +" " + String(rtc.getDay());
#endif
}

bool GlobalTime::isPM() {
    return rtc.getHour(true) >= 12;
}

bool GlobalTime::getFormat24Hour() {
    return m_format24hour;
}

bool GlobalTime::setFormat24Hour(bool format24hour) {
    m_format24hour = format24hour;
    return m_format24hour;
}

int GlobalTime::getTimeZoneOffset() {
    return m_timeZoneOffset;
}
