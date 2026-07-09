// TODO:
// 1
// factor out a selectDisplay that selects the dispay and returns the workable object,
// so we don't have this select, getDisplay crap all over the place
// 2
// make high/low an enum, if we even keep it (I strongly suggest just switching back
// and forth between high and low every 10 seconds or something)
// 3
// factor out the text wrapping (there's a utils for that already, if that doesn't work, why not?)

#include "WeatherWidget.h"
#include "WeatherTranslations.h"
#include "feeds/OpenWeatherMapFeed.h"
#include "feeds/TempestFeed.h"
#include "feeds/VisualCrossingFeed.h"
#include "icons.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>

WeatherWidget::WeatherWidget(ScreenManager &manager, ConfigManager &config)
    : Widget(manager, config),
      m_drawTimer(addDrawRefreshFrequency(WEATHER_DRAW_DELAY)),
      m_updateTimer(addUpdateRefreshFrequency(WEATHER_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_WEATHER == WIDGET_ON);
    m_time = GlobalTime::getInstance();
    m_config.addConfigBool("WeatherWidget", "weatherEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigBool("WeatherWidget", "weatherDefault", &m_default, t_defaultWidget);
    weatherFeed = createWeatherFeed();
    weatherFeed->setupConfig(config); // allow feed to add its own config
    m_config.addConfigBool("WeatherWidget", "showSecondTickW", &m_showSecondTickW, t_ShowSecondTick, false);
    m_config.addConfigComboBox("WeatherWidget", "weatherUnits", &m_weatherUnits, t_temperatureUnits, t_temperatureUnit, true);
    m_config.addConfigComboBox("WeatherWidget", "weatherScrMode", &m_screenMode, t_screenModes, t_screenMode, true);
    m_config.addConfigInt("WeatherWidget", "weatherCycleHL", &m_switchinterval, t_weatherCycleHL, true);
    //    Log.infoln("WeatherWidget initialized, mode=%d", m_screenMode);
    m_mode = MODE_HIGHS;
}

WeatherWidget::~WeatherWidget() {
    delete weatherFeed;
}

WeatherFeed *WeatherWidget::createWeatherFeed() {
    int weatherUnits = m_config.getConfigInt("weatherUnits", m_weatherUnits);

#if WEATHER_OPENWEATHERMAP_FEED
    return new OpenWeatherMapFeed(WEATHER_OPENWEATHERMAP_API_KEY, weatherUnits);
#elif WEATHER_TEMPEST_FEED
    return new TempestFeed(WEATHER_TEMPEST_API_KEY, weatherUnits);
#elif WEATHER_VISUALCROSSING_FEED
    return new VisualCrossingFeed(WEATHER_VISUALCROSSING_API_KEY, weatherUnits);
#endif
}

void WeatherWidget::changeMode() {
    m_prevMillisSwitch = millis();
    m_mode = (m_mode == MODE_HIGHS) ? MODE_LOWS : MODE_HIGHS;
    threeDayWeather(4);
}

void WeatherWidget::onLeave(bool force) {
}

void WeatherWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        changeMode();
    if (buttonId == BUTTON_OK && state == BTN_MEDIUM)
        update(true);
    if (buttonId == BUTTON_OK && state == BTN_LONG)
        m_digitalClock = !m_digitalClock;
}

void WeatherWidget::setup() {
    if (isItTimeToUpdate())
        update(true);
    configureColors();
    m_prevMillisSwitch = millis();
}

void WeatherWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);
    displayClock(0);

    if (force || model.isChanged()) {
        weatherText(1);

        m_manager.fillSprite(m_backgroundColor);
        drawWeatherIcon(2, model.getCurrentIcon(), 0, 0, 1);
        m_manager.pushSprite(2, 0, 0);

        singleWeatherDeg(3);

        m_manager.fillSprite(m_backgroundColor);
        threeDayWeather(4);

        model.setChangedStatus(false);
        if (force) {
            resetTimer(m_drawTimer); // Reset only on forced draw
        }
    }

    if ((millis() - m_prevMillisSwitch >= m_switchinterval * 1000) && m_switchinterval > 0) {
        changeMode();
    }
}

