#ifndef SPORTEVENTDATEMODEL_H
#define SPORTEVENTDATEMODEL_H

#include <Arduino.h>
#include <vector>

class SportEventDataModel {
public:
    SportEventDataModel();

    SportEventDataModel &setEventId(int eventId);
    int getEventId();
    SportEventDataModel &setEventName(String eventName);
    String getEventName();
    SportEventDataModel &setEventDate(String eventDate);
    String getEventDate();
    SportEventDataModel &setEventStatus(String eventStatus);
    String getEventStatus();
    SportEventDataModel &setEventStatusDetail(String eventStatusDetail);
    String getEventStatusDetail();

    SportEventDataModel &setHomeId(int homeId);
    int getHomeId();
    SportEventDataModel &setAwayId(int awayId);
    int getAwayId();
    SportEventDataModel &setHomeName(String homeName);
    String getHomeName();
    SportEventDataModel &setAwayName(String awayName);
    String getAwayName();
    SportEventDataModel &setHomeColor(String homeColor);
    String getHomeColor();
    SportEventDataModel &setAwayColor(String awayColor);
    String getAwayColor();
    SportEventDataModel &setHomeWin(bool homeWin);
    bool getHomeWin();
    SportEventDataModel &setAwayWin(bool awayWin);
    bool getAwayWin();
    SportEventDataModel &setHomeScore(int homeScore);
    int getHomeScore();
    SportEventDataModel &setAwayScore(int awayScore);
    int getAwayScore();
    SportEventDataModel &setHomeFileName(String homeFileName);
    String getHomeFileName();
    SportEventDataModel &setHomeLogoSub(String homeLogo);
    String getHomeLogoSub();
    SportEventDataModel &setAwayFileName(String awayFileName);
    String getAwayFileName();
    SportEventDataModel &setAwayLogoSub(String awayLogo);
    String getAwayLogoSub();
    SportEventDataModel &setHomeAbbrev(String homeAbbrev);
    String getHomeAbbrev();
    SportEventDataModel &setAwayAbbrev(String awayAbbrev);
    String getAwayAbbrev();

    String getAwayRecord();
    bool isLogoInitialized();
    SportEventDataModel &setLogoInitializationStatus(bool initialized);

private:
    int m_eventId = 0;
    String m_eventName = "";
    String m_eventDate = "";
    String m_eventStatus = "";
    String m_eventStatusDetail = "";

    int m_homeId = 0;
    int m_awayId = 0;
    String m_homeName = "";
    String m_awayName = "";
    String m_homeColor = "";
    String m_awayColor = "";
    String m_homeLogo = "";
    String m_awayLogo = "";
    String m_homeFileName = "";
    String m_homeLogoSub = "";
    String m_awayFileName = "";
    String m_awayLogoSub = "";
    String m_homeAbbrev = "";
    String m_awayAbbrev = "";

    bool m_homeWin = false;
    bool m_awayWin = false;
    int m_homeScore = 0;
    int m_awayScore = 0;

    bool m_hasLogo = false;

    bool m_changed = false;
    bool m_initialized = false;
    bool m_logoInitialized = false;
};

#endif