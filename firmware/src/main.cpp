#include "DimmingManager.h"
#include "GlobalResources.h"
#include "GlobalTime.h"
#include "MainHelper.h"
#include "TaskFactory.h"
#include "WidgetRegistry.h"
#include "wifiwidget/WifiWidget.h"
#include <ArduinoLog.h>

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite spr = TFT_eSprite(&tft);

GlobalTime *globalTime{nullptr};
WifiWidget *wifiWidget{nullptr};
ScreenManager *sm{nullptr};
ConfigManager *config{nullptr};
OrbsWiFiManager *wifiManager{nullptr};
WidgetSet *widgetSet{nullptr};
DimmingManager *dimmerInstance{nullptr};
TaskManager *taskInstance{nullptr};

void setup() {
    // Initialize global resources
    initializeGlobalResources();

#ifdef SERIAL_INTERFACE_INIT_DELAY
    // Add a delay to allow the serial interface to initialize
    delay(SERIAL_INTERFACE_INIT_DELAY);
#endif

    Serial.begin(115200);

    // Clear the serial buffer of any garbage
    while (Serial.available() > 0) {
        Serial.read();
    }

#ifdef LOG_TIMESTAMP
    Log.setPrefix(MainHelper::printPrefix);
#endif
    Log.begin(LOG_LEVEL, &Serial);
    Log.noticeln("🚀 Starting up...");
    Log.noticeln("PCB Version: %s", PCB_VERSION);

    wifiManager = new OrbsWiFiManager();
    config = new ConfigManager(*wifiManager);
    sm = new ScreenManager(tft, spr);
    widgetSet = new WidgetSet(sm);

    // Pass references to MainHelper
    MainHelper::init(wifiManager, config, sm, widgetSet);
    MainHelper::setupLittleFS();
    MainHelper::setupConfig();
    MainHelper::setupButtons();
    MainHelper::showWelcome();

    if (MainHelper::getLedType() == 0)
        pinMode(MainHelper::getBusyPin(), OUTPUT);

    Log.noticeln("Connecting to WiFi");
    wifiWidget = new WifiWidget(*sm, *config, *wifiManager);
    wifiWidget->setup();

    globalTime = GlobalTime::getInstance();

    registerWidgets(widgetSet, sm, config);

    config->setupWebPortal();

    MainHelper::resetCycleTimer();
    if (MainHelper::getAutoDim()) {
        dimmerInstance = DimmingManager::getInstance();
    }
    taskInstance = TaskManager::getInstance();
}

void loop() {
    MainHelper::watchdogReset();
    unsigned long wifiCurrentMillis = millis();
    if (wifiWidget->isConnected() == false) {
        wifiWidget->update();
        wifiWidget->draw();
        delay(100);
    } else {
        if (!widgetSet->initialUpdateDone()) {
            globalTime->updateTime(true);
            widgetSet->initializeAllWidgetsData();
            MainHelper::setupWebPortalEndpoints();
        }

        wifiManager->process();

        globalTime->updateTime();

        MainHelper::checkButtons();

        widgetSet->updateCurrent();

        if (MainHelper::getAutoDim())
            dimmerInstance->updateBrightness(sm, widgetSet);
        else
            MainHelper::updateBrightnessByTime(globalTime->getHour24());

        widgetSet->drawCurrent();

        MainHelper::checkCycleWidgets();

        taskInstance->processAwaitingTasks();
        taskInstance->processTaskResponses();
    }
#ifdef MEMORY_DEBUG_INTERVAL
    ShowMemoryUsage::printSerial();
#endif
    MainHelper::restartIfNecessary();
}
