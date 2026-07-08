#ifndef PLANE_RADAR_WIDGET_H
#define PLANE_RADAR_WIDGET_H

#include "GlobalTime.h"
#include "PlaneRouteDataModel.h"
#include "Translations.h"
#include "Widget.h"
#include "config_helper.h"

#ifndef RADAR1_API_URL
    #define RADAR1_API_URL "https://opendata.adsb.fi/api/v3/lat/"
#endif

#ifndef RADAR2_API_URL
    #define RADAR2_API_URL "https://api.adsbdb.com/v0/callsign/"
#endif

class PlaneRadarWidget : public Widget {
public:
    PlaneRadarWidget(ScreenManager &manager, ConfigManager &config);
    void setup() override;
    void update(bool force) override;
    void draw(bool force) override;
    void onLeave(bool force) override;
    void buttonPressed(uint8_t buttonId, ButtonState state) override;
    String getName() override;
    void processResponse(PlaneRouteDataModel &PlaneRouteData, int httpCode, const String &response);
    void setRoutes(PlaneRouteDataModel *route, int count);
    void sortRoutes(PlaneRouteDataModel *route, int count);
    void absprocessResponse(PlaneRouteDataModel &route, int httpCode, const String &response);

private:
    void drawEmptyDetail(int screenIndex);
    void changeFormat();
    float m_configLat;
    float m_configLon;
    int m_rangeIndex = 0;
    String m_ranges[5] = {"5 km", "10 km", "15 km", "20 km", "25 km"};
    String m_dir[16] = {"N", "NNE", "NE", "NEE", "E", "ESE", "SE", "SSE", "S", "SSW", "SW", "SWW", "W", "WNW", "NW", "NNW"};
    int m_range[5] = {5, 10, 15, 20, 25};
    int m_aircraftCount = 0;
    const float kKmPerDeg = 111.0f;
    bool m_showOnGround = false;
    PlaneRouteDataModel *m_planeRouteData = nullptr;

    WidgetTimer &m_drawTimer;
    WidgetTimer &m_updateTimer;

#ifndef PLANERADAR_UPDATE_DELAY
    #define PLANE_RADAR_UPDATE_DELAY TimeFrequency::FiveSeconds
#endif

#ifndef PLANERADAR_DRAW_DELAY
    #define PLANE_RADAR_DRAW_DELAY TimeFrequency::FiveHundredMilliseconds
#endif
};

#endif // PLANE_RADAR_WIDGET_H