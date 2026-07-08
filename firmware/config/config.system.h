#ifndef CONFIG_SYSTEM_H
#define CONFIG_SYSTEM_H

#undef ILI9341_DRIVER
#ifndef GC9A01_DRIVER
    #define GC9A01_DRIVER
#endif
// #ifndef TFT_SDA_READ
// #define TFT_SDA_READ
// #endif

// Default pin values
// Pins can be overridden in platformio.ini build_flags section e.g. -D TFT_MOSI=23
#ifndef TFT_MOSI
    #define TFT_MOSI 17
#endif
#ifndef TFT_MISO
    #define TFT_MISO -1
#endif
#ifndef TFT_SCLK
    #define TFT_SCLK 23
#endif
#ifndef TFT_CS
    #define TFT_CS -1
#endif
#ifndef TFT_DC
    #define TFT_DC 19
#endif
#ifndef TFT_RST
    #define TFT_RST 18
#endif

#ifndef SCREEN_1_CS
    #define SCREEN_1_CS 13
#endif
#ifndef SCREEN_2_CS
    #define SCREEN_2_CS 33
#endif
#ifndef SCREEN_3_CS
    #define SCREEN_3_CS 32
#endif
#ifndef SCREEN_4_CS
    #define SCREEN_4_CS 25
#endif
#ifndef SCREEN_5_CS
    #define SCREEN_5_CS 21
#endif

#ifndef BUTTON_LEFT_PIN
    #define BUTTON_LEFT_PIN 26
#endif
#ifndef BUTTON_MIDDLE_PIN
    #define BUTTON_MIDDLE_PIN 27
#endif
#ifndef BUTTON_RIGHT_PIN
    #define BUTTON_RIGHT_PIN 14
#endif
#ifndef BUTTON_DEBOUNCE_TIME
    #define BUTTON_DEBOUNCE_TIME 35 // Debounce buttons for X ms
#endif
#ifndef BUTTON_MEDIUM_PRESS_TIME
    #define BUTTON_MEDIUM_PRESS_TIME 500 // Medium press is registered after X ms
#endif
#ifndef BUTTON_LONG_PRESS_TIME
    #define BUTTON_LONG_PRESS_TIME 2000 // Long press is registered after X ms
#endif
#ifndef BUTTON_VERY_LONG_PRESS_TIME
    #define BUTTON_VERY_LONG_PRESS_TIME 15000 // 15 seconds
#endif
#ifndef BUTTON_MODE
    #define BUTTON_MODE INPUT_PULLDOWN
#endif
#ifndef BUSY_PIN
    #define BUSY_PIN 2
#endif
#ifndef ROTARY_PIN1
    #define ROTARY_PIN1 40
#endif
#ifndef ROTARY_PIN2
    #define ROTARY_PIN2 39
#endif
#ifndef ROTARY_PIN3
    #define ROTARY_PIN3 3
#endif

// LED_TYPE 0 = led, 1 = WS2812
#ifndef LED_TYPE
    #define LED_TYPE 0
#endif
#ifndef LED_COLOR
    #define LED_COLOR 0x001F
#endif

// Ambient light sensing for auto dimming. Must have photoresistor hardware mod.
// This entails wiring a GL5537 photoresistor between GPIO34 and 3.3v; and a
// 22K resistor between GPIO34 and gnd.
#ifndef LIGHT_SENSE_PIN
    #define LIGHT_SENSE_PIN 34
#endif
#ifndef LIGHT_MIN
    #define LIGHT_MIN 0 // (0-4095)
#endif
#ifndef LIGHT_MAX
    #define LIGHT_MAX 4095 // (0-4095)
#endif

// Pins for the SD card
#ifndef SD_MISO
    #define SD_MISO = 13
#endif

#ifndef SD_CLK
    #define SD_CLK = 12
#endif

#ifndef SD_MOSI
    #define SD_MOSI = 11
#endif

#ifndef SD_CS
    #define SD_CS = 10
#endif

#ifndef NTP_SERVER
    #define NTP_SERVER "pool.ntp.org"
#endif

// Maximum number of enabled widgets
// The ESP might run out of memory if this is set too high
#ifndef MAX_WIDGETS
    #define MAX_WIDGETS 12
#endif

#ifndef SCREEN_SIZE
    #define SCREEN_SIZE 240
#endif
#ifndef TFT_WIDTH
    #define TFT_WIDTH SCREEN_SIZE
#endif
#ifndef TFT_HEIGHT
    #define TFT_HEIGHT SCREEN_SIZE
#endif

// Legacy fonts
#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8N
#define LOAD_GFXFF
#define SMOOTH_FONT

#ifndef SPI_FREQUENCY
    #define SPI_FREQUENCY 27000000
#endif

#ifndef TIMEZONE_API_URL
    #define TIMEZONE_API_URL "http://timeapi.io/api/timezone/zone"
#endif

#ifndef TIMEZONE_API_LOCATION
    #define TIMEZONE_API_LOCATION "America/Vancouver"
#endif

#ifndef ORB_ROTATION
    #define ORB_ROTATION 0
#endif

#ifndef WIDGET_CYCLE_DELAY
    #define WIDGET_CYCLE_DELAY 0
#endif

#ifndef MAX_RETRIES
    #define MAX_RETRIES 3
#endif

#ifndef LOG_LEVEL
    #define LOG_LEVEL LOG_LEVEL_INFO
#endif

