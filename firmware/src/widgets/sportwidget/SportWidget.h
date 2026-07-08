#ifndef SPORTWIDGET_H
#define SPORTWIDGET_H

#include "GlobalTime.h"
#include "SportDataModel.h"
#include "Utils.h"
#include "Widget.h"
#include "config_helper.h"
#include <ArduinoJson.h>
#include <LittleFS.h>
#include <TFT_eSPI.h>
#include <TaskManager.h>

#ifndef SPORT_API_URL
    #define SPORT_API_URL "https://site.api.espn.com/apis/site/v2/sports/"
#endif

#ifndef SPORT_LOGO_URL1
    #define SPORT_LOGO_URL1 "http://a.espncdn.com/combiner/i?img="
#endif

#ifndef SPORT_LOGO_URL2
    #define SPORT_LOGO_URL2 "&w=180&h=180&cquality=40&scale=crop&location=origin&transparent=true"
#endif

#define MAX_IMAGE_WIDTH 240 // Adjust for your images

class SportWidget : public Widget {
public:
    SportWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force = false) override;
    void draw(bool force = false) override;
    void onLeave(bool force = false) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    void LSprocessResponse(SportDataModel &leagueData, int httpCode, const String &response);
    String getName() override;

private:
    SportDataModel m_leagueData;
    String calculateDateRange(int period);
    void drawTeamLogoScreen(int *idx, int *m_currentEvent);
    void getLogos(int *event);
    void getHttpLogos(String *url, String *fileName);
    void breakString(String m_text, String textArr[2]);
    void drawLeagueScoreboard(bool force);

    const int m_rangeList[4] = {1, 7, 14, 0};
    const String m_sportList[9] = {"nfl", "nba", "mlb", "nhl", "soccer", "soccer", "soccer", "soccer", "soccer"};
    const String m_leagueList[9] = {"football/nfl", "basketball/nba", "baseball/mlb", "hockey/nhl", "soccer/eng.1", "soccer/eng.2", "soccer/fra.1", "soccer/ger.1", "soccer/fifa.world"};

    int m_sportScreen = 0;
    int m_league = 0;
    int m_currentLeague = 0;
    int m_period = 0;
    int m_currentPeriod = 0;
    int m_startRange = 0;
    int m_endRange = 0;
    int m_recordLimit = 50;
    int m_currentRecordLimit = 50;
    int m_currentEvent = 0;
    String m_h_Logo = "";
    String m_a_Logo = "";

    GlobalTime *m_time;

    int m_switchinterval = 10;
    unsigned long m_prevMillisSwitch = 0;

    WidgetTimer &m_drawTimer;
    WidgetTimer &m_updateTimer;

#ifndef SPORT_UPDATE_DELAY
    #define SPORT_UPDATE_DELAY TimeFrequency::ThirtyMinutes
#endif

#ifndef SPORT_DRAW_DELAY
    #define SPORT_DRAW_DELAY TimeFrequency::FiveSeconds
#endif
};

#endif