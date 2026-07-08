#ifndef SPORTDATEMODEL_H
#define SPORTDATEMODEL_H

#include "SportEventDataModel.h"
#include <Arduino.h>
#include <vector>

class SportDataModel {
public:
    SportDataModel();

    SportDataModel &setLeagueId(int leagueId);
    int getLeagueId();
    SportDataModel &setLeagueName(String leagueName);
    String getLeagueName();
    SportDataModel &setSeasonType(String seasonType);
    String getSeasonType();
    SportDataModel &setLeagueUid(String leagueUid);
    String getLeagueUid();

    void setEvents(SportEventDataModel *events, int count);
    SportEventDataModel &getEvent(int index);
    void setEventCount(int index);
    int getEventCount();

    bool isChanged();
    SportDataModel &setChangedStatus(bool changed);
    bool isInitialized();
    SportDataModel &setInitializationStatus(bool initialized);

private:
    int m_leagueId = 0;
    String m_leagueName = "";
    String m_leagueUid = "";
    String m_seasonType = "";

    SportEventDataModel *m_events = nullptr;
    int m_eventCount = 0;

    bool m_changed = false;
    bool m_initialized = false;
};

#endif