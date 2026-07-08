#include "SportTranslations.h"

constexpr Translation t_Range = {
    "Select time Range for Event search", // EN
};

constexpr TranslationMulti<4> t_RangeOptions = {
    {
        "+/-1 Day", // EN
    },
    {
        "+/-7 Days", // EN
    },
    {
        "+/-14 Days", // EN
    },
    {
        "Manual Range", // EN
    }};

constexpr Translation t_StartRange = {
    "Start range x days from today", // EN
};

constexpr Translation t_EndRange = {
    "Search range n days from start", // EN
};

constexpr Translation t_League = {
    "Select the League for Event search", // EN
};

constexpr TranslationMulti<9> t_LeagueOptions = {
    {
        "Football - NFL", // EN
    },
    {
        "Basketball - NBA", // EN
    },
    {
        "Baseball - MLB", // EN
    },
    {
        "Hockey - NHL", // EN
    },
    {
        "Soccer - English Premier League", // EN
    },
    {
        "Soccer - English League Championship", // EN
    },
    {
        "Soccer - French Ligue 1", // EN
    },
    {
        "Soccer - German Bundesliga", // EN
    },
    {
        "Soccer - Worl Cup 2026", // EN
    }};

constexpr Translation t_recordLimit = {
    "Limit number of events to return (max 75)", // EN
};

constexpr Translation t_sportCycle = {
    "Switch between Events every X seconds", // EN
};

constexpr Translation t_noEvents = {
    "No Events for this Period", // EN
};

constexpr Translation t_loadingEventData = {
    "Loading data", // EN
    "Lade Daten", // DE
    "Chargement" // FR
};
