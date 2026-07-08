#ifndef DIMMINGMANAGER_H
#define DIMMINGMANAGER_H

// Include any necessary libraries here
#include "ScreenManager.h"
#include "WidgetSet.h"
#include "config_helper.h"

#ifndef AUTO_DIMMING_ENABLED
    #define AUTO_DIMMING_ENABLED false
#endif

#ifndef AUTO_BRIGHTNESS_MIN
    #define AUTO_BRIGHTNESS_MIN 80 // (0-255)
#endif

#ifndef AUTO_BRIGHTNESS_MAX
    #define AUTO_BRIGHTNESS_MAX 255 // (0-255)
#endif

#ifndef NUM_BRIGHTNESS_LEVELS
    #define NUM_BRIGHTNESS_LEVELS 10 // (2 or greater)
#endif

// Singleton class to handle auto dimming
class DimmingManager {
public:
    // Don't allow copying or assigning the singleton
    DimmingManager(const DimmingManager &) = delete;
    DimmingManager &operator=(const DimmingManager &) = delete;

    // Get the one and only instance of the singleton
    static DimmingManager *getInstance();

    void setup();

    void updateBrightness(ScreenManager *sm, WidgetSet *ws);

private:
    // Don't allow constructing the singleton from outside its class
    DimmingManager();
    ~DimmingManager();

    static DimmingManager *m_instance;

    bool isTimeToUpdate();

    uint16_t filter(uint16_t lightIntensity);
    int16_t hysteresis(uint16_t lightIntensity);
    uint16_t lightIntensity_ = LIGHT_MAX;
    uint16_t lowerHysteresis_ = LIGHT_MAX;
    uint16_t upperHysteresis_ = LIGHT_MIN;
    uint16_t brightnessLevel_ = 0;
    uint16_t screenBrightness_ = 0;
    uint64_t lastUpdateTime_uS_ = 0;
};

#endif // Include guard