void WeatherWidget::update(bool force) {
    if (force) {
        int retry = 0;
        while (!weatherFeed->getWeatherData(model) && retry++ < MAX_RETRIES)
            ;
        resetTimer(m_updateTimer); // Reset timer after forced update
    } else {
        weatherFeed->getWeatherData(model);
    }
}

void WeatherWidget::displayClock(int displayIndex) {
    const int dateY = 50;
    const int clockY = 110;
    const int dayOfWeekY = 180;

    m_manager.fillSprite(m_backgroundColor);
    m_manager.setFontColor(m_foregroundColor);

    if (m_digitalClock) {
        m_manager.drawCentreString(m_time->getDayAndMonth(), centre, dateY, 18);
        const String weekDay = m_time->getWeekday();
        m_manager.drawCentreString(weekDay, centre, dayOfWeekY, 22);

        m_manager.drawString(m_time->getTime(), centre, clockY, 66, Align::MiddleCenter);

        if (m_showSecondTickW) {
            int lv_second = m_time->getSecond();
            int startA = ((lv_second * 6) + 180 - 3) % 360;
            int endA = ((lv_second * 6) + 180 + 3) % 360;
            m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 115, startA, endA, m_foregroundColor, m_backgroundColor);
        }
    } else {
        float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // Saved H, M, S x & y multipliers
        float sdeg = 0, mdeg = 0, hdeg = 0;
        uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
        uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;

        if (m_screenMode == Light)
            m_manager.drawSmoothArc(120, 120, 120, 115, 0, 360, TFT_RED, m_backgroundColor);
        else
            m_manager.drawSmoothArc(120, 120, 120, 115, 0, 360, TFT_GREENYELLOW, m_backgroundColor);

        // Draw 12 lines
        for (int i = 0; i < 360; i += 30) {
            sx = cos((i - 90) * 0.0174532925);
            sy = sin((i - 90) * 0.0174532925);
            x0 = sx * 115 + 120;
            yy0 = sy * 115 + 120;
            x1 = sx * 105 + 120;
            yy1 = sy * 105 + 120;
            if (m_screenMode == Light)
                m_manager.drawLine(x0, yy0, x1, yy1, TFT_RED);
            else
                m_manager.drawLine(x0, yy0, x1, yy1, TFT_GREENYELLOW);
        }

        // Draw 60 dots
        for (int i = 0; i < 360; i += 6) {
            sx = cos((i - 90) * 0.0174532925);
            sy = sin((i - 90) * 0.0174532925);
            x0 = sx * 107 + 120;
            yy0 = sy * 107 + 120;
            if (i % 90 == 0)
                // Draw main quadrant dots
                m_manager.fillCircle(x0, yy0, 3, m_foregroundColor);
            else
                // Draw minute markers
                if (i % 30 != 0)
                    m_manager.drawPixel(x0, yy0, m_foregroundColor);
        }

        // Pre-compute hand degrees, x & y coords for a fast screen update
        sdeg = m_time->getSecond() * 6; // 0-59 -> 0-354
        mdeg = m_time->getMinute() * 6 + sdeg * 0.01666667; // 0-59 -> 0-360 - includes seconds
        hdeg = m_time->getHour() * 30 + mdeg * 0.0833333; // 0-11 -> 0-360 - includes minutes and seconds
        hx = cos((hdeg - 90) * 0.0174532925);
        hy = sin((hdeg - 90) * 0.0174532925);
        mx = cos((mdeg - 90) * 0.0174532925);
        my = sin((mdeg - 90) * 0.0174532925);
        sx = cos((sdeg - 90) * 0.0174532925);
        sy = sin((sdeg - 90) * 0.0174532925);

        ohx = hx * 62 + 121;
        ohy = hy * 62 + 121;
        omx = mx * 84 + 121;
        omy = my * 84 + 121;
        osx = sx * 90 + 121;
        osy = sy * 90 + 121;
        m_manager.drawLine(ohx, ohy, 120, 121, m_foregroundColor);
        m_manager.drawLine(omx, omy, 120, 121, m_foregroundColor);
        m_manager.drawLine(osx, osy, 120, 121, TFT_RED);

        m_manager.fillCircle(120, 121, 3, TFT_RED);
    }

    m_manager.pushSprite(displayIndex, 0, 0);
}

