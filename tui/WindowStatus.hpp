#pragma once

#include <memory>
struct Window;

struct WindowStatus {


    bool isActive;
    std::shared_ptr<Window> toBeOpened;
    bool stopSimulation;

    WindowStatus(bool a, std::shared_ptr<Window> w, bool ss) : isActive(a), toBeOpened(w), stopSimulation(ss) { }
};