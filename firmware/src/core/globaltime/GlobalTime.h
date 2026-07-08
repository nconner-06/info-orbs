#ifndef GLOBALTIME_H
#define GLOBALTIME_H

#include "config_helper.h"
#include <Arduino.h>
// #include <TimeLib.h>
#include "time.h"
#include <ESP32Time.h>

enum ClockFormat {
    CLOCK_FORMAT_24_HOUR = 0,
    CLOCK_FORMAT_12_HOUR = 1,
    CLOCK_FORMAT_12_HOUR_AMPM = 2
};

#if FORMAT_24_HOUR == true
    #define CLOCK_FORMAT CLOCK_FORMAT_24_HOUR
#else
    #if SHOW_AM_PM_INDICATOR == false
        #define CLOCK_FORMAT CLOCK_FORMAT_12_HOUR
    #else
        #define CLOCK_FORMAT CLOCK_FORMAT_12_HOUR_AMPM
    #endif
#endif

static String tz_list[] PROGMEM = {
    "Africa/Addis_Ababa",
    "Africa/Algiers",
    "Africa/Cairo",
    "Africa/Casablanca",
    "Africa/Dakar",
    "Africa/Dar_es_Salaam",
    "Africa/Johannesburg",
    "Africa/Nairobi",
    "America/Anchorage",
    "America/Argentina/Buenos_Aires",
    "America/Aruba",
    "America/Barbados",
    "America/Belize",
    "America/Bogota",
    "America/Chicago",
    "America/Costa_Rica",
    "America/Denver",
    "America/Detroit",
    "America/Guatemala",
    "America/Halifax",
    "America/Indiana/Indianapolis",
    "America/Los_Angeles",
    "America/Martinique",
    "America/Mexico_City",
    "America/New_York",
    "America/Phoenix",
    "America/Puerto_Rico",
    "America/Santiago",
    "America/Sao_Paulo",
    "America/St_Johns",
    "America/St_Kitts",
    "America/Toronto",
    "America/Vancouver",
    "America/Winnipeg",
    "Asia/Almaty",
    "Asia/Amman",
    "Asia/Baghdad",
    "Asia/Bangkok",
    "Asia/Beirut",
    "Asia/Dhaka",
    "Asia/Dubai",
    "Asia/Ho_Chi_Minh",
    "Asia/Hong_Kong",
    "Asia/Jakarta",
    "Asia/Jerusalem",
    "Asia/Karachi",
    "Asia/Kathmandu",
    "Asia/Kolkata",
    "Asia/Kuala_Lumpur",
    "Asia/Kuwait",
    "Asia/Manila",
    "Asia/Seoul",
    "Asia/Shanghai",
    "Asia/Singapore",
    "Asia/Taipei",
    "Asia/Tashkent",
    "Asia/Tehran",
    "Asia/Tokyo",
    "Asia/Ulaanbaatar",
    "Asia/Yekaterinburg",
    "Atlantic/Azores",
    "Atlantic/Reykjavik",
    "Australia/Adelaide",
    "Australia/Brisbane",
    "Australia/Darwin",
    "Australia/Hobart",
    "Australia/Melbourne",
    "Australia/Perth",
    "Australia/Sydney",
    "Europe/Amsterdam",
    "Europe/Athens",
    "Europe/Belgrade",
    "Europe/Berlin",
    "Europe/Brussels",
    "Europe/Bucharest",
    "Europe/Copenhagen",
    "Europe/Dublin",
    "Europe/Helsinki",
    "Europe/Istanbul",
    "Europe/Kiev",
    "Europe/Lisbon",
    "Europe/London",
    "Europe/Madrid",
    "Europe/Moscow",
    "Europe/Oslo",
    "Europe/Paris",
    "Europe/Prague",
    "Europe/Rome",
    "Europe/Stockholm",
    "Europe/Warsaw",
    "Pacific/Auckland",
    "Pacific/Chatham",
    "Pacific/Fiji",
    "Pacific/Galapagos",
    "Pacific/Guam",
    "Pacific/Honolulu",
    "Pacific/Kiritimati",
    "Pacific/Niue",
    "Pacific/Port_Moresby",
    "Pacific/Tahiti",
    "Etc/UTC"};

