#pragma once

#include "./Element.hpp"
#include "../Vec2.hpp"
#include "./TUI.hpp"
#include <memory>
#include <ostream>
#include <string>
#include "../Joint.hpp"
#include "WindowStatus.hpp"
#include "../constraints/Constraint.hpp"
#include "../constraints/DistanceConstraint.hpp"
#include "../constraints/DirectionConstraint.hpp"
#include "../constraints/LimitConstraint.hpp"
#include "../constraints/LinearConstraint.hpp"
#include "../solver.hpp"
#include "../SimulationData.hpp"
#include "../DynamicJoint.hpp"


struct ConstraintsTab : Element {

    enum STAGES {
        CONSTRAINTS,
        NEW_CONSTRAINT,
        SELECT_CONSTRAINT,
        CONSTRAINT_SELECTED,
        EDITING_JOINT,
        EXIT
    };
    
    bool inputInterfaceOpen = false;
    std::shared_ptr<std::map<std::string,std::shared_ptr<Constraint>>> constraints;
    std::shared_ptr<std::map<std::string,std::shared_ptr<Joint>>> joints;
    std::shared_ptr<Solver> solver;

    std::function<void(std::shared_ptr<Joint>)> setSelectedVec2;

    std::string selectedConstraint;

    std::string inputData;
    STAGES stage = CONSTRAINTS;

    Vec2 screenSize;
    ConstraintsTab(Vec2 size, std::shared_ptr<std::map<std::string, std::shared_ptr<Constraint>>> cst, std::shared_ptr<std::map<std::string, std::shared_ptr<Joint>>> js, std::shared_ptr<Solver> s) : screenSize(size), constraints(cst), joints(js), solver(s) {
            
    }

    void selectConstraint(int j) {
        inputData.clear();
        inputInterfaceOpen = false;
        selectedConstraint = j;
        stage = CONSTRAINT_SELECTED;
    }

    void printConstraint(std::shared_ptr<Constraint> constraint, int idx = -1) {
        std::string i;
        if(idx >= 0)
            i = std::to_string(idx) + " | ";
        else 
            i = "";
        if(auto d = std::dynamic_pointer_cast<DistanceConstraint>(constraint); d) {
            std::string j1 = SimulationData::findJointkey(d->j1, joints);
            std::string j2 = SimulationData::findJointkey(d->j2, joints);
            std::cout << i << "distance  | first joint: " << j1 << " second joint: " << j2 << " distance: " << (*d->target_distance) << std::endl;
        
        } else if(auto d = std::dynamic_pointer_cast<LinearConstraint>(constraint); d) {
            std::string j1 = SimulationData::findJointkey(d->joint, joints);
            std::cout << i << "linear    | joint: " << j1 << " position: " << (*d->position) << " direction: " << (*d->sharedDirection) << std::endl;
        
        } else if(auto d = std::dynamic_pointer_cast<DirectionConstraint>(constraint); d) {
            std::string j1 = SimulationData::findJointkey(d->j1, joints);
            std::string j2 = SimulationData::findJointkey(d->j2, joints);
            std::cout << i << "direction | first joint: " << j1 << " second joint: " << j2 << " direction: " << (*d->direction) << std::endl;
        
        } else if(auto d = std::dynamic_pointer_cast<LimitConstraint>(constraint); d) {
            std::string j1 = SimulationData::findJointkey(d->joint, joints);
            std::cout << i << "limit     | joint " << j1 << " min: " << (*d->lim1) << "max: " << (*d->lim2) << std::endl;
        }
    }

    void printJoint(std::shared_ptr<Joint> joint, int startIndex) {
            std::cout << "j" << startIndex - 1 << ", position: " << joint << ", is ground: " << joint->is_ground << ", is input: " << joint->is_visible << std::flush;
    }

    void printJoints() {
        int startIndex = 2;
        for (auto& [key, joint] : *joints) {
            TUI::setCursor(Vec2(0,startIndex));
            std::cout << startIndex - 1 << " | position: " << *joint << ", is ground: " << joint->is_ground << ", is input: " << joint->is_visible << " " << key << ", used :" << !joint.unique() << std::flush;
            startIndex++;
        }
    }

    void deleteConstraint() {
        constraints->erase(selectedConstraint);
        goBack();
    }

    void createConstraint() {

    }

    void goBack() {
        switch (stage) { 
            case NEW_CONSTRAINT: 
                stage = CONSTRAINTS; 
                break;
            case SELECT_CONSTRAINT: 
                stage = CONSTRAINTS; 
                inputData.clear();
                break;
            case CONSTRAINT_SELECTED:
                stage = CONSTRAINTS;
                selectedConstraint = -1;
                break;
            case CONSTRAINTS:
                stage = EXIT;
                break;
            case EDITING_JOINT:
                stage = CONSTRAINT_SELECTED;
                break;
            default:
                break;
        }
    }
    