// Write an image to the screen from a hex array.
// scale of the image (1=full size, then multiples of 2 to scale down)
// getting the byte array size is very annoying as it's computed on compile, so you can't do it dynamically.
void WeatherWidget::showJPG(int displayIndex, int x, int y, const byte jpgData[], int jpgDataSize, int scale) {
    TJpgDec.setJpgScale(scale);
    uint16_t w = 0, h = 0;
    TJpgDec.getJpgSize(&w, &h, jpgData, jpgDataSize);
    TJpgDec.drawJpg(x, y, jpgData, jpgDataSize);
}

// Take the text output from the weather API and map it to a icon/byte array, then display it
void WeatherWidget::drawWeatherIcon(int displayIndex, const String &condition, int x, int y, int scale) {
    const byte *iconStart = NULL;
    const byte *iconEnd = NULL;

    if (condition == "partly-cloudy-night") {
        iconStart = m_screenMode == Light ? moonCloudW_start : moonCloudB_start;
        iconEnd = m_screenMode == Light ? moonCloudW_end : moonCloudB_end;
    } else if (condition == "partly-cloudy-day") {
        iconStart = m_screenMode == Light ? sunCloudsW_start : sunCloudsB_start;
        iconEnd = m_screenMode == Light ? sunCloudsW_end : sunCloudsB_end;
    } else if (condition == "clear-day") {
        iconStart = m_screenMode == Light ? sunW_start : sunB_start;
        iconEnd = m_screenMode == Light ? sunW_end : sunB_end;
    } else if (condition == "clear-night") {
        iconStart = m_screenMode == Light ? moonW_start : moonB_start;
        iconEnd = m_screenMode == Light ? moonW_end : moonB_end;
    } else if (condition == "snow") {
        iconStart = m_screenMode == Light ? snowW_start : snowB_start;
        iconEnd = m_screenMode == Light ? snowW_end : snowB_end;
    } else if (condition == "rain") {
        iconStart = m_screenMode == Light ? rainW_start : rainB_start;
        iconEnd = m_screenMode == Light ? rainW_end : rainB_end;
    } else if (condition == "fog" || condition == "wind" || condition == "cloudy") {
        iconStart = m_screenMode == Light ? cloudsW_start : cloudsB_start;
        iconEnd = m_screenMode == Light ? cloudsW_end : cloudsB_end;
    } else {
        Log.warningln("Unknown weather icon: %s", condition.c_str());
    }

    const int size = iconEnd - iconStart;
    if (iconStart != NULL && size > 0) {
        showJPG(displayIndex, x, y, iconStart, size, scale);
    }
}

// Displays the current temperature on a single screen.
// doesn't round deg, just removes all text after the decimal
void WeatherWidget::singleWeatherDeg(int displayIndex) {
    m_manager.fillSprite(m_backgroundColor);
    m_manager.drawCentreString(model.getCurrentTemperature(0), centre, 90, 88);

    // No glaring white chunks in Dark mode
    if (m_screenMode == Light) {
        m_manager.fillRect(0, 150, 240, 90, m_foregroundColor);
        m_manager.fillRect(centre - 1, 150, 2, 90, m_backgroundColor);
    }

    int fontSize = 22;
    m_manager.setFontColor(m_invertedForegroundColor);
    m_manager.setBackgroundColor(m_invertedBackgroundColor);
    m_manager.drawCentreString("High", 75, 170, fontSize);
    m_manager.drawCentreString("Low", 160, 170, fontSize);
    m_manager.drawCentreString(model.getTodayHigh(0), 80, 210, fontSize);
    m_manager.drawCentreString(model.getTodayLow(0), 160, 210, fontSize);
    m_manager.setFontColor(m_foregroundColor);
    m_manager.setBackgroundColor(m_backgroundColor);
    m_manager.pushSprite(displayIndex, 0, 0);
}

