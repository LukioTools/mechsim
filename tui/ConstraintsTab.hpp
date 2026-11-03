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
#include "../SimulationData.hpp"
#include "../solver.hpp"


struct ConstraintsTab : Element {

    enum STAGES {
        CONSTRAINTS,
        NEW_Constraint,
        SELECT_CONSTRAINT,
        CONSTRAINT_SELECTED,
        VECTOR_INPUT,
        EXIT
    };
    
    bool inputInterfaceOpen = false;
    std::shared_ptr<std::vector<std::shared_ptr<Constraint>>> constraints;
    std::shared_ptr<std::vector<std::shared_ptr<Joint>>> joints;
    std::shared_ptr<Solver> solver;

    std::shared_ptr<Constraint> selectedConstraint;

    std::string inputData;
    STAGES stage = CONSTRAINTS;

    Vec2 screenSize;
    ConstraintsTab(Vec2 size, std::shared_ptr<std::vector<std::shared_ptr<Constraint>>> cst, std::shared_ptr<std::vector<std::shared_ptr<Joint>>> js, std::shared_ptr<Solver> s) : screenSize(size), constraints(cst), joints(js), solver(s) {
            
    }

    void selectConstraint(int j) {
        inputData.clear();
        inputInterfaceOpen = false;
        selectedConstraint = (*constraints)[j];
        stage = CONSTRAINT_SELECTED;
    }

    void printConstraint(std::shared_ptr<Constraint> constraint, int idx = -1) {
        std::string i;
        if(idx >= 0)
            i = std::to_string(idx) + " | ";
        else 
            i = "";
        if(auto d = std::dynamic_pointer_cast<DistanceConstraint>(constraint); d) {
            int j1 = find(joints->begin(), joints->end(), d->j1) - joints->begin();
            int j2 = find(joints->begin(), joints->end(), d->j2) - joints->begin();
            std::cout << i << "distance  | first joint: " << j1 << " second joint: " << j2 << " distance: " << d->target_distance->length() << std::endl;
        } else if(auto d = std::dynamic_pointer_cast<LinearConstraint>(constraint); d) {
            int j1 = find(joints->begin(), joints->end(), d->joint) - joints->begin();
            std::cout << i << "linear    | joint: " << j1 << " position: " << (*d->position) << " direction: " << (*d->direction) << std::endl;
        } else if(auto d = std::dynamic_pointer_cast<DirectionConstraint>(constraint); d) {
            int j1 = find(joints->begin(), joints->end(), d->j1) - joints->begin();
            int j2 = find(joints->begin(), joints->end(), d->j2) - joints->begin();
            std::cout << i << "direction | first joint: " << j1 << " second joint: " << j2 << " direction: " << (*d->direction_point) << std::endl;
        } else if(auto d = std::dynamic_pointer_cast<LimitConstraint>(constraint); d) {
            int j1 = find(joints->begin(), joints->end(), d->joint) - joints->begin();
            std::cout << i << "limit     | joint " << j1 << " min: " << (*d->lim1) << "max: " << (*d->lim2) << std::endl;
        }
    }

    void printJoint(std::shared_ptr<Joint> joint, int startIndex) {
            std::cout << "j" << startIndex - 1 << ", position: " << joint << ", is ground: " << joint->is_ground << ", is input: " << joint->is_input << std::flush;
    }

    void goBack() {
        switch (stage) { 
            case NEW_Constraint: 
                stage = CONSTRAINTS; 
                break;
            case SELECT_CONSTRAINT: 
                stage = CONSTRAINTS; 
                inputData.clear();
                break;
            case CONSTRAINT_SELECTED:
                stage = CONSTRAINTS;
                selectedConstraint = nullptr;
                break;
            case CONSTRAINTS:
                stage = EXIT;
            default:
                break;
        }
    }
    
    int printConstraints() {
        int startIndex = 2;
        for (auto& constraint : (*constraints)) {
            TUI::setCursor(Vec2(0,startIndex));
            printConstraint(constraint, startIndex-1);
            startIndex++;
        }
        return startIndex;
    }

    WindowStatus update() override {

        switch (stage) {
            case CONSTRAINTS: {
                TUI::setCursor(Vec2(0, 1));
                std::cout << "n: new constraint    s: select constraint" << std::flush;
                int i = printConstraints();
                break;
            }
            case NEW_Constraint:
                break;
            case SELECT_CONSTRAINT:
                TUI::setCursor(Vec2(0,1));
                TUI::setFGColor(255, 0, 0);
                std::cout << "select joint index: " << inputData << std::flush;
                TUI::resetColor();
                printConstraints();
                break;
            case CONSTRAINT_SELECTED:
                TUI::setCursor(Vec2(0,1));
                std::cout << selectedConstraint->editor() << std::flush;
                TUI::setCursor(Vec2(0,2));
                printConstraint(selectedConstraint);
                break;
            case EXIT:
                stage = CONSTRAINTS;
                return WindowStatus(false, nullptr, false);
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
                        return false;
                    case 's':
                        stage = STAGES::SELECT_CONSTRAINT; 
                        return true;
                    default: 
                        return false;
                }
                break;
            }
            case CONSTRAINT_SELECTED: {

            }
            default:
                break;
        }

        return false;
    }

    //return true if more
    bool keyboardUpdate(char key) override {

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