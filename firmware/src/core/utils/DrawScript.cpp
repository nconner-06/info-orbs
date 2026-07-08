#include "DrawScript.h"
#include "Utils.h"
#include <ArduinoLog.h>
#include <map>

// Static command map definition
const std::map<std::string, DrawScript::CommandType> DrawScript::commandMap = {
    {"f", COMMAND_FILL},
    {"l", COMMAND_LINE},
    {"wil", COMMAND_WIDELINE},
    {"wel", COMMAND_WEDGELINE},
    {"c", COMMAND_CIRCLE},
    {"e", COMMAND_ELLIPSE},
    {"fe", COMMAND_FILL_ELLIPSE},
    {"sc", COMMAND_SMOOTH_CIRCLE},
    {"fc", COMMAND_FILL_CIRCLE},
    {"fsc", COMMAND_FILL_SMOOTH_CIRCLE},
    {"r", COMMAND_RECTANGLE},
    {"rr", COMMAND_ROUND_RECTANGLE},
    {"srr", COMMAND_SMOOTH_ROUND_RECTANGLE},
    {"fr", COMMAND_FILL_RECTANGLE},
    {"frr", COMMAND_FILL_ROUND_RECTANGLE},
    {"fsrr", COMMAND_FILL_SMOOTH_ROUND_RECTANGLE},
    {"tr", COMMAND_TRIANGLE},
    {"ftr", COMMAND_FILL_TRIANGLE},
    {"a", COMMAND_ARC},
    {"sa", COMMAND_SMOOTH_ARC},
    {"i", COMMAND_IMAGE},
    {"t", COMMAND_TEXT},
    {"tc", COMMAND_TEXT_CENTERED},
    {"tf", COMMAND_TEXT_FITTED},
    {"fo", COMMAND_FONT},
    {"fos", COMMAND_FONT_SIZE},
    {"foa", COMMAND_FONT_ALIGN},
    {"focol", COMMAND_FONT_COLOR},
    {"fobcol", COMMAND_FONT_BACK_COLOR}};

DrawScript::DrawScript(ScreenManager &manager) : m_manager(manager) {
}

