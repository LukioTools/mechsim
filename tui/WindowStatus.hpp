#pragma once

#include <memory>
struct Window;

struct WindowStatus {


    bool isActive;
    std::shared_ptr<Window> toBeOpened;
    bool stopSimulation;
    bool save;

    WindowStatus(bool a, std::shared_ptr<Window> w, bool ss, bool s = false) : isActive(a), toBeOpened(w), stopSimulation(ss), save(s) { }
};