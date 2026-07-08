#include "SportWidget.h"
#include "LittleFSHelper.h"
#include "SportTranslations.h"
#include "TaskFactory.h"
#include <ArduinoLog.h>
#include <HTTPClient.h>

SportWidget::SportWidget(ScreenManager &manager, ConfigManager &config)
    : Widget(manager, config),
      m_drawTimer(addDrawRefreshFrequency(SPORT_DRAW_DELAY)),
      m_updateTimer(addUpdateRefreshFrequency(SPORT_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_SPORT == WIDGET_ON);
    m_time = GlobalTime::getInstance();

    m_config.addConfigBool("SportWidget", "SportEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigComboBox("SportWidget", "SportLeague", &m_league, t_LeagueOptions, t_League, false);
    m_config.addConfigComboBox("SportWidget", "SportPeriod", &m_period, t_RangeOptions, t_Range, false);
    m_config.addConfigInt("SportWidget", "SportStart", &m_startRange, t_StartRange, true);
    m_config.addConfigInt("SportWidget", "SportEnd", &m_endRange, t_EndRange, true);
    m_config.addConfigInt("SportWidget", "SportLimit", &m_recordLimit, t_recordLimit, true);
    m_config.addConfigInt("SportWidget", "SportCycle", &m_switchinterval, t_sportCycle, true);

    m_currentLeague = m_league;
    m_currentPeriod = m_period;
    if (m_recordLimit > 75)
        m_recordLimit = 75;
    m_currentRecordLimit = m_recordLimit;
}

String SportWidget::calculateDateRange(int period) {
    String r_range = "";
    time_t l_startDate = 0;
    time_t l_endDate = 0;
    const time_t l_unixEpoch = m_time->getUnixEpoch() + m_time->getTimeZoneOffset();

    int l_range = m_rangeList[period];

    if (l_range != 0) {
        l_startDate = l_unixEpoch - (86400 * l_range);
        l_endDate = l_unixEpoch + (86400 * l_range);
    } else {
        l_startDate = l_unixEpoch + (86400 * m_startRange);
        l_endDate = l_startDate + (86400 * m_endRange);
    }

#ifdef SPORT_DEBUG
    Log.infoln("Epoch : %i, Offset : %i\n", m_time->getUnixEpoch(), m_time->getTimeZoneOffset());
#endif

    struct tm *lv_startTime = gmtime(&l_startDate);

#ifdef SPORT_DEBUG
    Log.infoln("Start : %i, year : %i, month : %i, day : %i\n", l_startDate, lv_startTime->tm_year + 1900, lv_startTime->tm_mon + 1, lv_startTime->tm_mday);
#endif

    r_range = String(lv_startTime->tm_year + 1900) +
              (((lv_startTime->tm_mon + 1) < 10) ? "0" : "") + String(lv_startTime->tm_mon + 1) +
              ((lv_startTime->tm_mday < 10) ? "0" : "") + String(lv_startTime->tm_mday) + "-";

    struct tm *lv_endTime = gmtime(&l_endDate);

#ifdef SPORT_DEBUG
    Log.infoln("End   : %i, year : %i, month : %i, day : %i\n", l_endDate, lv_endTime->tm_year + 1900, lv_endTime->tm_mon + 1, lv_endTime->tm_mday);
#endif

    r_range = r_range + String(lv_endTime->tm_year + 1900) +
              (((lv_endTime->tm_mon + 1) < 10) ? "0" : "") + String(lv_endTime->tm_mon + 1) +
              ((lv_endTime->tm_mday < 10) ? "0" : "") + String(lv_endTime->tm_mday);
    return r_range;
}

void SportWidget::setup() {
    m_prevMillisSwitch = millis();
    m_currentEvent = 0;
}

void SportWidget::update(bool force) {
    const String date_range = calculateDateRange(m_currentPeriod);
    String url = String(SPORT_API_URL) + m_leagueList[m_currentLeague] + "/scoreboard?dates=" + date_range + "&limit=" + m_currentRecordLimit;

    JsonDocument jfilter;
    JsonObject jfilter_leagues_0 = jfilter["leagues"].add<JsonObject>();
    jfilter_leagues_0["id"] = true;
    jfilter_leagues_0["uid"] = true;
    jfilter_leagues_0["name"] = true;
    jfilter_leagues_0["season"]["type"]["name"] = true;

    JsonObject jfilter_events_0 = jfilter["events"].add<JsonObject>();
    jfilter_events_0["id"] = true;
    jfilter_events_0["date"] = true;
    jfilter_events_0["shortName"] = true;
    JsonObject filter_events_0_competitions_0 = jfilter_events_0["competitions"].add<JsonObject>();
    filter_events_0_competitions_0["status"]["type"]["name"] = true;
    filter_events_0_competitions_0["status"]["type"]["detail"] = true;
    JsonObject jfilter_events_0_competitions_0_competitors_0 = jfilter_events_0["competitions"][0]["competitors"].add<JsonObject>();
    jfilter_events_0_competitions_0_competitors_0["id"] = true;
    jfilter_events_0_competitions_0_competitors_0["winner"] = true;
    jfilter_events_0_competitions_0_competitors_0["score"] = true;
    JsonObject jfilter_events_0_competitions_0_competitors_0_team = jfilter_events_0_competitions_0_competitors_0["team"].to<JsonObject>();
    jfilter_events_0_competitions_0_competitors_0_team["displayName"] = true;
    jfilter_events_0_competitions_0_competitors_0_team["color"] = true;
    jfilter_events_0_competitions_0_competitors_0_team["abbreviation"] = true;

    String filter = "";
    filter = jfilter.as<String>();

    auto task = TaskFactory::createHttpGetTask(
        url, filter, [this](int httpCode, const String &response) {
            LSprocessResponse(m_leagueData, httpCode, response);
        });

    if (!task) {
        Log.errorln("Failed to create sport task");
        return;
    }

    bool success = TaskManager::getInstance()->addTask(std::move(task));
    if (!success) {
        Log.errorln("Failed to add sport task");
        return;
    }
}

void SportWidget::LSprocessResponse(SportDataModel &leagueData, int httpCode, const String &response) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            JsonObject leagues_0 = doc["leagues"][0];
            leagueData.setLeagueId(leagues_0["id"].as<int>());
            leagueData.setLeagueUid(leagues_0["uid"].as<String>());
            leagueData.setLeagueName(leagues_0["name"].as<String>());
            leagueData.setSeasonType(leagues_0["season"]["type"]["name"].as<String>());

            JsonArray events = doc["events"];
            auto *eventArray = new SportEventDataModel[events.size() + 1];
            int count = 0;
            for (JsonVariant event : events) {
                if (event["id"].as<int>() > 0) {
                    SportEventDataModel l_e = SportEventDataModel();
                    l_e.setEventId(event["id"].as<int>());
                    l_e.setEventName(event["shortName"].as<String>());
                    l_e.setEventDate(event["date"].as<String>());
                    l_e.setEventStatus(event["competitions"][0]["status"]["type"]["name"].as<String>());
                    l_e.setEventStatusDetail(event["competitions"][0]["status"]["type"]["detail"].as<String>());
                    l_e.setHomeId(event["competitions"][0]["competitors"][0]["id"].as<int>());
                    l_e.setHomeWin(event["competitions"][0]["competitors"][0]["winner"].as<bool>());
                    l_e.setHomeScore(event["competitions"][0]["competitors"][0]["score"].as<int>());
                    l_e.setHomeName(event["competitions"][0]["competitors"][0]["team"]["displayName"].as<String>());
                    l_e.setHomeColor(event["competitions"][0]["competitors"][0]["team"]["color"].as<String>());
                    l_e.setHomeAbbrev(event["competitions"][0]["competitors"][0]["team"]["abbreviation"].as<String>());
                    l_e.setAwayId(event["competitions"][0]["competitors"][1]["id"].as<int>());
                    l_e.setAwayWin(event["competitions"][0]["competitors"][1]["winner"].as<bool>());
                    l_e.setAwayScore(event["competitions"][0]["competitors"][1]["score"].as<int>());
                    l_e.setAwayName(event["competitions"][0]["competitors"][1]["team"]["displayName"].as<String>());
                    l_e.setAwayColor(event["competitions"][0]["competitors"][1]["team"]["color"].as<String>());
                    l_e.setAwayAbbrev(event["competitions"][0]["competitors"][1]["team"]["abbreviation"].as<String>());

                    if (leagueData.getLeagueUid().substring(2, 5) == "600" && leagueData.getLeagueId() != 606) {
                        l_e.setHomeFileName("/" + String(l_e.getHomeId()) + ".png");
                        l_e.setAwayFileName("/" + String(l_e.getAwayId()) + ".png");
                    } else {
                        l_e.setHomeFileName("/" + l_e.getHomeAbbrev() + ".png");
                        l_e.setAwayFileName("/" + l_e.getAwayAbbrev() + ".png");
                    }
                    if (leagueData.getLeagueId() != 606) {
                        l_e.setHomeLogoSub("/i/teamlogos/" + m_sportList[m_currentLeague] + "/500" + l_e.getHomeFileName());
                        l_e.setAwayLogoSub("/i/teamlogos/" + m_sportList[m_currentLeague] + "/500" + l_e.getAwayFileName());
                    } else {
                        l_e.setHomeLogoSub("/i/teamlogos/countries/500" + l_e.getHomeFileName());
                        l_e.setAwayLogoSub("/i/teamlogos/countries/500" + l_e.getAwayFileName());
                    }
                    l_e.setLogoInitializationStatus(false);
                    eventArray[count++] = l_e;
                }
            }
            leagueData.setEvents(eventArray, count);
            leagueData.setInitializationStatus(true);
            leagueData.setChangedStatus(true);
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    } else {
        Log.errorln("HTTP request failed, error code: %d\n", httpCode);
    }
}

void SportWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);

    if (!m_leagueData.isInitialized()) {
        m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
        for (int displayIndex = 0; displayIndex < 5; displayIndex++) {
            m_manager.fillSprite(TFT_BLACK);
            m_manager.drawCentreString(I18n::get(t_loadingEventData), ScreenCenterX, ScreenCenterY, 20);
            m_manager.pushSprite(displayIndex, 0, 0);
        }
        m_prevMillisSwitch = millis(); // Reset timer
        m_currentEvent = 0;
    } else {
        drawLeagueScoreboard(force);
    }
}

void SportWidget::drawLeagueScoreboard(bool force) {
    String l_str = "";
    String l_eventStatus = "";
    String l_leagueName = "";
    String l_teamName = "";
    String l_textArr[2] = {};
    int l_y = 0;
    int l_idx = 0;
    int lv_f = 0;

    if ((m_leagueData.isChanged() || force) && m_leagueData.isInitialized()) {
        if (m_leagueData.getEventCount() == 0) {
            m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
            for (int displayIndex = 0; displayIndex < 5; displayIndex++) {
                m_manager.fillSprite(TFT_BLACK);
                m_manager.drawCentreString(I18n::get(t_noEvents), ScreenCenterX, ScreenCenterY, 16);
                m_manager.pushSprite(displayIndex, 0, 0);
            }
        } else {
            SportEventDataModel l_e = m_leagueData.getEvent(m_currentEvent);
            getLogos(&m_currentEvent);

            // draw screen 0
            drawTeamLogoScreen(&l_idx, &m_currentEvent);

            // draw screen 1
            m_manager.setFont(DEFAULT_FONT);
            m_manager.setFontColor(TFT_WHITE);
            m_manager.fillSprite(TFT_BLACK);
            m_manager.drawArc(120, 120, 120, 115, 0, 360, Utils::rgb888htmlToRgb565(l_e.getHomeColor()), TFT_BLACK);
            l_teamName = l_e.getHomeName() + " ";

            l_textArr[0] = "";
            l_textArr[1] = "";
            if (l_teamName.length() > 15)
                breakString(l_teamName, l_textArr);
            else
                l_textArr[0] = l_e.getHomeName();

            l_y = 40;
            m_manager.drawString(l_textArr[0], 120, l_y, 20, Align::Center);
            l_y += 35;
            lv_f = ((l_textArr[0] == "") ? 16 : 20);
            m_manager.drawString(l_textArr[1], 120, l_y, lv_f, Align::Center);

            l_eventStatus = l_e.getEventStatus();
            if (l_eventStatus == "STATUS_SCHEDULED") {
                l_str = "Scheduled";
            } else if (l_eventStatus == "STATUS_FIRST_HALF" || l_eventStatus == "STATUS_HALFTIME" || l_eventStatus == "STATUS_SECOND_HALF" || l_eventStatus == "STATUS_IN_PROGRESS" || l_eventStatus == "STATUS_END_PERIOD" || l_eventStatus == "STATUS_OVERTIME") {
                m_manager.drawString(String(l_e.getHomeScore()), 120, 100, 55, Align::Center);
                l_str = "In progress";
            } else if (l_eventStatus == "STATUS_FULL_TIME" || l_eventStatus == "STATUS_FINAL" || l_eventStatus == "STATUS_FINAL_PEN" || l_eventStatus == "STATUS_FINAL_AET") {
                m_manager.drawString(String(l_e.getHomeScore()), 120, 100, 55, Align::Center);
                if (!l_e.getHomeWin() && !l_e.getAwayWin())
                    l_str = "Draw";
                else {
                    if (l_e.getHomeWin())
                        l_str = "Won";
                    else
                        l_str = "Lost";
                }
            } else
                l_str = l_eventStatus;

            m_manager.drawString(l_str, 120, 190, 16, Align::Center);

            l_idx = 1;
            m_manager.pushSprite(l_idx, 0, 0);

            // draw screen 2
            m_manager.setFont(DEFAULT_FONT);
            m_manager.setFontColor(TFT_WHITE);
            m_manager.fillSprite(TFT_BLACK);
            l_leagueName = m_leagueData.getLeagueName() + " ";
            l_textArr[0] = "";
            l_textArr[1] = "";
            if (l_leagueName.length() > 15)
                breakString(l_leagueName, l_textArr);
            else
                l_textArr[0] = m_leagueData.getLeagueName();

            l_y = 40;
            for (int i = 0; i < 2; i++) {
                m_manager.drawString(l_textArr[i], 120, l_y, 20, Align::Center);
                l_y += 35;
            }
            m_manager.drawString(l_e.getEventDate().substring(0, 10), 120, 120, 18, Align::Center);
            if (l_e.getEventStatus() != "STATUS_SCHEDULED")
                m_manager.drawString(l_e.getEventStatusDetail(), 120, 160, 18, Align::Center);
            else
                m_manager.drawString(l_e.getEventDate().substring(11, 16) + " (UTC)", 120, 160, 18, Align::Center);
            String l_page = String(m_currentEvent + 1) + "/" + String(m_leagueData.getEventCount());
            m_manager.drawString(l_page, 120, 200, 14, Align::Center);
            l_idx = 2;
            m_manager.pushSprite(l_idx, 0, 0);

            // draw screen 3
            m_manager.setFont(DEFAULT_FONT);
            m_manager.setFontColor(TFT_WHITE);
            m_manager.fillSprite(TFT_BLACK);
            m_manager.drawArc(120, 120, 120, 115, 0, 360, Utils::rgb888htmlToRgb565(l_e.getAwayColor()), TFT_BLACK);

            l_teamName = l_e.getAwayName() + " ";
            l_textArr[0] = "";
            l_textArr[1] = "";
            if (l_teamName.length() > 15)
                breakString(l_teamName, l_textArr);
            else
                l_textArr[0] = l_e.getAwayName();

            l_y = 40;
            m_manager.drawString(l_textArr[0], 120, l_y, 20, Align::Center);
            l_y += 35;
            lv_f = ((l_textArr[0] == "") ? 16 : 20);
            m_manager.drawString(l_textArr[1], 120, l_y, lv_f, Align::Center);

            l_eventStatus = l_e.getEventStatus();
            if (l_eventStatus == "STATUS_SCHEDULED") {
                l_str = "Scheduled";
            } else if (l_eventStatus == "STATUS_FIRST_HALF" || l_eventStatus == "STATUS_HALFTIME" || l_eventStatus == "STATUS_SECOND_HALF" || l_eventStatus == "STATUS_IN_PROGRESS" || l_eventStatus == "STATUS_END_PERIOD" || l_eventStatus == "STATUS_OVERTIME") {
                m_manager.drawString(String(l_e.getAwayScore()), 120, 100, 55, Align::Center);
                l_str = "In progress";
            } else if (l_eventStatus == "STATUS_FULL_TIME" || l_eventStatus == "STATUS_FINAL" || l_eventStatus == "STATUS_FINAL_PEN" || l_eventStatus == "STATUS_FINAL_AET") {
                m_manager.drawString(String(l_e.getAwayScore()), 120, 100, 55, Align::Center);
                if (!l_e.getHomeWin() && !l_e.getAwayWin())
                    l_str = "Draw";
                else {
                    if (l_e.getAwayWin())
                        l_str = "Won";
                    else
                        l_str = "Lost";
                }
            } else
                l_str = l_eventStatus;

            m_manager.drawString(l_str, 120, 190, 16, Align::Center);

            l_idx = 3;
            m_manager.pushSprite(l_idx, 0, 0);

            // draw screen 4
            l_idx = 4;
            drawTeamLogoScreen(&l_idx, &m_currentEvent);

            m_leagueData.setChangedStatus(false);
        }
    }

    if ((millis() - m_prevMillisSwitch >= (m_switchinterval * 1000)) && m_switchinterval > 0) {
        m_currentEvent++;
        if (m_currentEvent == m_leagueData.getEventCount())
            m_currentEvent = 0;
        m_leagueData.setChangedStatus(true);
        m_prevMillisSwitch = millis(); // Reset timer
    }
}

