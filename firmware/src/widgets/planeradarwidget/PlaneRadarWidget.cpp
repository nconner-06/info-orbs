#include "PlaneRadarWidget.h"
#include "PlaneRadarTranslations.h"
#include "TaskFactory.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <HTTPClient.h>

PlaneRadarWidget::PlaneRadarWidget(ScreenManager &manager, ConfigManager &config) : Widget(manager, config),
                                                                                    m_drawTimer(addDrawRefreshFrequency(PLANE_RADAR_DRAW_DELAY)),
                                                                                    m_updateTimer(addUpdateRefreshFrequency(PLANE_RADAR_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_PLANE_RADAR == WIDGET_ON);
    m_config.addConfigBool("PlaneRadarWid", "planeEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigFloat("PlaneRadarWid", "planeLat", &m_configLat, t_radarLat);
    m_config.addConfigFloat("PlaneRadarWid", "planeLon", &m_configLon, t_radarLong);
    m_config.addConfigComboBox("PlaneRadarWid", "planeRange", &m_rangeIndex, t_cycleRanges, t_cycleRange);
    m_config.addConfigBool("PlaneRadarWid", "planeOnGnd", &m_showOnGround, t_showOnGround);
}

void PlaneRadarWidget::setup() {
}

void PlaneRadarWidget::update(bool force) {
    String url = String(RADAR1_API_URL) + String(m_configLat, 7) + "/lon/" + String(m_configLon, 7) + "/dist/" + m_range[m_rangeIndex];

    JsonDocument jfilter;
    jfilter["ac"] = true;

    String filter = "";
    filter = jfilter.as<String>();

    auto task = TaskFactory::createHttpGetTask(
        url, filter, [this](int httpCode, const String &response) {
            processResponse(*m_planeRouteData, httpCode, response);
        });

    if (!task) {
        Log.errorln("Failed to create task");
        return;
    }

    bool success = TaskManager::getInstance()->addTask(std::move(task));
    if (!success) {
        Log.errorln("Failed to add task");
        return;
    }
}

void PlaneRadarWidget::processResponse(PlaneRouteDataModel &PlaneRouteData, int httpCode, const String &response) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            JsonArray ac = doc["ac"].as<JsonArray>();
            auto *routeArray = new PlaneRouteDataModel[ac.size() + 1];
            int count = 0;
            for (JsonVariant route : ac) {
                PlaneRouteDataModel l_r = PlaneRouteDataModel();
                l_r.setCallSign(route["flight"].as<String>());
                l_r.setOpCode(route["r"].as<String>());
                l_r.setType(route["t"].as<String>());
                l_r.setDesc(route["desc"].as<String>());
                l_r.setLat(route["lat"].as<float>());
                l_r.setLong(route["lon"].as<float>());
                l_r.setonGround((route["alt_baro"].as<String>() == "ground") ? true : false);
                if (!l_r.getonGround())
                    l_r.setAlt(route["alt_baro"].as<int>());
                l_r.setbaroRateFpm(route["baro_rate"].as<int>());

                if (route["true_heading"].as<float>())
                    l_r.setnoseDeg(route["true_heading"].as<float>());
                else if (route["mag_heading"].as<float>())
                    l_r.setnoseDeg(route["mag_heading"].as<float>());
                else if (route["track"].as<float>())
                    l_r.setnoseDeg(route["track"].as<float>());
                else if (route["dir"].as<float>())
                    l_r.setnoseDeg(route["dir"].as<float>());

                if (route["track"].as<float>())
                    l_r.settrackDeg(route["track"].as<float>());
                else if (route["true_heading"].as<float>())
                    l_r.settrackDeg(route["true_heading"].as<float>());
                else if (route["mag_heading"].as<float>())
                    l_r.settrackDeg(route["mag_heading"].as<float>());
                else if (route["dir"].as<float>())
                    l_r.settrackDeg(route["dir"].as<float>());

                if (route["gs"].as<float>())
                    l_r.setgsKnots(route["gs"].as<float>());
                else if (route["tas"].as<float>())
                    l_r.setgsKnots(route["tas"].as<float>());
                else if (route["ias"].as<float>())
                    l_r.setgsKnots(route["ias"].as<float>());

                float dx_km = float((l_r.getLong() - m_configLon) * kKmPerDeg);
                float dy_km = float((l_r.getLat() - m_configLat) * kKmPerDeg);
                l_r.setDistSq(sqrtf((dy_km * dy_km) + (dx_km * dx_km)));

                routeArray[count++] = l_r;
            }
            setRoutes(routeArray, count);
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed (1): %s", error.c_str());
        }

    } else {
        Log.errorln("HTTP request failed (1), error code: %d\n", httpCode);
    }
}

