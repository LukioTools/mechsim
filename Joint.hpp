#pragma once

#include "Vec2.hpp"
#include "tui/TUI.hpp"
#include <iostream>
#include <memory>
#include <ostream>

#define grounded -1

struct Joint : Vec2 {
        bool is_ground = false;
        bool is_input = false;

        int variable_index = grounded;

        Vec2 start_position;

        Joint(Vec2 _current_position, bool ground, bool input) : is_ground(ground), is_input(input) {
            x = _current_position.x;
            y = _current_position.y;
        }

        void move(Vec2 delta) {
            x += delta.x;
            y += delta.y;
        }

        void setPosition(Vec2 p) {
            x = p.x;
            y = p.y;
        }
};