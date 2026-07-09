#include "StockWidget.h"
#include "StockTranslations.h"
#include "TaskFactory.h"
#include <ArduinoJson.h>
#include <ArduinoLog.h>
#include <iomanip>

StockWidget::StockWidget(ScreenManager &manager, ConfigManager &config)
    : Widget(manager, config),
      m_drawTimer(addDrawRefreshFrequency(STOCK_DRAW_DELAY)),
      m_updateTimer(addUpdateRefreshFrequency(STOCK_UPDATE_DELAY)) {
    m_enabled = (INCLUDE_STOCK == WIDGET_ON);

    m_time = GlobalTime::getInstance();

    m_config.addConfigBool("StockWidget", "stocksEnabled", &m_enabled, t_enableWidget);
    m_config.addConfigBool("StockWidget", "stockDefault", &m_default, t_defaultWidget);
    m_config.addConfigString("StockWidget", "stockList", &m_stockList, 200, t_stockList);
    char stockList[m_stockList.size()];
    strcpy(stockList, m_stockList.c_str());

    m_config.addConfigComboBox("StockWidget", "stockchgFmt", &m_stockchangeformat, t_stockChangeFormats, t_stockChangeFormat, true);
    m_config.addConfigInt("StockWidget", "stockPaginate", &m_switchinterval, t_stockSwitchInterval, true);
    m_config.addConfigBool("StockWidget", "showClockS", &m_showClockS, t_showClockS, true);
    m_config.addConfigBool("StockWidget", "showSecondTickS", &m_showSecondTickS, t_showSecondTickS, true);

    char *symbol = strtok(stockList, ",");
    m_stockCount = 0;
    do {
        if (m_stockCount >= MAX_STOCKS) {
            Log.warningln("MAX STOCKS UNABLE TO ADD MORE");
            break;
        }
        StockDataModel stockModel = StockDataModel();
        stockModel.setSymbol(String(symbol));
        m_stocks[m_stockCount] = stockModel;
        m_stockCount++;
    } while (symbol = strtok(nullptr, ","));
    m_pageCount = 1 + ((m_stockCount - 1) / (m_showClockS ? 4 : 5)); // int division round up
#ifdef STOCK_DEBUG
    Log.infoln("StockWidget initialized");
    Log.infoln("StockWidget Pages: %d across %d symbools.", m_pageCount, m_stockCount);
#endif
}

void StockWidget::setup() {
    if (m_stockCount == 0) {
        Log.warningln("No stock tickers available");
        return;
    }
    m_prevMillisSwitch = millis();
    m_page = 0;
    if (isItTimeToUpdate())
        update(true);
}

void StockWidget::draw(bool force) {
    m_manager.setFont(DEFAULT_FONT);
    int8_t stockDisplays = 5;
    int8_t startDisplay = 0;

    if (m_showClockS) {
        displayClock(0);
        stockDisplays--;
        startDisplay++;
    }

    for (int8_t i = 0; i < stockDisplays; i++) {
        m_manager.fillSprite(TFT_BLACK);
        int8_t stockIdx = i + (m_page * (m_showClockS ? 4 : 5));
        int8_t displayIndex = startDisplay + i;
        if ((m_stocks[stockIdx].isChanged() || force) && !m_stocks[stockIdx].getStatus() == STOCK_EMTPY) {
#ifdef STOCK_DEBUG
            Log.infoln("StockWidget::draw - %s", m_stocks[i].getSymbol().c_str());
#endif
            displayStock(displayIndex, m_stocks[stockIdx]);
            m_stocks[stockIdx].setChangedStatus(false);
            m_manager.pushSprite(displayIndex, 0, 0);
        } else if (force) {
            m_manager.pushSprite(displayIndex, 0, 0);
        }
    }

    if ((millis() - m_prevMillisSwitch >= (m_switchinterval * 1000)) && m_switchinterval > 0) {
        nextPage();
    }
}

void StockWidget::update(bool force) {

    // Queue requests for each stock
    for (int8_t i = 0; i < m_stockCount; i++) {
#ifdef STOCK_DEBUG
        Log.infoln("StockWidget::update - %s", m_stocks[i].getSymbol().c_str());
#endif
        String url = String(STOCK_API_URL) + "?apikey=" + String(STOCK_API_KEY) + "&symbol=" + m_stocks[i].getSymbol();
        StockDataModel &stock = m_stocks[i];
        String filter = "";

        auto task = TaskFactory::createHttpGetTask(url, filter, [this, &stock](int httpCode, const String &response) {
            processResponse(stock, httpCode, response);
        });

        TaskManager::getInstance()->addTask(std::move(task));
    }
}