void PlaneRadarWidget::setRoutes(PlaneRouteDataModel *route, int count) {
    bool lv_found = false;
    Log.infoln("setRoute() count=%d", count);
    if (count > 0) {
        sortRoutes(route, count);

        for (int j = 0; j < count; j++) {
            lv_found = false;
            for (int i = 0; i < m_aircraftCount; i++) {
                PlaneRouteDataModel l_r_old = m_planeRouteData[i];
                if (l_r_old.getCallSign() == route[j].getCallSign()) {
                    route[j].setairlineName(l_r_old.getairlineName());
                    route[j].setairportOrigin(l_r_old.getairportOrigin());
                    route[j].setairportNameO(l_r_old.getairportNameO());
                    route[j].setairportDest(l_r_old.getairportDest());
                    route[j].setairportNameD(l_r_old.getairportNameD());
                    lv_found = true;
                    break;
                }
            }
            if (!lv_found) {
                String url = RADAR2_API_URL + route[j].getCallSign();
                String filter = "";
                PlaneRouteDataModel &m_route = route[j];

                auto task = TaskFactory::createHttpGetTask(
                    url, filter, [this, &m_route](int httpCode, const String &response) {
                        absprocessResponse(m_route, httpCode, response);
                    });

                if (!task) {
                    Log.errorln("Failed to create task");
                    return;
                }

                bool success = TaskManager::getInstance()->addTask(std::move(task));
                if (!success) {
                    Log.errorln("Failed to add task");
                    return;
                }
            }
        }
    }

    delete[] m_planeRouteData;
    m_planeRouteData = route;
    m_aircraftCount = count;
}

void PlaneRadarWidget::absprocessResponse(PlaneRouteDataModel &route, int httpCode, const String &response) {
    if (httpCode > 0) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            JsonObject adsb = doc["response"]["flightroute"].as<JsonObject>();
            if (adsb.size() > 0) {
                route.setairlineName(adsb["airline"]["name"].as<String>());
                route.setairportOrigin(adsb["origin"]["iata_code"].as<String>());
                route.setairportNameO(adsb["origin"]["name"].as<String>());
                route.setairportDest(adsb["destination"]["iata_code"].as<String>());
                route.setairportNameD(adsb["destination"]["name"].as<String>());
            }
        } else {
            // Handle JSON deserialization error
            Log.errorln("Deserialization failed: %s", error.c_str());
        }
    } else {
        Log.errorln("🔴 HTTP request failed, error code: %d", httpCode);
    }
}

void PlaneRadarWidget::sortRoutes(PlaneRouteDataModel *route, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - 1 - i; j++) {
            if (route[j].getDistSq() > route[j + 1].getDistSq()) {
                PlaneRouteDataModel l_r = route[j];
                route[j] = route[j + 1];
                route[j + 1] = l_r;
            }
        }
    }
}

void PlaneRadarWidget::changeFormat() {
    m_rangeIndex++;
    if (m_rangeIndex > 4)
        m_rangeIndex = 0;
    m_config.putConfigInt("planeRange", m_rangeIndex);

    // m_manager.clearAllScreens();
    update(true);
    draw(true);
}