// WIDGETS
#ifndef INCLUDE_WEATHER
    #define INCLUDE_WEATHER WIDGET_ON
#endif
#ifndef INCLUDE_STOCK
    #define INCLUDE_STOCK WIDGET_ON
#endif
#ifndef INCLUDE_PARQET
    #define INCLUDE_PARQET WIDGET_OFF
#endif
#ifndef INCLUDE_WEBDATA
    #define INCLUDE_WEBDATA WIDGET_DISABLED
#endif
#ifndef INCLUDE_MQTT
    #define INCLUDE_MQTT WIDGET_DISABLED
#endif
#ifndef INCLUDE_5ZONE
    #define INCLUDE_5ZONE WIDGET_OFF
#endif
#ifndef INCLUDE_MATRIXSCREEN
    #define INCLUDE_MATRIXSCREEN WIDGET_OFF
#endif
#ifndef INCLUDE_SPORT
    #define INCLUDE_SPORT WIDGET_OFF
#endif
#ifndef INCLUDE_PLANE_RADAR
    #define INCLUDE_PLANE_RADAR WIDGET_OFF
#endif
#ifndef INCLUDE_RADIO
    #define INCLUDE_RADIO WIDGET_OFF
#endif
#ifndef INCLUDE_MP3
    #define INCLUDE_MP3 WIDGET_OFF
#endif

// CLOCK WIDGET SETTINGS
#ifndef FORMAT_24_HOUR
    #define FORMAT_24_HOUR false
#endif
#ifndef SHOW_AM_PM_INDICATOR
    #define SHOW_AM_PM_INDICATOR false
#endif
#ifndef SHOW_SECOND_TICKS
    #define SHOW_SECOND_TICKS true
#endif
#ifndef CLOCK_COLOR
    #define CLOCK_COLOR 0xfc80
#endif
#ifndef CLOCK_SHADOW_COLOR
    #define CLOCK_SHADOW_COLOR 0x20a1
#endif
#ifndef CLOCK_SHADOWING
    #define CLOCK_SHADOWING true
#endif
#ifndef USE_CLOCK_NIXIE
    #define USE_CLOCK_NIXIE NIXIE_NOHOLES
#endif
#ifndef USE_CLOCK_CUSTOM
    #define USE_CLOCK_CUSTOM 1
#endif
#ifndef DEFAULT_CLOCK
    #define DEFAULT_CLOCK ClockType::NORMAL
#endif

// WEATHER FEEDS

#ifndef WEATHER_VISUALCROSSING_FEED
    #define WEATHER_VISUALCROSSING_FEED true
#endif
#ifndef WEATHER_OPENWEATHERMAP_FEED
    #define WEATHER_OPENWEATHERMAP_FEED false
#endif
#ifndef WEATHER_TEMPEST_FEED
    #define WEATHER_TEMPEST_FEED false
#endif

#ifndef WEATHER_VISUALCROSSING_LOCATION
    #define WEATHER_VISUALCROSSING_LOCATION "Victoria, BC"
#endif

#ifndef WEATHER_OPENWEATHERMAP_LAT
    #define WEATHER_OPENWEATHERMAP_LAT "41.9795"
#endif
#ifndef WEATHER_OPENWEATHERMAP_LON
    #define WEATHER_OPENWEATHERMAP_LON "-87.8865"
#endif
#ifndef WEATHER_OPENWEATHERMAP_NAME
    #define WEATHER_OPENWEATHERMAP_NAME "Chicago"
#endif
#ifndef WEATHER_TEMPEST_STATION_ID
    #define WEATHER_TEMPEST_STATION_ID "93748" // Set the station ID of your Tempest weather station
#endif
#ifndef WEATHER_TEMPEST_STATION_NAME
    #define WEATHER_TEMPEST_STATION_NAME "Chicago" // Set the name of your Tempest weather station
#endif

#ifndef WEATHER_VISUALCROSSING_API_KEY
    #define WEATHER_VISUALCROSSING_API_KEY "XW2RDGD6XK432AF25BNK2A3C7"
#endif
#ifndef WEATHER_OPENWEATHERMAP_API_KEY
    #define WEATHER_OPENWEATHERMAP_API_KEY "SOME-KEY-GOES-HERE"
#endif
#ifndef WEATHER_TEMPEST_API_KEY
    #define WEATHER_TEMPEST_API_KEY "20c70eae-e62f-4d3b-b3a4-8586e90f3ac8"
#endif

// STOCK WIDGET SETTINGS
#ifndef STOCK_API_URL
    #define STOCK_API_URL "https://api.twelvedata.com/quote"
#endif
#ifndef STOCK_API_KEY
    #define STOCK_API_KEY "e03fc53524454ab8b65d91b23c669cc5"
#endif
#ifndef STOCK_TICKER_LIST
    #define STOCK_TICKER_LIST "BTC/USD,USD/CAD,XEQT,SPY,APC&country=Germany"
#endif
#ifndef STOCK_CHANGE_FORMAT
    #define STOCK_CHANGE_FORMAT 0
#endif

#ifndef SPORT_API_URL
    #define SPORT_API_URL "https://site.api.espn.com/apis/site/v2/sports/"
#endif

#ifndef RADAR1_API_URL
    #define RADAR1_API_URL "https://opendata.adsb.fi/api/v3/lat/"
#endif

#ifndef RADAR2_API_URL
    #define RADAR2_API_URL "https://api.adsbdb.com/v0/callsign/"
#endif

#endif