// Display the user's current city and the text description of the weather
void WeatherWidget::weatherText(int displayIndex) {

    //=== TEXT OVERFLOW ============================
    // This takes a given string a and breaks it down in max x character long strings ensuring not to break it only at a space.
    // Given the small width of the screens this will porbablly be needed to this project again so making sure to outline it
    // clearly as this should liekly eventually be turned into a fucntion. Before use the array size should be made to be dynamic.
    // In this case its used for the weather text description

    String message = model.getCurrentText() + " ";
    String messageArr[4];
    int variableRangeS = 0;
    int variableRangeE = 24;
    for (int i = 0; i < 4; i++) {
        while (message.substring(variableRangeE - 1, variableRangeE) != " ") {
            variableRangeE--;
        }
        messageArr[i] = message.substring(variableRangeS, variableRangeE);
        variableRangeS = variableRangeE;
        variableRangeE = variableRangeS + 24;
    }
    //=== OVERFLOW END ==============================

    m_manager.fillSprite(m_backgroundColor);
    String cityName = model.getCityName();
    cityName.remove(cityName.indexOf(",", 0));

    m_manager.setFontColor(m_foregroundColor);
    m_manager.drawFittedString(cityName, centre, 70, 195, 50, Align::MiddleCenter);

    auto y = 118;
    for (auto i = 0; i < 4; i++) {
        m_manager.drawCentreString(messageArr[i], centre, y, 15);
        y += 25;
    }
    m_manager.pushSprite(displayIndex, 0, 0);
}

// Displays the next 3 days' weather forecast
void WeatherWidget::threeDayWeather(int displayIndex) {
    const int days = 3;
    const int columnSize = 75;
    const int highLowY = 210;
    const int fontSize = 22;

    m_manager.fillSprite(m_backgroundColor);

    // No glaring white chunks in Dark mode
    if (m_screenMode == Light) {
        m_manager.fillRect(0, 180, 240, 70, m_foregroundColor);
        m_manager.setFontColor(m_backgroundColor);
        m_manager.setBackgroundColor(m_foregroundColor);
    }
    m_manager.drawString(m_mode == MODE_HIGHS ? "Highs" : "Lows", centre, highLowY, fontSize, Align::MiddleCenter);
    // Reset colors
    m_manager.setFontColor(m_foregroundColor);
    m_manager.setBackgroundColor(m_backgroundColor);

    int temperatureFontSize = fontSize; // 0-9 only
    // Look up all the temperatures, and if any of them are more than 2 digits, we need
    // to scale down the font -- or it won't look right on the screen.
    String temps[days];
    for (auto i = 0; i < days; i++) {
        temps[i] = m_mode == MODE_HIGHS ? model.getDayHigh(i, 0) : model.getDayLow(i, 0);
        if (temps[i].length() > 4) {
            // We've got a nutty 3-digit temperature (plus degree sign), scale down
            temperatureFontSize = fontSize - 4; // smaller
        }
    }

    m_manager.setFontColor(m_foregroundColor);
    int lv_day = 0;
    for (auto i = 0; i < days; i++) {
        // TODO: only works for 3 days
        const int x = (centre - columnSize) + i * columnSize;

        drawWeatherIcon(displayIndex, model.getDayIcon(i), x - 30, 40, 4);
        m_manager.drawCentreString(temps[i], x, 120, temperatureFontSize);

        lv_day = m_time->getDayOfWeek() + 1 + i;
        if (lv_day > 6)
            lv_day = lv_day - 7;
        String shortDayName = i18n(t_weekdays, lv_day);
        shortDayName.remove(3);
        m_manager.drawString(shortDayName, x, 160, fontSize, Align::MiddleCenter);
    }
    m_manager.pushSprite(displayIndex, 0, 0);
}

void WeatherWidget::configureColors() {
    m_foregroundColor = m_screenMode == Light ? TFT_BLACK : TFT_WHITE;
    m_backgroundColor = m_screenMode == Light ? TFT_WHITE : TFT_BLACK;

    // NOTE: In Light mode, we draw decorative black chunks and display the high and low on them in white.
    //       It does not make sense to have glaring white chunks in dark mode, so we don't draw them at all,
    //       and display the high and low in white too.
    m_invertedForegroundColor = m_screenMode == Light ? m_backgroundColor : m_foregroundColor;
    m_invertedBackgroundColor = m_screenMode == Light ? m_foregroundColor : m_backgroundColor;

    m_manager.setBackgroundColor(m_backgroundColor);
}

String WeatherWidget::getName() {
    return "Weather";
}