void StockWidget::processResponse(StockDataModel &stock, int httpCode, const String &response) {
    // https://twelvedata.com/docs#errors
    if (httpCode == 200) {
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, response);

        if (!error) {
            if (doc["code"] == 404) {
                Log.warningln("The specified data can not be found: %s", stock.getSymbol().c_str());
                stock.setStatus(STOCK_INVALID);
                return;
            } else {
                stock.setCurrentPrice(doc["close"].as<float>())
                    .setPercentChange(doc["percent_change"].as<float>() / 100)
                    .setPriceChange(doc["change"].as<float>())
                    .setHighPrice(doc["fifty_two_week"]["high"].as<float>())
                    .setLowPrice(doc["fifty_two_week"]["low"].as<float>())
                    .setCompany(doc["name"].as<String>())
                    .setTicker(doc["symbol"].as<String>())
                    .setCurrencySymbol(doc["currency"].as<String>())
                    .setStatus(STOCK_READY);
            }
        } else {
            Log.errorln("deserializeJson() failed");
            stock.setStatus(STOCK_ERROR);
        }
    } else {
        Log.errorln("HTTP request failed, error: %d\n", httpCode);
        stock.setStatus(STOCK_ERROR);
    }
}

void StockWidget::changeMode() {
    update(true);
}

void StockWidget::onLeave(bool force) {
}

void StockWidget::buttonPressed(uint8_t buttonId, ButtonState state) {
    if (buttonId == BUTTON_OK && state == BTN_SHORT)
        nextPage();
    else if (buttonId == BUTTON_OK && state == BTN_MEDIUM)
        changeMode();
    if (buttonId == BUTTON_OK && state == BTN_LONG)
        m_digitalClock = !m_digitalClock;
}

void StockWidget::displayStock(int8_t displayIndex, StockDataModel &stock) {
#ifdef STOCK_DEBUG
    Log.infoln("displayStock - %s ~ %s", stock.getSymbol().c_str(), stock.getCurrentPrice(2).c_str());
#endif
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    switch (stock.getStatus()) {
    case STOCK_UNINITIALIZED:
        m_manager.setFontColor(TFT_BLUE);
        m_manager.drawCentreString(i18nStr(t_loadingData), ScreenCenterX, ScreenCenterY - 30, 16);
        m_manager.drawCentreString(stock.getSymbol(), ScreenCenterX, ScreenCenterY, 16);
        break;

    case STOCK_INVALID:
        m_manager.setFontColor(TFT_RED);
        m_manager.drawCentreString(i18nStr(t_invalid), ScreenCenterX, ScreenCenterY - 30, 16);
        m_manager.drawCentreString(stock.getSymbol(), ScreenCenterX, ScreenCenterY, 16);
        break;

    case STOCK_ERROR:
        m_manager.setFontColor(TFT_RED);
        m_manager.drawCentreString(i18nStr(t_error), ScreenCenterX, ScreenCenterY - 30, 16);
        m_manager.drawCentreString(stock.getSymbol(), ScreenCenterX, ScreenCenterY, 16);
        if (not stock.getCurrentPrice() > 0.0) {
            // Only break if we don't have data.  better to display something if we have it
            break;
        }

    case STOCK_READY:
        // Calculate center positions
        int screenWidth = SCREEN_SIZE;
        int centre = 120;
        int arrowOffsetX = 0;
        int arrowOffsetY = -109;

        // Outputs
        m_manager.fillRect(0, 70, screenWidth, 49, TFT_WHITE);
        m_manager.fillRect(0, 111, screenWidth, 20, TFT_LIGHTGREY);
        int smallFontSize = 11;
        int bigFontSize = 29;
        m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
        m_manager.drawCentreString(i18n(t_stock52week), centre, 185, smallFontSize);
        m_manager.drawCentreString(i18nStr(t_highShort) + ": " + stock.getCurrencySymbol() + stock.getHighPrice(), centre, 200, smallFontSize);
        m_manager.drawCentreString(i18nStr(t_lowShort) + ": " + stock.getCurrencySymbol() + stock.getLowPrice(), centre, 215, smallFontSize);
        m_manager.setFontColor(TFT_BLACK, TFT_LIGHTGREY);
        m_manager.drawString(stock.getCompany(), centre, 121, smallFontSize, Align::MiddleCenter);
        if (stock.getPercentChange() < 0.0) {
            m_manager.setFontColor(TFT_RED, TFT_BLACK);
            m_manager.fillTriangle(110 + arrowOffsetX, 120 + arrowOffsetY, 130 + arrowOffsetX, 120 + arrowOffsetY, 120 + arrowOffsetX, 132 + arrowOffsetY, TFT_RED);
            m_manager.drawArc(centre, centre, 120, 118, 0, 360, TFT_RED, TFT_RED);
        } else {
            m_manager.setFontColor(TFT_GREEN, TFT_BLACK);
            m_manager.fillTriangle(110 + arrowOffsetX, 132 + arrowOffsetY, 130 + arrowOffsetX, 132 + arrowOffsetY, 120 + arrowOffsetX, 120 + arrowOffsetY, TFT_GREEN);
            m_manager.drawArc(centre, centre, 120, 118, 0, 360, TFT_GREEN, TFT_GREEN);
        }
        if (!m_stockchangeformat) {
            m_manager.drawString(stock.getPercentChange(2) + "%", centre, 48, bigFontSize, Align::MiddleCenter);
        } else {
            m_manager.drawString(stock.getCurrencySymbol() + stock.getPriceChange(2), centre, 48, bigFontSize, Align::MiddleCenter);
        }
        // Draw stock data
        m_manager.setFontColor(TFT_BLACK, TFT_WHITE);
        m_manager.drawString(stock.getTicker(), centre, 92, bigFontSize, Align::MiddleCenter);

        m_manager.setFontColor(TFT_WHITE, TFT_BLACK);
        m_manager.drawString(stock.getCurrencySymbol() + stock.getCurrentPrice(2), centre, 155, bigFontSize, Align::MiddleCenter);

        break;
    }
}

