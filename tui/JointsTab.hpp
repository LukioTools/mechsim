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

struct JointsTab : Element {

    enum STAGES {
        JOINTS,
        NEW_JOINT,
        SELECT_JOINT,
        JOINT_SELECTED,
        EDIT_POSITION,
        EXIT
    };
    
    bool inputInterfaceOpen = false;
    std::shared_ptr<std::vector<std::shared_ptr<Joint>>> joints;

    int selectedJoint;

    std::string inputData;
    STAGES stage = JOINTS;

    vec2 screenSize;
    JointsTab(vec2 size, std::shared_ptr<std::vector<std::shared_ptr<Joint>>> js) : screenSize(size), joints(js) {
            
        keyboard['s'] = [this]() {
            if(stage == JOINTS)
                stage = STAGES::SELECT_JOINT; return true;
        };
        keyboard['p'] = [this]() {
            if(stage == JOINT_SELECTED)
                stage = STAGES::EDIT_POSITION; return true;
        };
        keyboard['g'] = [this]() {
            if(stage == JOINT_SELECTED)
                (*joints)[selectedJoint]->is_ground = !(*joints)[selectedJoint]->is_ground;
            return false;
        };
        keyboard['i'] = [this]() {
            (*joints)[selectedJoint]->is_input = !(*joints)[selectedJoint]->is_input;
            return false;
        };
        keyboard['d'] = [this]() {
            joints->erase(joints->begin()+selectedJoint);
            goBack();
            return false;
        };
        keyboard['\e'] = [this]() {goBack(); return false;};
    }

    void selectJoint(int j) {
        inputData.clear();
        inputInterfaceOpen = false;
        selectedJoint = j;
        stage = JOINT_SELECTED;
    }

    void changePosition(std::string input ) {
        int x = std::stoi(input.substr(0, input.find(',')));
        int y = std::stoi(input.substr(input.find(',')+1, input.length()-1));
        (*joints)[selectedJoint]->x = x;
        (*joints)[selectedJoint]->y = y;
        goBack();
    }

    void goBack() {
        switch (stage) {
            case NEW_JOINT: 
                stage = JOINTS; 
                break;
            case SELECT_JOINT: 
                stage = JOINTS; 
                inputData.clear();
                break;
            case EDIT_POSITION: 
                stage = JOINT_SELECTED;
                inputData.clear();
                break;
            case JOINT_SELECTED:
                stage = JOINTS;
                selectedJoint = -1;
                break;
            case JOINTS:
                stage = EXIT;
            default:
                break;
        }
    }
    
    void printJoints() {
        int startIndex = 2;
        for (auto& joint : *joints) {
            TUI::setCursor(vec2(0,startIndex));
            std::cout << startIndex - 1 << " | position: " << *joint << ", is ground: " << joint->is_ground << ", is input: " << joint->is_input << ", used :" << !joint.unique() << std::flush;
            startIndex++;
        }
    }

    WindowStatus update() override {
        switch (stage) {
            case JOINTS:
                TUI::setCursor(vec2(0, 1));
                std::cout << "n: new joint    s: select joint" << std::flush;
                printJoints();
                break;
            case NEW_JOINT:
                break;
            case SELECT_JOINT:
                TUI::setCursor(vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select joint index: " << inputData << std::flush;
                TUI::resetColor();
                printJoints();
                break;
            case JOINT_SELECTED:
                TUI::setCursor(vec2(0,1));
                std::cout << "p: edit position     g: toggle ground     i: toggle input     d: delete" << std::flush;
                TUI::setCursor(vec2(0,2));
                std::cout << "joint: " << "position: " << *(*joints)[selectedJoint] << ", is ground: " << (*joints)[selectedJoint]->is_ground << ", is input: " << (*joints)[selectedJoint]->is_input << " used: " << !(*joints)[selectedJoint].unique() << std::flush;
                break;
            case EDIT_POSITION:
                TUI::setCursor(vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "position: " << inputData << std::flush;
                TUI::resetColor();
                
                TUI::setCursor(vec2(0,2));
                std::cout << "joint: " << "position: " << (*joints)[selectedJoint] << ", is ground: " << (*joints)[selectedJoint]->is_ground << ", is input: " << (*joints)[selectedJoint]->is_input << std::flush;
                return WindowStatus(true, nullptr, true);
                
                break;
            case EXIT:
                stage = JOINTS;
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
            case SELECT_JOINT:
                if(key == '\r') {
                    selectJoint(std::stoi(inputData) - 1);
                    return false;
                }

                if(key == '\b') {
                    inputData.pop_back();
                    return true;
                }
                
                inputData += key;
                break;
            case EDIT_POSITION:
                if(key == '\r') {
                        changePosition(inputData);
                        return false;
                    }

                    if(key == '\b') {
                        inputData.pop_back();
                        return true;
                    }
                    
                    inputData += key;
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