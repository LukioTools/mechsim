#pragma once

#include "Window.hpp"
#include "WindowStatus.hpp"
struct Element : Window {
    virtual WindowStatus update() override = 0;
    virtual bool firstKeyboardUpdate(char key) override = 0;
    virtual bool keyboardUpdate(char key) override = 0;

};