static String tz_info[] PROGMEM = {
    "EAT-3", //       "Africa/Addis_Ababa"
    "CET-1", //       "Africa/Algiers"
    "EET-2EEST,M4.5.5/0,M10.5.5/0", //       "Africa/Cairo"
    "WET0WEST,M3.5.0/0,M10.5.0/0", //       "Africa/Casablanca"
    "GMT0", //       "Africa/Dakar"
    "EAT-3", //       "Africa/Dar_es_Salaam"
    "SAST-2", //       "Africa/Johannesburg"
    "EAT-3", //       "Africa/Nairobi"
    "AKST9AKDT,M3.2.0,M11.1.0", //       "America/Anchorage"
    "ART3", //       "America/Argentina/Buenos_Aires"
    "AST4", //       "America/Aruba"
    "AST4", //       "America/Barbados"
    "CST6", //       "America/Belize"
    "<-05>5", //       "America/Bogota"
    "CST6CDT,M3.2.0,M11.1.0", //       "America/Chicago"
    "CST6", //       "America/Costa_Rica"
    "MST7MDT,M3.2.0,M11.1.0", //       "America/Denver"
    "EST5EDT,M3.2.0,M11.1.0", //       "America/Detroit"
    "CST6", //       "America/Guatemala"
    "AST4ADT,M3.2.0,M11.1.0", //       "America/Halifax"
    "EST5EDT,M3.2.0,M11.1.0", //       "America/Indiana/Indianapolis"
    "PST8PDT,M3.2.0,M11.1.0", //       "America/Los_Angeles"
    "AST4", //       "America/Martinique"
    "CST6CDT,M4.1.0,M10.5.0", //       "America/Mexico_City"
    "EST5EDT,M3.2.0,M11.1.0", //       "America/New_York"
    "MST7", //       "America/Phoenix"
    "AST4", //       "America/Puerto_Rico"
    "CLT4CLST,M9.1.6/24,M4.1.6/24", //       "America/Santiago"
    "BRT3", //       "America/Sao_Paulo"
    "NST3:30NDT,M3.2.0,M11.1.0", //       "America/St_Johns"
    "AST4", //       "America/St_Kitts"
    "EST5EDT,M3.2.0,M11.1.0", //       "America/Toronto"
    "PST8PDT,M3.2.0,M11.1.0", //       "America/Vancouver"
    "CST6CDT,M3.2.0,M11.1.0", //       "America/Winnipeg"
    "ALMT-6", //       "Asia/Almaty"
    "EET-2EEST,M3.5.4/24,M10.5.5/1", //       "Asia/Amman"
    "AST-3", //       "Asia/Baghdad"
    "ICT-7", //       "Asia/Bangkok"
    "EET-2EEST,M3.5.0/0,M10.5.0/0", //       "Asia/Beirut"
    "BDT-6", //       "Asia/Dhaka"
    "GST-4", //       "Asia/Dubai"
    "ICT-7", //       "Asia/Ho_Chi_Minh"
    "HKT-8", //       "Asia/Hong_Kong"
    "WIB-7", //       "Asia/Jakarta"
    "IST-2IDT,M3.4.4/26,M10.5.0", //       "Asia/Jerusalem"
    "PKT-5", //       "Asia/Karachi"
    "NPT-5:45", //       "Asia/Kathmandu"
    "IST-5:30", //       "Asia/Kolkata"
    "MYT-8", //       "Asia/Kuala_Lumpur"
    "<+03>-3", //       "Asia/Kuwait"
    "PST-8", //       "Asia/Manila"
    "KST-9", //       "Asia/Seoul"
    "CST-8", //       "Asia/Shanghai"
    "SGT-8", //       "Asia/Singapore"
    "CST-8", //       "Asia/Taipei"
    "UZT-5", //       "Asia/Tashkent"
    "<+0330>-3:30", //       "Asia/Tehran"
    "JST-9", //       "Asia/Tokyo"
    "ULAT-8", //       "Asia/Ulaanbaatar"
    "YEKT-5", //       "Asia/Yekaterinburg"
    "AZOT1AZOST,M3.5.0/0,M10.5.0/0", //       "Atlantic/Azores"
    "GMT0", //       "Atlantic/Reykjavik"
    "ACST-9:30ACDT,M10.1.0,M4.1.0", //       "Australia/Adelaide"
    "AEST-10", //       "Australia/Brisbane"
    "ACST-9:30", //       "Australia/Darwin"
    "AEST-10AEDT,M10.1.0,M4.1.0", //       "Australia/Hobart"
    "AEST-10AEDT,M10.1.0,M4.1.0", //       "Australia/Melbourne"
    "AWST-8", //       "Australia/Perth"
    "AEST-10AEDT,M10.1.0,M4.1.0", //       "Australia/Sydney"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Amsterdam"
    "EET-2EEST,M3.5.0/3,M10.5.0/4", //       "Europe/Athens"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Belgrade"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Berlin"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Brussels"
    "EET-2EEST,M3.5.0/3,M10.5.0/4", //       "Europe/Bucharest"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Copenhagen"
    "GMT0IST,M3.5.0/1,M10.5.0", //       "Europe/Dublin"
    "EET-2EEST,M3.5.0/3,M10.5.0/4", //       "Europe/Helsinki"
    "TRT-3", //       "Europe/Istanbul"
    "EET-2EEST,M3.5.0/3,M10.5.0/4", //       "Europe/Kiev"
    "WET0WEST,M3.5.0/0,M10.5.0/0", //       "Europe/Lisbon"
    "GMT0BST,M3.5.0/1,M10.5.0", //       "Europe/London"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Madrid"
    "MSK-3", //       "Europe/Moscow"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Oslo"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Paris"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Prague"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Rome"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Stockholm"
    "CET-1CEST,M3.5.0,M10.5.0/3", //       "Europe/Warsaw"
    "NZST-12NZDT,M9.5.0,M4.1.0", //       "Pacific/Auckland"
    "CHAST-12:45CHADT,M9.5.0,M4.1.0/3", //       "Pacific/Chatham"
    "FJT-12", //       "Pacific/Fiji"
    "<-06>6", //       "Pacific/Galapagos"
    "ChST-10", //       "Pacific/Guam"
    "HST10", //       "Pacific/Honolulu"
    "<+14>-14", //       "Pacific/Kiritimati"
    "<-11>11", //       "Pacific/Niue"
    "PGT-10", //       "Pacific/Port_Moresby"
    "TAHT10", //       "Pacific/Tahiti"
    "UTC0" // "UTC"
};

