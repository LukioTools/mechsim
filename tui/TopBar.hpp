#pragma once

#include "./Element.hpp"
#include "../Vec2.hpp"
#include "./TUI.hpp"
#include "WindowStatus.hpp"

struct TopBar : Element {
    
    enum WINDOWS {
        NONE,
        JOINTS,
        LINKAGES,
        CONSTRAINTS
    };

    WINDOWS windowToOpen = NONE;

    std::shared_ptr<Window> jointsWindow;
    std::shared_ptr<Window> linkagesWindow;
    std::shared_ptr<Window> constraintsWindow;

    bool isPaused = false;
    
    TopBar(std::shared_ptr<Window> jsWindow, std::shared_ptr<Window> lsWindow, std::shared_ptr<Window> cstrWindow) : jointsWindow(jsWindow), linkagesWindow(lsWindow), constraintsWindow(cstrWindow) { }

    
    WindowStatus update() override {
        TUI::setCursor(Vec2(0, 0));
        if(!isPaused)
            std::cout << "1: joints     2: linkages     3: constraints     p: pause simulation" << std::endl;
        else
            std::cout << "1: joints     2: linkages     3: constraints     p: continue simulation" << std::endl;

        switch (windowToOpen) {
            case JOINTS:
                windowToOpen = NONE;
                return WindowStatus(true, jointsWindow, isPaused);
            case LINKAGES:
                windowToOpen = NONE;
                return WindowStatus(true, linkagesWindow, isPaused);
            case CONSTRAINTS:
                windowToOpen = NONE;
                return WindowStatus(true, constraintsWindow, isPaused);
            default:
                windowToOpen = NONE;
                return WindowStatus(true, nullptr, isPaused);
                break;
        }
        
    }

    bool firstKeyboardUpdate(char key) override {
        if(key == '1') windowToOpen = JOINTS; 
        if(key == '2') windowToOpen = LINKAGES; 
        if(key == '3') windowToOpen = CONSTRAINTS; 
        if(key == 'p') isPaused = !isPaused;
        return false;
    }

    bool keyboardUpdate(char key) override {return false; }
};