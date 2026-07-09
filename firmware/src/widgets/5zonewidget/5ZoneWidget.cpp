#include "5ZoneWidget.h"
#include "5ZoneTranslations.h"
#include "TaskFactory.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>

FiveZoneWidget::FiveZoneWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config),
                                                                                m_drawTimer(addDrawRefreshFrequency(FIVEZONE_DRAW_DELAY)),
                                                                                m_updateTimer(addUpdateRefreshFrequency(FIVEZONE_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_5ZONE == WIDGET_ON);
    m_time = GlobalTime::getInstance();

    m_config.addConfigBool("FiveZoneWidget", "5zoEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigBool("FiveZoneWidget", "5zoDefault", &m_default, t_defaultWidget);
    m_config.addConfigBool("FiveZoneWidget", "showBizHours", &m_showBizHours, t_5zoneShowBizHours, false);
    m_config.addConfigBool("FiveZoneWidget", "showSecondTick", &m_showSecondTick, t_clockShowSecondTick, false);
    String optOrg[] = {"0", "1", "2", "3", "4", "5"};
    m_config.addConfigComboBox("FiveZoneWidget", "dimOrg", &s_dimOrg, optOrg, 5, t_dimOrg, false);
    m_config.addConfigComboBox("FiveZoneWidget", "5zoneFormat", &m_format, t_5zoneFormats, t_5zoneFormat);

    for (int i = 0; i < MAX_ZONES; i++) {
        const char *zoneName = strdup((String("5ZoneName") + String(i)).c_str());
        const char *zoneDesc = strdup((i18nStr(t_5zoneDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigString("FiveZoneWidget", zoneName, &m_timeZones[i].locName, 15, zoneDesc, false);

        const char *zoneTZ = strdup((String("5ZoneInfo") + String(i)).c_str());
        const char *zoneTZDesc = strdup((i18nStr(t_5zoneTZDesc) + " " + String(i) + ": ").c_str());
        int l_tzInfo = m_timeZones[i].tzInfo;

        m_config.addConfigComboBox("FiveZoneWidget", zoneTZ, &l_tzInfo, tz_list, 101, t_5zoneTZDesc);
        m_timeZones[i].tzInfo = m_config.getConfigInt(zoneTZ, 0);
    }

    for (int i = 0; i < MAX_ZONES; i++) {
        const char *zoneWorkStart = strdup((String("5ZoneWstart") + String(i)).c_str());
        const char *zoneWorkStartDesc = strdup((i18nStr(t_5zoneWorkStartDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigInt("FiveZoneWidget", zoneWorkStart, &m_timeZones[i].m_workStart, zoneWorkStartDesc, true);

        const char *zoneWorkEnd = strdup((String("5ZoneWend") + String(i)).c_str());
        const char *zoneWorkEndDesc = strdup((i18nStr(t_5zoneWorkEndDesc) + " " + String(i) + ": ").c_str());
        m_config.addConfigInt("FiveZoneWidget", zoneWorkEnd, &m_timeZones[i].m_workEnd, zoneWorkEndDesc, true);
    }
    m_timezone = m_config.getConfigInt("timezone", m_timezone);
}

void FiveZoneWidget::setup() {
}

void FiveZoneWidget::update(bool force) {
    String m_timezonePosix = "";

    struct tm tm_orb;
    struct tm tm_local;
    int l_orb = 0;
    int l_loc = 0;

    for (int i = 0; i < MAX_ZONES; i++) {
        TimeZone &zone = m_timeZones[i];

        if (zone.tzInfo != -1) {
            // https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
            m_timezonePosix = tz_info[zone.tzInfo];
#ifdef ZONE_DEBUG
            Log.infoln("Zone TZ id: %i, Zone : %s", zone.tzInfo, tz_info[zone.tzInfo].c_str());
#endif
            setenv("TZ", m_timezonePosix.c_str(), 1);
            tzset();
            if (!getLocalTime(&tm_local))
                Log.errorln("Error");
            l_loc = (tm_local.tm_hour * 3600) + (tm_local.tm_min * 60);

            m_timezonePosix = tz_info[m_timezone];
#ifdef ZONE_DEBUG
            Log.infoln("Orb TZ id: %i, Zone : %s", m_timezone, tz_info[m_timezone].c_str());
#endif
            setenv("TZ", m_timezonePosix.c_str(), 1);
            tzset();
            if (!getLocalTime(&tm_orb))
                Log.errorln("Error");
            l_orb = (tm_orb.tm_hour * 3600) + (tm_orb.tm_min * 60);

            if (tm_local.tm_mday > tm_orb.tm_mday)
                l_loc = l_loc + 86400;
            if (tm_local.tm_mday < tm_orb.tm_mday)
                l_loc = l_loc - 86400;
            zone.timeZoneOffset = l_loc - l_orb;
#ifdef ZONE_DEBUG
            Log.infoln("zone : %i - Zone Offset: %i", i, zone.timeZoneOffset);
#endif
        }
    }
}

void FiveZoneWidget::changeFormat() {
    GlobalTime *time = GlobalTime::getInstance();
    m_format++;
    if (m_format > 1)
        m_format = 0;
    m_config.putConfigInt("5zoneFormat", m_format);
    draw(true);
}

void FiveZoneWidget::draw(bool force) {
    for (int i = 0; i < MAX_ZONES; i++) {
        displayZone(i, force);
    }
}

void FiveZoneWidget::displayZone(int8_t displayIndex, bool force) {
    const int nameY = 50; // Zone name at top
    const int dateY = 75; // Date indicator below name
    const int clockY = 115; // Time in middle
    const int ampmY = 175; // AM/PM indicator
    const int offsetY = 200; // Offset at bottom
    String lv_displayHour = "";
    String lv_offsetStr = " ";
    int lv_ringColor;
    String lv_dateIndicator = "";
    String lv_displayAM = "";
    time_t lv_unixEpoch;
    int lv_localDay;
    int lv_zoneDiff;
    int lv_hour;
    int lv_minute;
    int lv_second;
    int lv_day;
    int lv_weekday;
    int lv_hourD;
    int lv_minuteD;

    TimeZone &zone = m_timeZones[displayIndex];
    if (zone.locName != "") {
        // Get Orb (local) time information
        m_localTimeZone.locName = "Local Time";
        m_localTimeZone.timeZoneOffset = m_time->getTimeZoneOffset();
        m_unixEpoch = m_time->getUnixEpoch();

        // Get Time information for this TZ
        lv_unixEpoch = m_unixEpoch + zone.timeZoneOffset + m_localTimeZone.timeZoneOffset;
#ifdef ZONE_DEBUG
        Log.infoln("displayzone : %i - Zone Offsets: %i, %i, %i", displayIndex, m_unixEpoch, zone.timeZoneOffset, m_localTimeZone.timeZoneOffset);
#endif

        struct tm *lv_time = gmtime(&lv_unixEpoch);
        lv_hour = lv_time->tm_hour;
        lv_minute = lv_time->tm_min;
        lv_second = lv_time->tm_sec;
        lv_day = lv_time->tm_mday;
        lv_weekday = lv_time->tm_wday;

        // Calculate offset from local time
        lv_zoneDiff = zone.timeZoneOffset; // - m_localTimeZone.timeZoneOffset; // Difference between target UTC offset and local UTC offset
        lv_hourD = lv_zoneDiff / 3600;
        lv_minuteD = ((lv_zoneDiff / 60) % 60) * ((lv_zoneDiff < 0 ? -1 : 1));

        // calculate if day offset
        if (lv_zoneDiff > 0) {
            lv_offsetStr = "+";
            lv_ringColor = m_afterLocalTzColour;
        } else if (lv_zoneDiff < 0) {
            lv_offsetStr = "-";
            lv_hourD = lv_hourD * -1;
            lv_ringColor = m_beforeLocalTzColour;
        } else
            lv_ringColor = m_sameLocalTzColour;
        lv_offsetStr = lv_offsetStr + ((lv_hourD < 10) ? "0" : "") + String(lv_hourD) + ":" + ((lv_minuteD < 10) ? "0" : "") + String(lv_minuteD);

        // calculate if crossing date line
        lv_localDay = m_time->getDay();
        if (lv_localDay != lv_day) {
            if (lv_unixEpoch > m_unixEpoch)
                lv_dateIndicator = "+1d";
            else
                lv_dateIndicator = "-1d";
        }

        // 12/24 hour formate and AM/PM indicator
        if (m_format == 0) {
            lv_displayHour = ((lv_hour < 10) ? "0" : "") + String(lv_hour);
            lv_displayAM = "";
        } else {
            lv_displayHour = String((lv_hour > 12) ? lv_hour - 12 : lv_hour);
            lv_displayAM = (lv_hour < 12) ? "AM" : "PM";
        }

        m_foregroundColor = m_workColour;
        m_manager.setFont(DEFAULT_FONT);
        m_manager.setFontColor(m_foregroundColor);
        m_manager.fillSprite(m_backgroundColor);
        m_manager.drawString(zone.locName.c_str(), ScreenCenterX, nameY, 18, Align::MiddleCenter);

        m_manager.drawString(lv_dateIndicator, ScreenCenterX - 60, ampmY, 16, Align::MiddleCenter);
        zone.m_lastDateIndicator = lv_dateIndicator;

        m_manager.drawString(lv_displayAM, ScreenCenterX + 60, ampmY, 16, Align::MiddleCenter);
        zone.m_lastDisplayAM = lv_displayAM;

        if (zone.timeZoneOffset == -1)
            m_manager.setFontColor(TFT_RED);
        m_manager.drawString(lv_offsetStr, ScreenCenterX, offsetY, 16, Align::MiddleCenter);
        m_manager.setFontColor(m_foregroundColor);
        zone.m_zoneDiff = lv_zoneDiff;

        if (m_showBizHours) {
            m_manager.drawArc(120, 120, 120, 115, 0, 360, lv_ringColor, m_backgroundColor);
            if (isWeekend(lv_weekday)) {
                m_foregroundColor = m_weekendColor;
                m_manager.setFontColor(m_foregroundColor);
            } else {
                if (lv_hour < zone.m_workStart || lv_hour >= zone.m_workEnd) {
                    m_foregroundColor = m_afterWorkColour;
                    m_manager.setFontColor(m_foregroundColor);
                } else {
                    m_foregroundColor = m_workColour;
                    m_manager.setFontColor(m_foregroundColor);
                }
            }
        } else
            lv_ringColor = m_sameLocalTzColour;

        if (m_showSecondTick && displayIndex == s_dimOrg) {
            const int startA = ((lv_second * 6) + 180 - 3) % 360;
            const int endA = ((lv_second * 6) + 180 + 3) % 360;
            int color;
            if (lv_ringColor == m_sameLocalTzColour)
                color = TFT_WHITE;
            else
                color = TFT_BLACK;
            m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 115, startA, endA, color, TFT_BLACK);
        }

        String lv_minuteStr = (lv_minute < 10) ? "0" + String(lv_minute) : String(lv_minute);
        String lv_displayTime = lv_displayHour + ":" + lv_minuteStr;
        m_manager.drawString(lv_displayTime, ScreenCenterX, clockY, 62, Align::MiddleCenter);
        m_manager.pushSprite(displayIndex, 0, 0);
    }
}

void FiveZoneWidget::onLeave(bool force) {
}

void FiveZoneWidget ::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        changeFormat();
    }
}

String FiveZoneWidget ::getName() {
    return "5 Zone Clock";
}