class GlobalTime {
public:
    static GlobalTime *getInstance();
    static time_t getUnixEpochIfAvailable();

    void updateTime(bool force = false);
    void getHourAndMinute(int &hour, int &minute);
    int getHour();
    int getHour24();
    String getHourPadded();
    int getMinute();
    String getMinutePadded();
    String getSecondPadded();
    time_t getUnixEpoch();
    int getSecond();
    int getDay();
    int getDayOfWeek();
    int getMonth();
    String getMonthName();
    int getYear();
    String getTime();
    String getWeekday();
    String getDayAndMonth();
    bool isPM();
    bool getFormat24Hour();
    bool setFormat24Hour(bool format24hour);
    int getTimeZoneOffset();

private:
    GlobalTime();
    ~GlobalTime();

    static GlobalTime *m_instance;
    ESP32Time rtc;

    time_t m_unixEpoch = 0;
    String m_monthName = "";
    String m_time = "";
    String m_weekday = "";
    //    std::string m_timezoneLocation = TIMEZONE_API_LOCATION;
    int m_timezone = 0;
    String m_timezonePosix = "";
    int m_timeZoneOffset = -1; // A value that will be overwritten by the API

    unsigned long m_updateInterval = 900000; // Update every 15 min
    //    const int m_lowYearTest = 2025;
    //    const int m_highYearTest = 2035;
    unsigned long m_oneSecond = 1000;
    unsigned long m_oneHour = 3600000;
    unsigned long m_updateTimer = 0;
    unsigned long m_updateRTC = 0;

    bool m_format24hour{FORMAT_24_HOUR};
    std::string m_ntpServer{NTP_SERVER};
};

#endif // GLOBALTIME_H
