#pragma once

#include "./Element.hpp"
#include "../Vec2.hpp"
#include "./TUI.hpp"
#include <algorithm>
#include <memory>
#include <ostream>
#include <string>
#include "../Joint.hpp"
#include "WindowStatus.hpp"
#include "../LineRenderer.hpp"
#include "../SimulationData.hpp"

struct linkagesTab : Element {

    enum STAGES {
        LINKAGES,
        NEW_LINK,
        SELECT_LINK,
        LINK_SELECTED,
        EXIT
    };
    
    bool inputInterfaceOpen = false;
    std::shared_ptr<std::map<std::string, std::shared_ptr<LineRenderer>>> linkages;
    std::shared_ptr<std::map<std::string, std::shared_ptr<Joint>>> joints;

    std::string selectedLinkage;

    std::string inputData;
    STAGES stage = LINKAGES;

    Vec2 screenSize;
    linkagesTab(Vec2 size, std::shared_ptr<std::map<std::string, std::shared_ptr<LineRenderer>>> lr, std::shared_ptr<std::map<std::string, std::shared_ptr<Joint>>> js) : screenSize(size), linkages(lr), joints(js) {
            
        keyboard['s'] = [this]() {
            if(stage == LINKAGES)
                stage = STAGES::SELECT_LINK; return true;
            return false;
        };
        keyboard['d'] = [this]() {
            if(stage == LINK_SELECTED) {
                linkages->erase(selectedLinkage);
                goBack();
            }
            return false;
        };
        keyboard['n'] = [this]() {
            if(stage == LINKAGES) {
                stage = NEW_LINK;
                return true;
            }
            return false;
        };
        keyboard['\e'] = [this]() {goBack(); return false;};
    }

    void selectLinkage(int j) {
        inputData.clear();
        inputInterfaceOpen = false;
        selectedLinkage = j;
        stage = LINK_SELECTED;
    }

    void createLink() {
        auto attrs = SimulationData::splitString(inputData, ' ');
        if(attrs.size() != 2) {
            goBack();
        }

        std::string j1 = attrs[0];
        std::string j2 = attrs[1];

        linkages->insert({attrs[2],std::make_shared<LineRenderer>((*joints)[j1],(*joints)[j2], 10)});
        selectLinkage(linkages->size()-1);
    }

    void goBack() {
        switch (stage) {
            case NEW_LINK: 
                stage = LINKAGES;
                inputData.clear(); 
                break;
            case SELECT_LINK: 
                stage = LINKAGES; 
                inputData.clear();
                break;
            case LINK_SELECTED:
                stage = LINKAGES;
                selectedLinkage = -1;
                break;
            case LINKAGES:
                stage = EXIT;
            default:
                break;
        }
    }
    
    void printLinkages() {
        int startIndex = 2;
        for (auto& link : *linkages) {
            TUI::setCursor(Vec2(0,startIndex));
            std::string j1 = SimulationData::findJointkey(link.second->j1, joints);
            std::string j2 = SimulationData::findJointkey(link.second->j2, joints);
            std::cout << startIndex - 1 << " | " << "first joint: " << j1 << " second joint: " << j2 << std::flush;
            startIndex++;
        }
    }

    void printJoints() {
        int startIndex = 2;
        for (auto& [key, joint] : *joints) {
            TUI::setCursor(Vec2(0,startIndex));
            std::cout << startIndex - 1 << " | position: " << *joint << ", is ground: " << joint->is_ground << ", is input: " << joint->is_visible << ", used :" << !joint.unique() << std::flush;
            startIndex++;
        }
    }

    WindowStatus update() override {
        switch (stage) {
            case LINKAGES:
                TUI::setCursor(Vec2(0, 1));
                std::cout << "n: new link    s: select link" << std::flush;
                printLinkages();
                break;
            case NEW_LINK:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select joint index: " << inputData << std::flush;
                TUI::resetColor();
                printJoints();
                break;
            case SELECT_LINK:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select joint index: " << inputData << std::flush;
                TUI::resetColor();
                printLinkages();
                break;
            case LINK_SELECTED:
                TUI::setCursor(Vec2(0,1));
                std::cout << "d: delete" << std::flush;
                TUI::setCursor(Vec2(0,2));
                std::cout << "joint: " << std::flush;
                break;
            case EXIT:
                stage = LINKAGES;
                return WindowStatus(false, nullptr, false);
            default:
                break;
        }

        return WindowStatus(true, nullptr, false);

    }

    // return true if more is expected
    bool firstKeyboardUpdate(char key) override {
        if(keyboard.find(key) != keyboard.end())
            return keyboard[key]();
        return false;
    }

    //return true if more
    bool keyboardUpdate(char key) override {

        switch (stage) {
            case SELECT_LINK:
                if(key == '\r') {
                    selectLinkage(std::stoi(inputData) - 1);
                    return false;
                }

                if(key == '\b') {
                    inputData.pop_back();
                    return true;
                }
                
                inputData += key;
                break;
            case NEW_LINK:
                if(key == '\r') {
                    createLink();
                    return false;
                }

                if(key == '\b') {
                    inputData.pop_back();
                    return true;
                }
                
                inputData += key;
                break;
            break;
            default:
                break;
        }

        if(key == '\e') {
            goBack();
            return false;
        }

        return true;
    }
};