// Function to parse and execute a command
void DrawScript::processDrawCommand(CommandType command, const std::vector<String> &p) {
    int pc = p.size() - 1;
    // printf(">>processing %s(%d) %d params\n", p[0], command, pc);
    switch (command) {
    case COMMAND_FILL:
        if (pc == 1) {
            m_manager.fillSprite(parseColor(p[1]));
        }
        break;
    case COMMAND_LINE:
        if (pc == 5) {
            m_manager.drawLine(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_WIDELINE:
        if (pc == 6) {
            m_manager.drawWideLine(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]));
        } else if (pc == 7) {
            m_manager.drawWideLine(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]));
        }
        break;
    case COMMAND_WEDGELINE:
        if (pc == 8) {
            m_manager.drawWedgeLine(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), parseColor(p[8]));
        }
        break;
    case COMMAND_CIRCLE:
        if (pc == 4) {
            m_manager.drawCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]));
        }
        break;
    case COMMAND_FILL_CIRCLE:
        if (pc == 4) {
            m_manager.fillCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]));
        }
        break;
    case COMMAND_SMOOTH_CIRCLE:
        if (pc == 5) {
            m_manager.drawSmoothCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_FILL_SMOOTH_CIRCLE:
        if (pc == 4) {
            m_manager.fillSmoothCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]));
        } else if (pc == 5) {
            m_manager.fillSmoothCircle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_ELLIPSE:
        if (pc == 5) {
            m_manager.drawEllipse(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_FILL_ELLIPSE:
        if (pc == 5) {
            m_manager.fillEllipse(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseColor(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_RECTANGLE:
        if (pc == 5) {
            m_manager.drawRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_FILL_RECTANGLE:
        if (pc == 5) {
            m_manager.fillRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_ROUND_RECTANGLE:
        if (pc == 6) {
            m_manager.drawRoundRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]), parseColor(p[6]));
        }
        break;
    case COMMAND_FILL_ROUND_RECTANGLE:
        if (pc == 6) {
            m_manager.fillRoundRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]), parseColor(p[6]));
        }
        break;
    case COMMAND_SMOOTH_ROUND_RECTANGLE:
        if (pc == 8) {
            m_manager.drawSmoothRoundRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]), parseColor(p[6]), parseColor(p[7]), parseColor(p[8]));
        } else if (pc == 9) {
            m_manager.drawSmoothRoundRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]), parseColor(p[6]), parseColor(p[7]), parseColor(p[8]), parseColor(p[9]));
        }
        break;
    case COMMAND_FILL_SMOOTH_ROUND_RECTANGLE:
        if (pc == 6) {
            m_manager.fillSmoothRoundRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]), parseColor(p[6]));
        } else if (pc == 7) {
            m_manager.fillSmoothRoundRect(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseColor(p[5]), parseColor(p[6]), parseColor(p[7]));
        }
        break;
    case COMMAND_TRIANGLE:
        if (pc == 7) {
            m_manager.drawTriangle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]));
        }
        break;
    case COMMAND_FILL_TRIANGLE:
        if (pc == 7) {
            m_manager.fillTriangle(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]));
        }
        break;
    case COMMAND_ARC:
        if (pc == 7) {
            m_manager.drawArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), 0, false);
        } else if (pc == 8) {
            m_manager.drawArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), parseColor(p[8]), true);
        }
        break;
    case COMMAND_SMOOTH_ARC:
        if (pc == 8) {
            m_manager.drawSmoothArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), parseColor(p[8]), false);
        } else if (pc == 9) {
            m_manager.drawSmoothArc(parseInt(p[1]), parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseInt(p[6]), parseColor(p[7]), parseColor(p[8]), parseInt(p[9]));
        }
        break;
    case COMMAND_IMAGE:
        if (pc == 3) {
            m_manager.drawFsJpg(parseInt(p[1]), parseInt(p[2]), p[3].c_str());
        } else if (pc == 4) {
            m_manager.drawFsJpg(parseInt(p[1]), parseInt(p[2]), p[3].c_str(), parseInt(p[4]));
        } else if (pc == 5) {
            m_manager.drawFsJpg(parseInt(p[1]), parseInt(p[2]), p[3].c_str(), parseInt(p[4]), parseColor(p[5]));
        }
        break;
    case COMMAND_TEXT:
        switch (pc) {
        case 3:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]));
            break;
        case 5:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]));
            break;
        case 6:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]), parseColor(p[6]));
            break;
        case 7:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]), parseColor(p[6]), parseColor(p[7]));
            break;
        case 8:
            m_manager.drawString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseAlign(p[5]), parseColor(p[6]), parseColor(p[7]), parseInt(p[8]));
            break;
        }
        break;
    case COMMAND_TEXT_CENTERED:
        if (pc == 3) {
            m_manager.drawCentreString(p[1], parseInt(p[2]), parseInt(p[3]));
        } else if (pc == 4) {
            m_manager.drawCentreString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]));
        }
        break;
    case COMMAND_TEXT_FITTED:
        if (pc == 5) {
            m_manager.drawFittedString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]));
        } else if (pc == 6) {
            m_manager.drawFittedString(p[1], parseInt(p[2]), parseInt(p[3]), parseInt(p[4]), parseInt(p[5]), parseAlign(p[6]));
        }
        break;
    case COMMAND_FONT:
        if (pc == 1) {
            m_manager.setFont(parseFont(p[1]));
        }
        break;
    case COMMAND_FONT_SIZE:
        if (pc == 1) {
            m_manager.setFontSize(parseInt(p[1]));
        }
        break;
    case COMMAND_FONT_ALIGN:
        if (pc == 1) {
            m_manager.setAlignment(parseAlign(p[1]));
        }
        break;
    case COMMAND_FONT_COLOR:
        if (pc == 1) {
            m_manager.setFontColor(parseColor(p[1]));
        }
        break;
    case COMMAND_FONT_BACK_COLOR:
        if (pc == 1) {
            m_manager.setBackgroundColor(parseColor(p[1]));
        }
        break;
    }
}