void SportWidget::breakString(String m_text, String textArr[2]) {
    const int l_length = 15;
    int variableRangeS = 0;
    int variableRangeE = l_length;
    textArr[0] = "";
    textArr[1] = "";
    int lv_space = m_text.indexOf(" ");
    if (lv_space < 15) {
        for (int i = 0; i < 2; i++) {
            while (m_text.substring(variableRangeE - 1, variableRangeE) != " ") {
                variableRangeE--;
            }
            textArr[i] = m_text.substring(variableRangeS, variableRangeE);
            variableRangeS = variableRangeE;
            variableRangeE = variableRangeS + l_length;
        }
    } else {
        textArr[0] = "";
        textArr[1] = m_text;
    }
}

void SportWidget::getLogos(int *event) {
    String l_logoSub = "";
    String l_fileName = "";
    String l_url = "";
    SportEventDataModel l_e = m_leagueData.getEvent(*event);

    l_fileName = "/home.png";
    l_logoSub = l_e.getHomeLogoSub();
    l_logoSub.toLowerCase();
    l_url = String(SPORT_LOGO_URL1) + l_logoSub + String(SPORT_LOGO_URL2);
    getHttpLogos(&l_url, &l_fileName);

    l_fileName = "/away.png";
    l_logoSub = l_e.getAwayLogoSub();
    l_logoSub.toLowerCase();
    l_url = String(SPORT_LOGO_URL1) + l_logoSub + String(SPORT_LOGO_URL2);
    getHttpLogos(&l_url, &l_fileName);
}

