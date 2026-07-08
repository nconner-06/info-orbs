#ifndef DRAWSCRIPT_H
#define DRAWSCRIPT_H
#include "ScreenManager.h"
#include <map>
#include <vector>

class DrawScript {
public:
    /**
     * @brief Constructor for DrawScript.
     *
     * @param manager Reference to the ScreenManager.
     */
    DrawScript(ScreenManager &manager);
    void processScript(const String &script);

private:
    ScreenManager &m_manager;
    // Define command enums
    enum CommandType {
        COMMAND_FILL,
        COMMAND_LINE,
        COMMAND_WIDELINE,
        COMMAND_WEDGELINE,
        COMMAND_SMOOTH_CIRCLE,
        COMMAND_FILL_SMOOTH_CIRCLE,
        COMMAND_CIRCLE,
        COMMAND_FILL_CIRCLE,
        COMMAND_ELLIPSE,
        COMMAND_FILL_ELLIPSE,
        COMMAND_RECTANGLE,
        COMMAND_FILL_RECTANGLE,
        COMMAND_ROUND_RECTANGLE,
        COMMAND_FILL_ROUND_RECTANGLE,
        COMMAND_SMOOTH_ROUND_RECTANGLE,
        COMMAND_FILL_SMOOTH_ROUND_RECTANGLE,
        COMMAND_TRIANGLE,
        COMMAND_FILL_TRIANGLE,
        COMMAND_ARC,
        COMMAND_SMOOTH_ARC,
        COMMAND_IMAGE,
        COMMAND_TEXT,
        COMMAND_TEXT_CENTERED,
        COMMAND_TEXT_FITTED,
        COMMAND_FONT,
        COMMAND_FONT_SIZE,
        COMMAND_FONT_ALIGN,
        COMMAND_FONT_COLOR,
        COMMAND_FONT_BACK_COLOR,
        COMMAND_UNKNOWN
    };
    static const std::map<std::string, DrawScript::CommandType> commandMap;

    void processDrawCommand(CommandType command, const std::vector<String> &parameters);
    std::vector<String> parseLine(const String &line);

    int parseInt(const String &p);
    int parseColor(const String &p);
    TTF_Font parseFont(const String &p);
    Align parseAlign(const String &p);
};
#endif
