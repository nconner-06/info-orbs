#ifndef PLANROUTEDATEMODEL_H
#define PLANROUTEDATEMODEL_H

#include <Arduino.h>

class PlaneRouteDataModel {
public:
    PlaneRouteDataModel();

    PlaneRouteDataModel &setCallSign(String callsign);
    String getCallSign();
    PlaneRouteDataModel &setOpCode(String opCode);
    String getOpCode();
    PlaneRouteDataModel &setairlineName(String airlineName);
    String getairlineName();
    PlaneRouteDataModel &setairportOrigin(String airportOrigin);
    String getairportOrigin();
    PlaneRouteDataModel &setairportNameO(String airportNameO);
    String getairportNameO();
    PlaneRouteDataModel &setairportDest(String airportDest);
    String getairportDest();
    PlaneRouteDataModel &setairportNameD(String airportNameD);
    String getairportNameD();
    PlaneRouteDataModel &setType(String type);
    String getType();
    PlaneRouteDataModel &setDesc(String desc);
    String getDesc();
    PlaneRouteDataModel &setLat(float lat);
    float getLat();
    PlaneRouteDataModel &setLong(float lon);
    float getLong();
    PlaneRouteDataModel &setDistSq(float distSq);
    float getDistSq();
    PlaneRouteDataModel &setnoseDeg(float noseDeg);
    float getnoseDeg();
    PlaneRouteDataModel &settrackDeg(float trackDeg);
    float gettrackDeg();
    PlaneRouteDataModel &setgsKnots(float gsKnots);
    float getgsKnots();
    PlaneRouteDataModel &setAlt(int alt);
    int getAlt();
    PlaneRouteDataModel &setdbFlags(int dbFlags);
    int getdbFlags();
    PlaneRouteDataModel &setbaroRateFpm(int baroRateFpm);
    int getbaroRateFpm();
    PlaneRouteDataModel &setonGround(bool onGround);
    bool getonGround();

    void setRouteCount(int index);
    int getRouteCount();

    bool isChanged();
    PlaneRouteDataModel &setChangedStatus(bool changed);
    bool isInitialized();
    PlaneRouteDataModel &setInitializationStatus(bool initialized);

private:
    String m_callsign = "";
    String m_operatorCode = "";
    String m_airlineName = "";
    String m_airportOrigin = "";
    String m_airportNameO = "";
    String m_airportDest = "";
    String m_airportNameD = "";
    String m_type = "";
    String m_desc = "";
    float m_lat;
    float m_long;
    float m_noseDeg;
    float m_trackDeg;
    float m_gsKnots;
    float m_distSq;
    int m_alt = 0;
    int m_dbFlags = 0;
    int m_baroRateFpm = 0;
    bool m_onGround = false;

    int m_routeCount = 0;

    bool m_changed = false;
    bool m_initialized = false;
};

#endif