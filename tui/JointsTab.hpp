#pragma once

#include "./Element.hpp"
#include "../Vec2.hpp"
#include "./TUI.hpp"
#include <memory>
#include <ostream>
#include <string>
#include "../Joint.hpp"
#include "WindowStatus.hpp"
#include "../SimulationData.hpp"

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
    std::shared_ptr<std::map<std::string, std::shared_ptr<Joint>>> joints;

    std::string selectedJoint;

    std::string inputData;
    STAGES stage = JOINTS;

    Vec2 screenSize;
    JointsTab(Vec2 size, std::shared_ptr<std::map<std::string, std::shared_ptr<Joint>>> js) : screenSize(size), joints(js) {
            
        keyboard['s'] = [this]() {
            if(stage == JOINTS){
                stage = STAGES::SELECT_JOINT; return true;
            }
            return false;
        };
        keyboard['p'] = [this]() {
            if(stage == JOINT_SELECTED) {
                stage = STAGES::EDIT_POSITION; return true;
            }
            return false;
        };
        keyboard['g'] = [this]() {
            if(stage == JOINT_SELECTED)
                (*joints)[selectedJoint]->is_ground = !(*joints)[selectedJoint]->is_ground;
            return false;
        };
        keyboard['i'] = [this]() {
            (*joints)[selectedJoint]->is_visible = !(*joints)[selectedJoint]->is_visible;
            return false;
        };
        keyboard['d'] = [this]() {
            if (stage == JOINT_SELECTED) {
                joints->erase(selectedJoint);
                goBack();
            }
            return false;
        };
        keyboard['n'] = [this]() {
            if(stage == JOINTS) {
                stage = NEW_JOINT;
                return true;
            }
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

    void createJoint() {
        std::vector<std::string> data = SimulationData::splitString(inputData, ' ');
        if(data[0] == "s") {
            std::cout << "static" << std::endl;
            int x = std::stoi(data[1]);
            int y = std::stoi(data[2]);
            bool is_ground = (bool)std::stoi(data[3]);
            bool is_input = (bool)std::stoi(data[4]);
            joints->insert({data[5], std::make_shared<Joint>(Vec2(x,y), is_ground, is_input, data[5])});
        } else if(data[0] == "d") {
            std::cout << "dynamic" << std::endl;
            joints->insert({data[5], std::make_shared<DynamicJoint>((*joints)[data[1]], (*joints)[data[3]], SimulationData::getOperator(data[2][0]), std::stoi(data[4]), data[5])});
        }
        inputData.clear();
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
        for (auto& [key, joint] : *joints) {
            TUI::setCursor(Vec2(0,startIndex));
            std::cout << key << " | position: " << *joint << ", is ground: " << joint->is_ground << ", is input: " << joint->is_visible << " " << key << ", used :" << !joint.unique() << std::flush;
            startIndex++;
        }
    }

    WindowStatus update() override {
        switch (stage) {
            case JOINTS:
                TUI::setCursor(Vec2(0, 1));
                std::cout << "n: new joint    s: select joint" << std::flush;
                printJoints();
                break;
            case NEW_JOINT:
                break;
            case SELECT_JOINT:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select joint index: " << inputData << std::flush;
                TUI::resetColor();
                printJoints();
                break;
            case JOINT_SELECTED:
                TUI::setCursor(Vec2(0,1));
                std::cout << "p: edit position     g: toggle ground     i: toggle input     d: delete" << std::flush;
                TUI::setCursor(Vec2(0,2));
                std::cout << "joint: " << "position: " << *(*joints)[selectedJoint] << ", is ground: " << (*joints)[selectedJoint]->is_ground << ", is input: " << (*joints)[selectedJoint]->is_visible << " used: " << !(*joints)[selectedJoint].unique() << std::flush;
                break;
            case EDIT_POSITION:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "position: " << inputData << std::flush;
                TUI::resetColor();
                
                TUI::setCursor(Vec2(0,2));
                std::cout << "joint: " << "position: " << (*joints)[selectedJoint] << ", is ground: " << (*joints)[selectedJoint]->is_ground << ", is input: " << (*joints)[selectedJoint]->is_visible << std::flush;
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