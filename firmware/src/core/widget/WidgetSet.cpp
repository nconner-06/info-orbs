#include "WidgetSet.h"
#include <ArduinoLog.h>

WidgetSet::WidgetSet(ScreenManager *sm) : m_screenManager(sm) {
}

void WidgetSet::add(Widget *widget) {
    if (!widget->isEnabled()) {
        Log.infoln("Widget %s is disabled", widget->getName().c_str());
        return;
    }
    if (m_widgetCount == MAX_WIDGETS) {
        Log.warningln("MAX WIDGETS UNABLE TO ADD");
        return;
    }
    m_widgets[m_widgetCount] = widget;
    if (widget->isEnabled()) {
        m_enabledWidgets++;
        m_widgets[m_widgetCount]->setup();
    }
    m_widgetCount++;
}

void WidgetSet::addForced(Widget *widget) {
    widget->setEnabled();
    Log.infoln("\nWidget %s is force enabled\n", widget->getName().c_str());

    if (m_widgetCount == MAX_WIDGETS) {
        Log.warningln("MAX WIDGETS UNABLE TO ADD");
        return;
    }
    m_widgets[m_widgetCount] = widget;
    m_enabledWidgets++;
    m_widgets[m_widgetCount]->setup();
    m_widgetCount++;
}

void WidgetSet::drawCurrent(bool force) {
    Widget *currentWidget = m_widgets[m_currentWidget];
    if (force || currentWidget->isItTimeToDraw()) {
        Log.traceln("Drawing widget: %s", currentWidget->getName().c_str());
        if (currentWidget->isItTimeToUpdate()) {
            currentWidget->update();
        }
        if (m_clearScreensOnDrawCurrent) {
            m_clearScreensOnDrawCurrent = false;
            currentWidget->draw(true);
        } else {
            currentWidget->draw(force);
        }
    }
}

void WidgetSet::updateCurrent() {
    Widget *currentWidget = m_widgets[m_currentWidget];
    if (!getCurrent()->isEnabled())
        next();
    if (currentWidget->isItTimeToUpdate()) {
        Log.traceln("Updating widget: %s", currentWidget->getName().c_str());
        currentWidget->update();
    }
}

Widget *WidgetSet::getCurrent() {
    return m_widgets[m_currentWidget];
}

int WidgetSet::getEnabledWidgetCount() {
    return m_enabledWidgets;
}

Widget *WidgetSet::getWidget(int widgetId) {
    return m_widgets[widgetId];
}

void WidgetSet::buttonPressed(uint8_t buttonId, ButtonState state) {
    m_widgets[m_currentWidget]->buttonPressed(buttonId, state);
}

void WidgetSet::setClearScreensOnDrawCurrent() {
    m_clearScreensOnDrawCurrent = true;
}

void WidgetSet::next() {
    if (m_previousWidget == -1)
        m_previousWidget = m_currentWidget;
    m_currentWidget++;
    if (m_currentWidget >= m_widgetCount) {
        m_currentWidget = 0;
    }
    if (!getCurrent()->isEnabled()) {
        next();
    } else {
        switchWidget();
    }
}

void WidgetSet::prev() {
    if (m_previousWidget == -1)
        m_previousWidget = m_currentWidget;
    if (m_currentWidget == 0) {
        if (m_widgetCount > 0)
            m_currentWidget = m_widgetCount - 1;
        else
            m_currentWidget = 0;
    } else {
        m_currentWidget--;
    }
    if (!getCurrent()->isEnabled()) {
        prev();
    } else {
        switchWidget();
    }
}

void WidgetSet::switchWidget() {
    if (m_previousWidget != -1) {
        if (m_previousWidget != m_currentWidget) {
            Log.noticeln("Previous widget was : %s", getWidget(m_previousWidget)->getName());
            getWidget(m_previousWidget)->onLeave();
        }
        m_previousWidget = -1;
    }
    m_screenManager->clearAllScreens();
    getCurrent()->setup();
    uint16_t start = millis();
    getCurrent()->draw(true);
    uint16_t end = millis();
    Log.noticeln("Drawing of %s took %d ms", getCurrent()->getName().c_str(), (end - start));
}

void WidgetSet::switchToWidget(int newWidget) {
    m_previousWidget = m_currentWidget;
    m_currentWidget = newWidget;

    if (m_previousWidget != m_currentWidget) {
        Log.noticeln("Previous widget was : %s", getWidget(m_previousWidget)->getName());
        getWidget(m_previousWidget)->onLeave();
        m_previousWidget = -1;

        m_screenManager->clearAllScreens();
        getCurrent()->setup();
        uint16_t start = millis();
        getCurrent()->draw(true);
        uint16_t end = millis();
        Log.noticeln("Drawing of %s took %d ms", getCurrent()->getName().c_str(), (end - start));
    }
}

void WidgetSet::showCenteredLine(int screen, const String &text) {
    m_screenManager->fillSprite(TFT_BLACK);
    m_screenManager->setFontColor(TFT_WHITE);
    m_screenManager->drawCentreString(text, ScreenCenterX, ScreenCenterY, 22);
    m_screenManager->pushSprite(screen, 0, 0);
}

void WidgetSet::showLoading() {
    showCenteredLine(3, I18n::get(t_loadingData));
}

void WidgetSet::updateAll() {
    for (uint8_t i = 0; i < m_widgetCount; i++) {
        if (m_widgets[i]->isEnabled()) {
            Log.infoln("updating widget %s", m_widgets[i]->getName().c_str());
            showCenteredLine(4, m_widgets[i]->getName());
            m_widgets[i]->update();
#ifdef WIDGETSET_DEBUG
            ShowMemoryUsage::printSerial(true);
#endif
        }
    }
}

bool WidgetSet::initialUpdateDone() {
    return m_initialized;
}

void WidgetSet::initializeAllWidgetsData() {
    showLoading();
    updateAll();
    m_initialized = true;
    m_screenManager->clearAllScreens();
}

int WidgetSet::getWidgetCount() {
    return m_widgetCount;
}
