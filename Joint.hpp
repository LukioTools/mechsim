#pragma once

#include "Vec2.hpp"
#include "BallRenderer.hpp"
#include "tui/TUI.hpp"
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#define grounded -1

struct Joint : Vec2 {
        bool is_ground = false;
        bool is_visible = false;

        int variable_index = grounded;

        Vec2 start_position;

        BallRenderer ballRenderer;

        std::string comment;

        Joint(Vec2 _current_position, bool ground, bool visible, std::string c) : is_ground(ground), is_visible(visible), ballRenderer(this), comment(c) {
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

        virtual ~Joint() = default;
};