void SportWidget::getHttpLogos(String *url, String *fileName) {
    Utils::setBusy(true);
    HTTPClient http;

    Log.infoln("Logos : %s , %s", fileName->c_str(), url->c_str());

    http.useHTTP10(true);
    http.begin(*url);
    int httpCode = http.GET();
    if (httpCode > 0) {
        File file = LittleFS.open(*fileName, FILE_WRITE);

        // get lenght of document (is -1 when Server sends no Content-Length header)
        int len = http.getSize();

        // create buffer for read
        //        uint8_t buff[64] = {0};
        uint8_t buff[256] = {0};

        // get tcp stream
        WiFiClient *stream = http.getStreamPtr();

        // read all data from server
        while (http.connected() && (len > 0)) { // (len > 0 || len == -1)
            // get available data size
            size_t size = stream->available();

            if (size) {
                // read up to 128 byte
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                file.write(buff, c);
                if (len > 0)
                    len -= c;
            }
            delay(1);
        }
        file.close();
    } else {
        Log.errorln("🔴 HTTP request failed, error code: %d", httpCode);
    }
    http.end();
    Utils::setBusy(false);
}

void SportWidget::drawTeamLogoScreen(int *idx, int *event) {
    String l_fileName = "";
    SportEventDataModel l_e = m_leagueData.getEvent(*event);

    m_manager.fillSprite(TFT_WHITE);

    if (*idx == 0)
        l_fileName = "/home.png";
    else
        l_fileName = "/away.png";

    m_manager.loadPngFile(LittleFS, l_fileName.c_str());
    m_manager.pushSprite(*idx, 0, 0);
}

void SportWidget::onLeave(bool force) {
}

void SportWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        update(true);
    } else if (buttonId == BUTTON_LEFT && state == BTN_MEDIUM) {
        m_currentEvent--;
        if (m_currentEvent < 0)
            m_currentEvent = m_leagueData.getEventCount() - 1;
        m_leagueData.setChangedStatus(true);
        m_prevMillisSwitch = millis(); // Reset timer
    } else if (buttonId == BUTTON_RIGHT && state == BTN_MEDIUM) {
        m_currentEvent++;
        if (m_currentEvent == m_leagueData.getEventCount())
            m_currentEvent = 0;
        m_leagueData.setChangedStatus(true);
        m_prevMillisSwitch = millis(); // Reset timer
    }
}

String SportWidget::getName() {
    return "Sport";
}