    int printConstraints() {
        int startIndex = 2;
        for (auto& [key, constraint] : (*constraints)) {
            TUI::setCursor(Vec2(0,startIndex));
            printConstraint(constraint, startIndex-1);
            startIndex++;
        }
        return startIndex;
    }

    void setVec2ToJoint() {
        TUI::setCursor(Vec2(0, 100));
        std::string idx = inputData;
        inputData.clear();
        setSelectedVec2((*joints)[idx]);
        goBack();
    }

    WindowStatus update() override {

        switch (stage) {
            case CONSTRAINTS: {
                TUI::setCursor(Vec2(0, 1));
                std::cout << "n: new constraint    s: select constraint " << constraints->size() << std::flush;
                int i = printConstraints();
                break;
            }
            case NEW_CONSTRAINT:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "new constraint: " << inputData << std::flush;
                TUI::resetColor();
                printJoints();
                break;
            case SELECT_CONSTRAINT:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select constraint index: " << inputData << std::flush;
                TUI::resetColor();
                printConstraints();
                break;
            case CONSTRAINT_SELECTED:
                TUI::setCursor(Vec2(0,1));
                std::cout << (*constraints)[selectedConstraint]->editor() << std::flush;
                TUI::setCursor(Vec2(0,2));
                printConstraint((*constraints)[selectedConstraint]);
                break;
            case EXIT:
                stage = CONSTRAINTS;
                return WindowStatus(false, nullptr, false);
            case EDITING_JOINT:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select joint index: " << inputData << std::flush;
                TUI::resetColor();
                printJoints();
                break;
            default:
                break;
        }

        return WindowStatus(true, nullptr, false);

    }

    // return true if more is expected
    bool firstKeyboardUpdate(char key) override {

        if(key == '\e') {
            goBack();
            return false;
        }

        switch (stage) {
            case CONSTRAINTS: {
                switch (key) {
                    case 'n':
                        stage = NEW_CONSTRAINT;
                        return true;
                    case 's':
                        stage = SELECT_CONSTRAINT; 
                        return true;
                    default: 
                        return false;
                }
            }
            case CONSTRAINT_SELECTED: {
            if(auto d = std::dynamic_pointer_cast<DistanceConstraint>((*constraints)[selectedConstraint]); d) {
                switch (key) {
                    case '1': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->j1 = v;
                        };
                        return true;
                    }
                    case '2': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->j2 = v;
                        };
                        return true;
                    }
                    case 'd': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->target_distance = v;

                        };
                        return true;
                    }
                    case 'r': {
                        deleteConstraint();
                        return false;
                    }
                }
            } else if(auto d = std::dynamic_pointer_cast<LinearConstraint>((*constraints)[selectedConstraint]); d) {
                switch (key) {
                    case '1': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->joint = v;
                        };
                        return true;
                    }
                    case 'p': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->position = v;
                        };
                        return true;
                    }
                    case 'd': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->sharedDirection = v;
                        };
                        return true;
                    }
                    case 'r': {
                        deleteConstraint();
                        return false;
                    }
                }
            } else if(auto d = std::dynamic_pointer_cast<DirectionConstraint>((*constraints)[selectedConstraint]); d) {
                switch (key) {
                    case '1': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->j1 = v;
                        };
                        return true;
                    }
                    case 'p': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->j2 = v;
                        };
                        return true;
                    }
                    case 'd': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->direction = v;
                        };
                        return true;
                    }
                    case 'r': {
                        deleteConstraint();
                        return false;
                    }
                }
            } else if(auto d = std::dynamic_pointer_cast<LimitConstraint>((*constraints)[selectedConstraint]); d) {
                switch (key) {
                    case 'j': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->joint = v;
                        };
                        return true;
                    }
                    case '1': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->lim1 = v;
                        };
                        return true;
                    }
                    case '2': {
                        stage = EDITING_JOINT;
                        setSelectedVec2 = [d](std::shared_ptr<Joint> v) {
                            d->lim2 = v;
                        };
                        return true;
                    }
                    case 'r': {
                        deleteConstraint();
                        return false;
                    }
                }
            } else {
                return false;
            }
            }
            default:
                break;
        }

        return false;
    }

    //return true if more
    bool keyboardUpdate(char key) override {

        if(key == '\e') {
            goBack();
            return false;
        }

        switch (stage) {
            case SELECT_CONSTRAINT:
                if(key == '\r') {
                    selectConstraint(std::stoi(inputData) - 1);
                    return false;
                }

                if(key == '\b') {
                    inputData.pop_back();
                    return true;
                }
                
                inputData += key;
                break;
            case EDITING_JOINT:
                if(key == '\r') {
                    setVec2ToJoint();
                    return false;
                }

                if(key == '\b') {
                    inputData.pop_back();
                    return true;
                }
                
                inputData += key;
            default:
                break;
        }

        return true;
    }
};