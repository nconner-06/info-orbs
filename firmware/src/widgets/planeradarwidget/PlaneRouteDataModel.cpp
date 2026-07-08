#include "PlaneRouteDataModel.h"
#include <ArduinoLog.h>

PlaneRouteDataModel::PlaneRouteDataModel() {
}

PlaneRouteDataModel &PlaneRouteDataModel::setCallSign(String callsign) {
    if (m_callsign != callsign) {
        m_callsign = callsign;
        m_callsign.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getCallSign() {
    return m_callsign;
}

PlaneRouteDataModel &PlaneRouteDataModel::setOpCode(String opCode) {
    if (m_operatorCode != opCode) {
        m_operatorCode = opCode;
        m_operatorCode.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getOpCode() {
    return m_operatorCode;
}

PlaneRouteDataModel &PlaneRouteDataModel::setairlineName(String airlineName) {
    if (m_airlineName != airlineName) {
        m_airlineName = airlineName;
        m_airlineName.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getairlineName() {
    return m_airlineName;
}

PlaneRouteDataModel &PlaneRouteDataModel::setairportOrigin(String airportOrigin) {
    if (m_airportOrigin != airportOrigin) {
        m_airportOrigin = airportOrigin;
        m_airportOrigin.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getairportOrigin() {
    return m_airportOrigin;
}

PlaneRouteDataModel &PlaneRouteDataModel::setairportNameO(String airportNameO) {
    if (m_airportNameO != airportNameO) {
        m_airportNameO = airportNameO;
        m_airportNameO.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getairportNameO() {
    return m_airportNameO;
}

PlaneRouteDataModel &PlaneRouteDataModel::setairportDest(String airportDest) {
    if (m_airportDest != airportDest) {
        m_airportDest = airportDest;
        m_airportDest.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getairportDest() {
    return m_airportDest;
}

PlaneRouteDataModel &PlaneRouteDataModel::setairportNameD(String airportNameD) {
    if (m_airportNameD != airportNameD) {
        m_airportNameD = airportNameD;
        m_airportNameD.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getairportNameD() {
    return m_airportNameD;
}

PlaneRouteDataModel &PlaneRouteDataModel::setType(String type) {
    if (m_type != type) {
        m_type = type;
        m_type.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getType() {
    return m_type;
}
PlaneRouteDataModel &PlaneRouteDataModel::setDesc(String desc) {
    if (m_desc != desc) {
        m_desc = desc;
        m_desc.trim();
        m_changed = true;
    }
    return *this;
}
String PlaneRouteDataModel::getDesc() {
    return m_desc;
}

PlaneRouteDataModel &PlaneRouteDataModel::setLat(float lat) {
    if (m_lat != lat) {
        m_lat = lat;
        m_changed = true;
    }
    return *this;
}
float PlaneRouteDataModel::getLat() {
    return m_lat;
}
PlaneRouteDataModel &PlaneRouteDataModel::setLong(float lon) {
    if (m_long != lon) {
        m_long = lon;
        m_changed = true;
    }
    return *this;
}
float PlaneRouteDataModel::getLong() {
    return m_long;
}
PlaneRouteDataModel &PlaneRouteDataModel::setDistSq(float distSq) {
    if (m_distSq != distSq) {
        m_distSq = distSq;
        m_changed = true;
    }
    return *this;
}
float PlaneRouteDataModel::getDistSq() {
    return m_distSq;
}

PlaneRouteDataModel &PlaneRouteDataModel::setnoseDeg(float noseDeg) {
    if (m_noseDeg != noseDeg) {
        m_noseDeg = noseDeg;
        m_changed = true;
    }
    return *this;
}
float PlaneRouteDataModel::getnoseDeg() {
    return m_noseDeg;
}
PlaneRouteDataModel &PlaneRouteDataModel::settrackDeg(float trackDeg) {
    if (m_trackDeg != trackDeg) {
        m_trackDeg = trackDeg;
        m_changed = true;
    }
    return *this;
}
float PlaneRouteDataModel::gettrackDeg() {
    return m_trackDeg;
}
PlaneRouteDataModel &PlaneRouteDataModel::setgsKnots(float gsKnots) {
    if (m_gsKnots != gsKnots) {
        m_gsKnots = gsKnots;
        m_changed = true;
    }
    return *this;
}
float PlaneRouteDataModel::getgsKnots() {
    return m_gsKnots;
}

PlaneRouteDataModel &PlaneRouteDataModel::setAlt(int alt) {
    if (m_alt != alt) {
        m_alt = alt;
        m_changed = true;
    }
    return *this;
}
int PlaneRouteDataModel::getAlt() {
    return m_alt;
}

PlaneRouteDataModel &PlaneRouteDataModel::setdbFlags(int dbFlags) {
    if (m_dbFlags != dbFlags) {
        m_dbFlags = dbFlags;
        m_changed = true;
    }
    return *this;
}
int PlaneRouteDataModel::getdbFlags() {
    return m_dbFlags;
}

PlaneRouteDataModel &PlaneRouteDataModel::setbaroRateFpm(int baroRateFpm) {
    if (m_baroRateFpm != baroRateFpm) {
        m_baroRateFpm = baroRateFpm;
        m_changed = true;
    }
    return *this;
}
int PlaneRouteDataModel::getbaroRateFpm() {
    return m_baroRateFpm;
}

PlaneRouteDataModel &PlaneRouteDataModel::setonGround(bool onGround) {
    if (m_onGround != onGround) {
        m_onGround = onGround;
        m_changed = true;
    }
    return *this;
}
bool PlaneRouteDataModel::getonGround() {
    return m_onGround;
}

void PlaneRouteDataModel::setRouteCount(int index) {
    if (m_routeCount != index) {
        m_routeCount = index;
        m_changed = true;
    }
}
int PlaneRouteDataModel::getRouteCount() {
    return m_routeCount;
}

bool PlaneRouteDataModel::isChanged() {
    return m_changed;
}
PlaneRouteDataModel &PlaneRouteDataModel::setChangedStatus(bool changed) {
    m_changed = changed;
    return *this;
}

bool PlaneRouteDataModel::isInitialized() {
    return m_initialized;
}
PlaneRouteDataModel &PlaneRouteDataModel::setInitializationStatus(bool initialized) {
    m_initialized = initialized;
    return *this;
}
