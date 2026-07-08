#include "WifiWidget.h"
#include "OrbsWiFiManager.h"
#include "Utils.h"
#include <ArduinoLog.h>
#include <ESPmDNS.h>
#include <WiFi.h>

const int lineHeight = 40;
const int statusScreenIndex = 3;
const int fontSize = 19;
const int messageDelay = 5000;

WifiWidget::WifiWidget(ScreenManager &manager, ConfigManager &config, WiFiManager &wifiManager) : Widget(manager, config), m_wifiManager(wifiManager) {}

WifiWidget::~WifiWidget() {}

void WifiWidget::setup() {
    m_manager.setFont(DEFAULT_FONT);
    m_manager.setFontColor(TFT_WHITE);

    m_manager.fillSprite(TFT_BLACK);
    m_manager.drawCentreString("Connecting", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
    m_manager.pushSprite(statusScreenIndex, 0, 0);

    // Hold right button when connecting to power to reset wifi settings
    // these are stored by the ESP WiFi library
    if (digitalRead(BUTTON_RIGHT_PIN) == Button::PRESSED_LEVEL) {
        m_wifiManager.resetSettings();
        m_manager.fillSprite(TFT_BLACK);
        m_manager.drawCentreString("Wifi Settings reset", ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        m_manager.pushSprite(statusScreenIndex, 0, 0);
        delay(messageDelay);
    }

    // Set hostname to InfoOrbs-<last 6 digits of MAC address> so it can be found on the network
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    String hostname = "InfoOrbs-" + mac.substring(mac.length() - 6);
    WiFi.softAPsetHostname(hostname.c_str());
    m_wifiManager.setHostname(hostname);
    WiFi.mode(WIFI_STA); // For WiFiManager explicitly set mode to station, ESP defaults to STA+AP
    WiFi.setTxPower(WIFI_POWER_18_5dBm);
    WiFi.setSleep(false);

#if (defined WIFI_SSID && defined WIFI_PASS)
    // Preload credentials from config.h
    m_wifiManager.preloadWiFi(WIFI_SSID, WIFI_PASS);
#endif

    // Remove unwanted buttons from the config portal
    //    std::vector<const char *> wm_menu = {"wifi", "custom", "info", "restart"}; // buttons: wifi, info, exit, update
    std::vector<const char *> wm_menu = {"wifi", "param", "custom", "info", "restart"}; // buttons: wifi, info, exit, update
    // Remove unwanted buttons from the Info page
    m_wifiManager.setShowInfoUpdate(false);
    m_wifiManager.setShowInfoErase(false);
    // Add buttons link
    const char *customMenuHtml = "<form action='/browse' method='get'><button>Browse Filesystem</button></form><br/>\n"
                                 "<form action='/buttons' method='get'><button>Buttons</button></form><br/>\n";
    m_wifiManager.setCustomMenuHTML(customMenuHtml);
    m_wifiManager.setMenu(wm_menu);
    m_wifiManager.setClass("invert"); // Dark mode
    m_wifiManager.setShowStaticFields(true);

    // Set WiFiManager to non-blocking so status and info can be displayed
    m_wifiManager.setConfigPortalBlocking(false);

    // If you want the config portal to only be available for so many seconds
    // wm.setConfigPortalTimeout(60);

    // Use a static SSID to be able to show a static QR Code
    m_apssid = "InfoOrbs";

    m_wifiManager.setCleanConnect(true);
    m_wifiManager.setConnectTimeout(3);
    m_wifiManager.setConnectRetries(10);

    Log.infoln("Hostname: %s", hostname.c_str());

    // WiFiManager automatically connects using saved credentials...
    if (m_wifiManager.autoConnect(m_apssid.c_str())) {
        Log.infoln("WifiManager connected.");
    } else { // ...if connection fails (no saved credentials), it starts an access point with a WiFi setup portal at 192.168.4.1
        m_configPortalRunning = true;
        Log.infoln("Configuration portal running.");
        m_manager.fillSprite(TFT_BLACK);
        m_manager.drawCentreString("Configure", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.pushSprite(statusScreenIndex, 0, 0);

        m_manager.fillSprite(TFT_BLACK);
        m_manager.drawCentreString("Connect", ScreenCenterX, ScreenCenterY - lineHeight * 2, fontSize);
        m_manager.drawCentreString("phone or PC", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.drawCentreString("to WiFi network:", ScreenCenterX, ScreenCenterY, fontSize);
        m_manager.setFontColor(TFT_SKYBLUE);
        m_manager.drawCentreString(m_apssid, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        m_manager.setFontColor(TFT_GREENYELLOW);
        m_manager.drawCentreString("192.168.4.1", ScreenCenterX, ScreenCenterY + lineHeight * 2, fontSize);
        m_manager.pushSprite(statusScreenIndex + 1, 0, 0);
        m_manager.setFontColor(TFT_WHITE);
    }
}

void WifiWidget::update(bool force) {
    // Force is currently unhandled due to not knowing what behavior it would change

    // If WiFiManager is non-blocking, this keeps the configuration portal running
    m_wifiManager.process();

    if (WiFi.status() == WL_CONNECTED) {
        m_isConnected = true;
        m_connectionString = "Connected";
        m_ipaddress = WiFi.localIP().toString();
        Log.infoln("IP address: %s", m_ipaddress.c_str());
        // Start the WebPortal
        m_wifiManager.startWebPortal();
#ifdef INCLUDE_MDNS
        // Initialize mDNS
        String mDNSname = m_wifiManager.getWiFiHostname();
        if (!MDNS.begin(mDNSname)) {
            Log.warningln("Error setting up MDNS responder!");
        } else {
            Log.infoln("mDNS responder started. You should find this device at http://%s\n", mDNSname.c_str());
        }
        MDNS.addService("http", "tcp", 80);
#endif
    } else {
        m_connectionTimer += 500;
        m_dotsString += " . ";
        Serial.print(".");
        if (m_dotsString.length() > 9) {
            m_dotsString = "";
        }
        if (m_connectionTimer > m_connectionTimeout && !m_configPortalRunning) {
            m_connectionFailed = true;
            connectionTimedOut();
        }
    }
}

void WifiWidget::draw(bool force) {
    // Force is currently unhandled due to not knowing what behavior it would change
    m_manager.fillSprite(TFT_BLACK);

    if (!m_isConnected && !m_connectionFailed) {
        m_manager.drawCentreString("Configure", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.drawCentreString(m_dotsString, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        m_manager.pushSprite(statusScreenIndex, 0, 0);
    } else if (m_isConnected && !m_hasDisplayedSuccess) {
        m_hasDisplayedSuccess = true;
        m_manager.drawCentreString("Success", ScreenCenterX, ScreenCenterY, fontSize);
        m_manager.pushSprite(statusScreenIndex, 0, 0);

        m_manager.fillSprite(TFT_BLACK);
        m_manager.drawCentreString("IP Address", ScreenCenterX, ScreenCenterY - lineHeight, fontSize);
        m_manager.drawCentreString(m_ipaddress, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        m_manager.pushSprite(statusScreenIndex + 1, 0, 0);
        Log.infoln("Connected to WiFi");
        delay(messageDelay);
    } else if (m_connectionFailed && !m_hasDisplayedError) {
        m_hasDisplayedError = true;
        m_manager.drawCentreString(m_connectionString, ScreenCenterX, ScreenCenterY + lineHeight, fontSize);
        m_manager.pushSprite(statusScreenIndex, 0, 0);
        delay(messageDelay);
    }
}

void WifiWidget::onLeave(bool force) {}

void WifiWidget::buttonPressed(uint8_t buttonId, ButtonState state) {}

void WifiWidget::connectionTimedOut() {
    switch (WiFi.status()) {
    case WL_CONNECTED:
        m_connectionString = "Connected";
        break;
    case WL_NO_SSID_AVAIL:
        m_connectionString = "No SSID available";
        break;
    case WL_CONNECT_FAILED:
        m_connectionString = "Connection failed";
        break;
    case WL_IDLE_STATUS:
        m_connectionString = "Idle status";
        break;
    case WL_DISCONNECTED:
        m_connectionString = "Disconnected";
        break;
    default:
        m_connectionString = "Unknown";
        break;
    }
}

String WifiWidget::getName() {
    return "WiFi";
}
