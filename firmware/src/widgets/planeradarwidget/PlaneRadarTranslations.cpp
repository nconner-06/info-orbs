#include "PlaneRadarTranslations.h"

// Languages are defined in I18n.h:
// EN, DE, FR

// All translation variables for PlaneRadarWidget should start with "t_plane"

constexpr Translation t_radarLat = {
    "Radar center latitude", // EN
};

constexpr Translation t_radarLong = {
    "Radar center longitude", // EN
};

constexpr Translation t_cycleRange = {
    "Range in km", // EN
};
constexpr Translation t_showOnGround = {
    "Show On Ground Planes", // EN
};

constexpr TranslationMulti<5> t_cycleRanges = {
    {
        "5 km", // EN
    },
    {
        "10 km", // EN
    },
    {
        "15 km", // EN
    },
    {
        "20 km", // EN
    },
    {
        "25 km", // EN
    }};
