#pragma once

#include <iostream>
#include <ostream>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>
#include "../Vec2.hpp"

#define COLOR_FG(r,g,b) "\e[38;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'
#define COLOR_BG(r,g,b) "\e[48;2;" + std::to_string(r) + ';' + std::to_string(g) +  ';' + std::to_string(b) + 'm'


#define COLOR_C_FG(c) "\e[38;2;" + std::to_string(c.r) + ';' + std::to_string(c.g) +  ';' + std::to_string(c.b) + 'm'
#define COLOR_C_BG(c) "\e[48;2;" + std::to_string(c.r) + ';' + std::to_string(c.g) +  ';' + std::to_string(c.b) + 'm'


// ANSI escape code for resetting colors
#define RESET_COLOR "\033[0m"

#define CLEAR_TERMINAL() std::cout << "\e[2J" << std::flush;
#define CLEAR_TERMINAL_CLOG() clog << "\033c" << std::endl;

#define ESC "\e"
#define MOVE_TO_COORDINATES(x, y) "\033[" << (y) + 1 << ";" << (x) + 1 << "H"

#define ENABLE_ALT_BUF() std::cout <<  "\e[?1049h"
#define DISABLE_ALT_BUF() std::cout <<  "\e[?1049l"

#define DISABLE_WARP() std::cout <<  "\e[?7l"
#define ENABLE_WARP() std::cout <<  "\e[?7h"

#define HIDE_CURSOR() std::cout << "\033[?25l";
#define SHOW_CURSOR() std::cout << "\033[?25h" << std::flush;

struct TUI {
    static inline void clear() {
        CLEAR_TERMINAL();
    }

    static inline void setCursor(Vec2 pos) {
        std::cout << MOVE_TO_COORDINATES(pos.x, pos.y);
    }

    static inline void setFGColor(int r, int g, int b) {
        std::cout << COLOR_BG(r, g, b);
    }

    static inline void setChar(char ch) {
        std::cout << ch;
    }

    static inline void printString(std::string string) {
        std::cout << string;
    }

    static inline void resetColor() {
        std::cout << RESET_COLOR << std::endl;
    }

    static Vec2 ScreenSize(){
        winsize w;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        return Vec2(w.ws_col, w.ws_row);
    }
};