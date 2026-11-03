#pragma once

#include <algorithm>
#include <functional>
#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "Window.hpp"
#include "../Vec2.hpp"
#include "WindowStatus.hpp"

struct WindowManager {
    
    std::vector<std::shared_ptr<Window>> openWindows;
    std::shared_ptr<Window> activeWindow;

    Vec2 screenSize;

    bool moreToCome = false;
    
    WindowManager(Vec2 ss) : screenSize(ss) { }

    void openWindow(std::shared_ptr<Window> window) {
        openWindows.push_back(window);
    }

    void selectWindow(std::shared_ptr<Window> window) {
        activeWindow = window;
    }

    WindowStatus update() {
        bool stopSimulation = false;
        for (std::shared_ptr<Window> window : openWindows) {
            if(window == activeWindow) continue;
            WindowStatus status = window->update();
            if(status.stopSimulation) stopSimulation = true;
        }

        WindowStatus status = activeWindow->update();

        if(status.stopSimulation) stopSimulation = true;

        if(status.isActive && status.toBeOpened != nullptr) {
            openWindows.push_back(status.toBeOpened);
            activeWindow = openWindows.back();

        } else if(!status.isActive && status.toBeOpened != nullptr) {
            openWindows.pop_back();
            openWindows.push_back(status.toBeOpened);
            activeWindow = openWindows.back();
            
        } else if(!status.isActive && status.toBeOpened == nullptr) {
            openWindows.pop_back();
            activeWindow = openWindows.back();
        }

        status.stopSimulation = stopSimulation;
        return status;
    }

    void keyboardUpdate(std::string inputs) {
        for (char key : inputs) {
            if(activeWindow == nullptr) {
                return; 
            }
    
            if(!moreToCome) moreToCome = activeWindow->firstKeyboardUpdate(key);
            else moreToCome = activeWindow->keyboardUpdate(key);
        }
    }
};