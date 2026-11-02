#pragma once

#include <functional>
#include <unordered_map>

#include "WindowStatus.hpp"

struct Window {

    std::unordered_map<char, std::function<bool()>> keyboard;

    virtual WindowStatus update() = 0;

    // return true if more is expected
    virtual bool firstKeyboardUpdate(char key) = 0;

    //return true if end
    virtual bool keyboardUpdate(char key) = 0;
};