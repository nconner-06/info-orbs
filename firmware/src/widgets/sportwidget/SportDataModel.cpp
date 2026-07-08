#include "SportDataModel.h"
#include <ArduinoLog.h>

SportDataModel::SportDataModel() {
}

SportDataModel &SportDataModel::setLeagueId(int leagueId) {
    if (m_leagueId != leagueId) {
        m_leagueId = leagueId;
        m_changed = true;
    }
    return *this;
}
int SportDataModel::getLeagueId() {
    return m_leagueId;
}

SportDataModel &SportDataModel::setLeagueName(String leagueName) {
    if (m_leagueName != leagueName) {
        m_leagueName = leagueName;
        m_changed = true;
    }
    return *this;
}

String SportDataModel::getLeagueName() {
    return m_leagueName;
}

SportDataModel &SportDataModel::setLeagueUid(String leagueUid) {
    if (m_leagueUid != leagueUid) {
        m_leagueUid = leagueUid;
        m_changed = true;
    }
    return *this;
}

String SportDataModel::getLeagueUid() {
    return m_leagueUid;
}

SportDataModel &SportDataModel::setSeasonType(String seasonType) {
    if (m_seasonType != seasonType) {
        m_seasonType = seasonType;
        m_changed = true;
    }
    return *this;
}
String SportDataModel::getSeasonType() {
    return m_seasonType;
}

void SportDataModel::setEvents(SportEventDataModel *events, int count) {
    Log.infoln("setEvents() count=%d", count);
    delete[] m_events;
    m_events = events;
    m_eventCount = count;
}

SportEventDataModel &SportDataModel::getEvent(int index) {
    return m_events[index];
}

void SportDataModel::setEventCount(int index) {
    if (m_eventCount != index) {
        m_eventCount = index;
        m_changed = true;
    }
}

int SportDataModel::getEventCount() {
    return m_eventCount;
}

bool SportDataModel::isChanged() {
    return m_changed;
}
SportDataModel &SportDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}

bool SportDataModel::isInitialized() {
    return m_initialized;
}

SportDataModel &SportDataModel::setInitializationStatus(bool initialized) {
    m_initialized = initialized;
    return *this;
}