// Function to process an entire script
void DrawScript::processScript(const String &script) {
    int lineStart = 0;
    int lineEnd;

    while (lineStart < script.length()) {
        lineEnd = script.indexOf('\n', lineStart);
        if (lineEnd == -1)
            lineEnd = script.length();

        String line = script.substring(lineStart, lineEnd);
        line.replace("\r", ""); // Handle Windows-style text
        line.trim();

        std::vector<String> parameters = parseLine(line);

        if (!parameters.empty()) {
            auto it = commandMap.find(parameters[0].c_str());
            CommandType command = (it != commandMap.end()) ? it->second : COMMAND_UNKNOWN;

            processDrawCommand(command, parameters);
        }

        lineStart = lineEnd + 1;
    }
}
// Helper functions for parameter extraction
std::vector<String> DrawScript::parseLine(const String &line) {
    std::vector<String> tokens;
    String currentToken;
    bool escape = false;

    for (size_t i = 0; i < line.length(); i++) {
        char c = line[i];

        if (escape) {
            currentToken += c;
            escape = false;
        } else if (c == '\\') {
            escape = true;
        } else if (c == ',') {
            tokens.push_back(currentToken);
            currentToken = "";
        } else {
            currentToken += c;
        }
    }

    if (!currentToken.isEmpty()) {
        tokens.push_back(currentToken);
    }

    return tokens;
}

int DrawScript::parseInt(const String &p) {
    int result;
    if (p.startsWith("y") || p.startsWith("t")) {
        result = 1;
    } else if (p.startsWith("n") || p.startsWith("f")) {
        result = 0;
    } else {
        result = strtol(p.c_str(), nullptr, 0);
    }
    return result;
}

TTF_Font DrawScript::parseFont(const String &name) {
    TTF_Font fval = DEFAULT_FONT;
    if (name.startsWith("r")) {
        fval = ROBOTO_REGULAR;
    } else if (name.startsWith("f")) {
        fval = FINAL_FRONTIER;
    } else if (name == "dseg7") {
        fval = DSEG7;
    } else if (name == "dseg14") {
        fval = DSEG14;
    }
    //   Log.infoln("font name '%s' = %d\n",name.c_str(), fval);
    return fval;
}

int DrawScript::parseColor(const String &p) {
    const char *start = p.c_str();
    int value;
    // Check if the parameter begins with a letter
    if (isalpha(*start)) {
        value = Utils::stringToColor(p);
    } else {
        // Treat as integer (including base 16)
        value = strtol(start, nullptr, 0); // Automatically detects base
        if (value != -1) {
            uint16_t r = (value >> 8) & 0xF800;
            uint16_t g = (value >> 5) & 0x07E0;
            uint16_t b = (value >> 3) & 0x001F;
            value = r | g | b;
        }
    }
    // Log.infoln("color value=%x\n", value);
    return value;
}

static const std::map<const std::string, const Align> alignStringsMap = {
    {"l", Align::Left},
    {"c", Align::Center},
    {"r", Align::Right},
    {"tl", Align::TopLeft},
    {"tc", Align::TopCenter},
    {"tr", Align::TopRight},
    {"ml", Align::MiddleLeft},
    {"mc", Align::MiddleCenter},
    {"mr", Align::MiddleRight},
    {"bl", Align::BottomLeft},
    {"bc", Align::BottomCenter},
    {"br", Align::BottomRight}};

Align DrawScript::parseAlign(const String &p) {
    Align val = Align::MiddleCenter;
    auto it = alignStringsMap.find(p.c_str());
    if (it != alignStringsMap.end()) {
        val = it->second;
    }
    // Log.infoln("align value=%d\n", val);
    return val;
}
