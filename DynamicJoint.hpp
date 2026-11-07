#pragma once

#include "Joint.hpp"
#include "Vec2.hpp"
#include "tui/TUI.hpp"
#include <iostream>
#include <memory>
#include <ostream>

#define grounded -1

struct DynamicJoint : Joint {

    enum Operator{
        ADDITION,
        MULTIPLICATION,
        SUBSTRACTION,
        DIVITION
    };

    int variable_index = grounded;
    std::shared_ptr<Vec2> joint1;
    std::shared_ptr<Vec2> joint2;
    Operator oper;

    DynamicJoint(std::shared_ptr<Vec2> j1, std::shared_ptr<Vec2> j2, Operator op) : Joint(Vec2(0,0), 1, 1), joint1(j1), joint2(j2), oper(op) { }

    void update() {
        switch (oper) {
            case ADDITION: {
                x = joint1->x + joint2->x;
                y = joint1->y + joint2->y;
                break;
            } 
            case MULTIPLICATION: {
                x = joint1->x * joint2->x;
                y = joint2->y * joint2->y;
                break;
            }
            case SUBSTRACTION: {
                x = joint1->x - joint2->x;
                y = joint1->y - joint2->y;
                break;
            }
            case DIVITION: {
                x = joint1->x / joint2->x;
                y = joint1->y / joint2->y;
            }
        }
    }
};