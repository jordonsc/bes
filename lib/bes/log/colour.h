#pragma once

enum class ColourMode
{
    ENABLE,
    DISABLE,
    AUTO
};

/// Modifiers
#define BES_CLR_BOLD "\033[1m"
#define BES_CLR_DIM "\033[2m"
#define BES_CLR_ULINE "\033[4m"
#define BES_CLR_BLINK "\033[5m"
#define BES_CLR_REV "\033[7m"
#define BES_CLR_HIDE "\033[8m"

/// Resetters
#define BES_CLR_RESET "\033[0m"
#define BES_CLR_RESET_BOLD "\033[21m"
#define BES_CLR_RESET_DIM "\033[22m"
#define BES_CLR_RESET_ULINE "\033[24m"
#define BES_CLR_RESET_BLINK "\033[25m"
#define BES_CLR_RESET_REV "\033[27m"
#define BES_CLR_RESET_HIDE "\033[28m"

/// Foreground
#define BES_CLR_DEFAULT "\033[39m"

#define BES_CLR_BLACK "\033[30m"
#define BES_CLR_RED "\033[31m"
#define BES_CLR_GREEN "\033[32m"
#define BES_CLR_YELLOW "\033[33m"
#define BES_CLR_BLUE "\033[34m"
#define BES_CLR_MAGENTA "\033[35m"
#define BES_CLR_CYAN "\033[36m"

#define BES_CLR_LGREY "\033[37m"
#define BES_CLR_DGREY "\033[90m"

#define BES_CLR_LRED "\033[91m"
#define BES_CLR_LGREEN "\033[92m"
#define BES_CLR_LYELLOW "\033[93m"
#define BES_CLR_LBLUE "\033[94m"
#define BES_CLR_LMAGENTA "\033[95m"
#define BES_CLR_LCYAN "\033[96m"
#define BES_CLR_WHITE "\033[97m"

/// Background
#define BES_CLR_BG_DEFAULT "\033[49m"

#define BES_CLR_BG_BLACK "\033[40m"
#define BES_CLR_BG_RED "\033[41m"
#define BES_CLR_BG_GREEN "\033[42m"
#define BES_CLR_BG_YELLOW "\033[43m"
#define BES_CLR_BG_BLUE "\033[44m"
#define BES_CLR_BG_MAGENTA "\033[45m"
#define BES_CLR_BG_CYAN "\033[46m"

#define BES_CLR_BG_LGREY "\033[47m"
#define BES_CLR_BG_DGREY "\033[100m"

#define BES_CLR_BG_LRED "\033[101m"
#define BES_CLR_BG_LGREEN "\033[102m"
#define BES_CLR_BG_LYELLOW "\033[103m"
#define BES_CLR_BG_LBLUE "\033[104m"
#define BES_CLR_BG_LMAGENTA "\033[105m"
#define BES_CLR_BG_LCYAN "\033[106m"
#define BES_CLR_BG_LWHITE "\033[107m"

/// Presets
#define BES_CLR_DEBUG BES_CLR_LGREY
#define BES_CLR_WARNING BES_CLR_YELLOW
#define BES_CLR_ERROR BES_CLR_BG_BLACK << BES_CLR_LRED
#define BES_CLR_CRITICAL BES_CLR_BG_RED << BES_CLR_LYELLOW

