#pragma once

#include "Vec2.hpp"

#define grounded -1

struct Joint : vec2 {
        bool is_ground = false;
        bool is_input = false;

        int variable_index = grounded;

        vec2 start_position;

        Joint(vec2 _current_position, bool ground, bool input) : is_ground(ground), is_input(input) {
            x = _current_position.x;
            y = _current_position.y;
        }

        void move(vec2 delta) {
            x += delta.x;
            y += delta.y;
        }
};