void StockWidget::nextPage() {
    if (m_pageCount <= 1)
        return;
    // Reset the timer for the next page if we just switched manually
    m_prevMillisSwitch = millis();
    m_page = (m_page + 1) % m_pageCount;
#ifdef STOCK_DEBUG
    Log.infoln("StockWidget Page: %d", m_page + 1);
#endif
    draw(true);
}

void StockWidget::displayClock(int displayIndex) {
    const int dateY = 50;
    const int clockY = 110;
    const int dayOfWeekY = 180;

    m_manager.fillSprite(TFT_BLACK);
    m_manager.setFontColor(TFT_WHITE, TFT_BLACK);

    if (m_digitalClock) {
        m_manager.drawCentreString(m_time->getDayAndMonth(), 120, dateY, 18);
        const String weekDay = m_time->getWeekday();
        m_manager.drawCentreString(weekDay, 120, dayOfWeekY, 22);

        m_manager.drawString(m_time->getTime(), 120, clockY, 66, Align::MiddleCenter);

        if (m_showSecondTickS) {
            int lv_second = m_time->getSecond();
            int startA = ((lv_second * 6) + 180 - 3) % 360;
            int endA = ((lv_second * 6) + 180 + 3) % 360;
            m_manager.drawSmoothArc(SCREEN_SIZE / 2, SCREEN_SIZE / 2, 120, 115, startA, endA, TFT_WHITE, TFT_BLACK);
        }

    } else {
        float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0; // Saved H, M, S x & y multipliers
        float sdeg = 0, mdeg = 0, hdeg = 0;
        uint16_t osx = 120, osy = 120, omx = 120, omy = 120, ohx = 120, ohy = 120; // Saved H, M, S x & y coords
        uint16_t x0 = 0, x1 = 0, yy0 = 0, yy1 = 0;
        m_manager.drawSmoothArc(120, 120, 120, 115, 0, 360, TFT_GREENYELLOW, TFT_BLACK);

        // Draw 12 lines
        for (int i = 0; i < 360; i += 30) {
            sx = cos((i - 90) * 0.0174532925);
            sy = sin((i - 90) * 0.0174532925);
            x0 = sx * 115 + 120;
            yy0 = sy * 115 + 120;
            x1 = sx * 105 + 120;
            yy1 = sy * 105 + 120;

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
                m_manager.fillCircle(x0, yy0, 3, TFT_WHITE);
            else
                // Draw minute markers
                if (i % 30 != 0)
                    m_manager.drawPixel(x0, yy0, TFT_WHITE);
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
        omx = mx * 84 + 120;
        omy = my * 84 + 121;
        osx = sx * 90 + 121;
        osy = sy * 90 + 121;
        m_manager.drawLine(ohx, ohy, 120, 121, TFT_WHITE);
        m_manager.drawLine(omx, omy, 120, 121, TFT_WHITE);
        m_manager.drawLine(osx, osy, 120, 121, TFT_RED);

        m_manager.fillCircle(120, 121, 3, TFT_RED);
    }

    m_manager.pushSprite(displayIndex, 0, 0);
}

String StockWidget::getName() {
    return "Stock";
}
