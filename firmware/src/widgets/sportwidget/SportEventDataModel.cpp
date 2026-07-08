#include "SportEventDataModel.h"
#include "Utils.h"
#include "config_helper.h"

SportEventDataModel::SportEventDataModel() {
}

// Event Details
SportEventDataModel &SportEventDataModel::setEventId(int eventId) {
    if (m_eventId != eventId) {
        m_eventId = eventId;
        m_changed = true;
    }
    return *this;
}
int SportEventDataModel::getEventId() {
    return m_eventId;
}

SportEventDataModel &SportEventDataModel::setEventName(String eventName) {
    if (m_eventName != eventName) {
        m_eventName = eventName;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getEventName() {
    return m_eventName;
}

SportEventDataModel &SportEventDataModel::setEventDate(String eventDate) {
    if (m_eventDate != eventDate) {
        m_eventDate = eventDate;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getEventDate() {
    return m_eventDate;
}

SportEventDataModel &SportEventDataModel::setEventStatus(String eventStatus) {
    if (m_eventStatus != eventStatus) {
        m_eventStatus = eventStatus;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getEventStatus() {
    return m_eventStatus;
}

SportEventDataModel &SportEventDataModel::setEventStatusDetail(String eventStatusDetail) {
    if (m_eventStatusDetail != eventStatusDetail) {
        m_eventStatusDetail = eventStatusDetail;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getEventStatusDetail() {
    return m_eventStatusDetail;
}

// Team Details
SportEventDataModel &SportEventDataModel::setHomeId(int homeId) {
    if (m_homeId != homeId) {
        m_homeId = homeId;
        m_changed = true;
    }
    return *this;
}

int SportEventDataModel::getHomeId() {
    return m_homeId;
}

SportEventDataModel &SportEventDataModel::setAwayId(int awayId) {
    if (m_awayId != awayId) {
        m_awayId = awayId;
        m_changed = true;
    }
    return *this;
}
int SportEventDataModel::getAwayId() {
    return m_awayId;
}

SportEventDataModel &SportEventDataModel::setHomeName(String homeName) {
    if (m_homeName != homeName) {
        m_homeName = homeName;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getHomeName() {
    return m_homeName;
}

SportEventDataModel &SportEventDataModel::setAwayName(String awayName) {
    if (m_awayName != awayName) {
        m_awayName = awayName;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getAwayName() {
    return m_awayName;
}

SportEventDataModel &SportEventDataModel::setHomeColor(String homeColor) {
    if (m_homeColor != homeColor) {
        m_homeColor = homeColor;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getHomeColor() {
    return m_homeColor;
}

SportEventDataModel &SportEventDataModel::setAwayColor(String awayColor) {
    if (m_awayColor != awayColor) {
        m_awayColor = awayColor;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getAwayColor() {
    return m_awayColor;
}

SportEventDataModel &SportEventDataModel::setHomeWin(bool homeWin) {
    if (m_homeWin != homeWin) {
        m_homeWin = homeWin;
        m_changed = true;
    }
    return *this;
}
bool SportEventDataModel::getHomeWin() {
    return m_homeWin;
}

SportEventDataModel &SportEventDataModel::setAwayWin(bool awayWin) {
    if (m_awayWin != awayWin) {
        m_awayWin = awayWin;
        m_changed = true;
    }
    return *this;
}
bool SportEventDataModel::getAwayWin() {
    return m_awayWin;
}

SportEventDataModel &SportEventDataModel::setHomeScore(int homeScore) {
    if (m_homeScore != homeScore) {
        m_homeScore = homeScore;
        m_changed = true;
    }
    return *this;
}

int SportEventDataModel::getHomeScore() {
    return m_homeScore;
}

SportEventDataModel &SportEventDataModel::setAwayScore(int awayScore) {
    if (m_awayScore != awayScore) {
        m_awayScore = awayScore;
        m_changed = true;
    }
    return *this;
}
int SportEventDataModel::getAwayScore() {
    return m_awayScore;
}

bool SportEventDataModel::isLogoInitialized() {
    return m_logoInitialized;
}

SportEventDataModel &SportEventDataModel::setLogoInitializationStatus(bool initialized) {
    m_logoInitialized = initialized;
    return *this;
}

SportEventDataModel &SportEventDataModel::setHomeFileName(String homeFileName) {
    if (m_homeFileName != homeFileName) {
        m_homeFileName = homeFileName;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getHomeFileName() {
    return m_homeFileName;
}

SportEventDataModel &SportEventDataModel::setHomeLogoSub(String homeLogo) {
    if (m_homeLogoSub != homeLogo) {
        m_homeLogoSub = homeLogo;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getHomeLogoSub() {
    return m_homeLogoSub;
}

SportEventDataModel &SportEventDataModel::setAwayFileName(String awayFileName) {
    if (m_awayFileName != awayFileName) {
        m_awayFileName = awayFileName;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getAwayFileName() {
    return m_awayFileName;
}

SportEventDataModel &SportEventDataModel::setAwayLogoSub(String awayLogo) {
    if (m_awayLogoSub != awayLogo) {
        m_awayLogoSub = awayLogo;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getAwayLogoSub() {
    return m_awayLogoSub;
}

SportEventDataModel &SportEventDataModel::setHomeAbbrev(String homeAbbrev) {
    if (m_homeAbbrev != homeAbbrev) {
        m_homeAbbrev = homeAbbrev;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getHomeAbbrev() {
    return m_homeAbbrev;
}

SportEventDataModel &SportEventDataModel::setAwayAbbrev(String awayAbbrev) {
    if (m_awayAbbrev != awayAbbrev) {
        m_awayAbbrev = awayAbbrev;
        m_changed = true;
    }
    return *this;
}

String SportEventDataModel::getAwayAbbrev() {
    return m_awayAbbrev;
}