void PlaneRadarWidget::draw(bool force) {
    String lv_range = "";
    m_manager.setFont(DEFAULT_FONT);
    m_manager.setFontColor(TFT_WHITE);

    m_manager.fillSprite(TFT_BLACK);
    // Draw background
    m_manager.drawString("N", 120, 6, 10, Align::MiddleCenter);
    m_manager.drawString("E", 234, 120, 10, Align::MiddleCenter);
    m_manager.drawString("S", 120, 234, 10, Align::MiddleCenter);
    m_manager.drawString("W", 6, 120, 10, Align::MiddleCenter);

    m_manager.drawLine(120, 14, 120, 226, TFT_GREEN);
    m_manager.drawLine(14, 120, 226, 120, TFT_GREEN);

    m_manager.drawArc(120, 120, 105, 106, 0, 360, TFT_GREEN, TFT_BLACK);
    m_manager.drawArc(120, 120, 70, 71, 0, 360, TFT_GREEN, TFT_BLACK);
    m_manager.drawArc(120, 120, 35, 36, 0, 360, TFT_GREEN, TFT_BLACK);

    m_manager.fillCircle(120, 120, 2, TFT_WHITE);

    m_manager.setFontColor(TFT_GREEN, TFT_BLACK);
    m_manager.fillRect(185, 111, 36, 20, TFT_BLACK); // drawRoundRect
    lv_range = " " + m_ranges[m_rangeIndex] + " ";
    m_manager.drawString(lv_range, 203, 120, 8, Align::MiddleCenter);

    float pxPerKm = float(106.0) / m_range[m_rangeIndex];

    for (int i = 0; i < m_aircraftCount; i++) {
        PlaneRouteDataModel lv_plane = m_planeRouteData[i];
        float dx_km = float((lv_plane.getLong() - m_configLon) * kKmPerDeg);
        float dy_km = float((lv_plane.getLat() - m_configLat) * kKmPerDeg);
        int out_x = 120 + int(lroundf(dx_km * pxPerKm));
        int out_y = 120 - int(lroundf(dy_km * pxPerKm));
        const float rad = lv_plane.getnoseDeg() * DEG_TO_RAD;
        const float sinH = sinf(rad);
        const float cosH = cosf(rad);
        const int baseX = out_x - int(lroundf(sinH * 12));
        const int baseY = out_y + int(lroundf(cosH * 12));
        const int wingX = int(lroundf(cosH * 4));
        const int wingY = int(lroundf(sinH * 4));

        if (!lv_plane.getonGround()) {
            if (lv_plane.getDistSq() < m_range[m_rangeIndex]) {
                uint32_t lv_col = TFT_WHITE;
                if (lv_plane.getbaroRateFpm() < -150)
                    lv_col = TFT_RED;
                if (lv_plane.getbaroRateFpm() > 150)
                    lv_col = TFT_GREEN;
                m_manager.fillTriangle(out_x, out_y, baseX + wingX, baseY + wingY, baseX - wingX, baseY - wingY, lv_col);
                //  m_manager.fillCircle(out_x, out_y, 2, lv_col);
#ifdef PR_DEBUG
                Log.infoln("In Range : %i, %F, %s, %i, %i, %F, %F", i, lv_plane.getnoseDeg(), lv_plane.getCallSign(), out_x, out_y, lv_plane.getLong(), lv_plane.getLat());
#endif
                m_manager.setFontColor(TFT_WHITE);
                m_manager.drawString(lv_plane.getCallSign(), out_x, (out_y - 4), 8, Align::BottomLeft);
            } else {
                const float angle_rad = atan2f(dx_km, dy_km);
                int out_x = 120 + int(lroundf(sinf(angle_rad) * 118));
                int out_y = 120 - int(lroundf(cosf(angle_rad) * 118));
                m_manager.fillCircle(out_x, out_y, 4, TFT_RED);
#ifdef PR_DEBUG
                Log.infoln("Out Range : %i, %F, %s, %i, %i, %F, %F", i, lv_plane.getnoseDeg(), lv_plane.getCallSign(), out_x, out_y, lv_plane.getLong(), lv_plane.getLat());
#endif
            }
        } else {
#ifdef PR_DEBUG
            Log.infoln("On Ground : %i, %F, %s, %i, %i, %F, %F", i, lv_plane.getnoseDeg(), lv_plane.getCallSign(), out_x, out_y, lv_plane.getLong(), lv_plane.getLat());
#endif
        }
    }
    m_manager.pushSprite(0, 0, 0);

    m_manager.setFontColor(TFT_WHITE);
    int lv_idx = 0;
    PlaneRouteDataModel lv_plane;
    for (int i = 1; i <= 4; i++) {
        m_manager.fillSprite(TFT_BLACK);
        if (i < m_aircraftCount + 1) {
            if (lv_idx < m_aircraftCount) {
                if (m_showOnGround) {
                    lv_plane = m_planeRouteData[lv_idx];
                } else {
                    do {
                        lv_plane = m_planeRouteData[lv_idx];
                        if (!lv_plane.getonGround())
                            break;
                        lv_idx++;
                        if (lv_idx == m_aircraftCount)
                            break;
                    } while (true);
                }
                lv_idx++;
                if (lv_idx < m_aircraftCount && lv_plane.getDistSq() < m_range[m_rangeIndex]) {
                    int lv_dir = ((int(lv_plane.getnoseDeg() + 11.25) % 360) / 22.5);

                    m_manager.drawString(lv_plane.getCallSign(), 120, 30, 20, Align::MiddleCenter);
                    if (lv_plane.getonGround())
                        m_manager.drawString("On Ground", 120, 65, 16, Align::MiddleCenter);
                    else {
                        m_manager.drawString(lv_plane.getairlineName(), 120, 60, 16, Align::MiddleCenter);
                        m_manager.drawString(lv_plane.getairportNameO(), 120, 90, 14, Align::MiddleCenter);
                        m_manager.drawString(lv_plane.getairportNameD(), 120, 120, 14, Align::MiddleCenter);

                        m_manager.drawString("Speed : " + String(lv_plane.getgsKnots()), 75, 150, 12, Align::MiddleCenter);
                        m_manager.drawString("Dir : " + m_dir[lv_dir], 180, 150, 12, Align::MiddleCenter);
                        m_manager.drawString("Alt : " + String(lv_plane.getAlt()), 120, 180, 14, Align::MiddleCenter);
                    }
#ifdef PR_DEBUG
                    Log.infoln("Info : %s, %s", lv_plane.getCallSign().c_str(), lv_plane.getairlineName().c_str());
#endif
                } else
                    drawEmptyDetail(i);
            } else
                drawEmptyDetail(i);
        } else
            drawEmptyDetail(i);
        m_manager.pushSprite(i, 0, 0);
    }
}

void PlaneRadarWidget::drawEmptyDetail(int screenIndex) {
    m_manager.drawString("No Flight Details", 120, 120, 18, Align::MiddleCenter);
}

void PlaneRadarWidget::onLeave(bool force) {
}

void PlaneRadarWidget ::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT) {
        update(true);
    }
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM) {
        changeFormat();
    }
}

String PlaneRadarWidget ::getName() {
    return "Plane Radar";
}
