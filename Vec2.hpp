#pragma once

#include <cmath>
#include <stdexcept>
#include <ostream>

struct Vec2 {
    float x,y = 0;
    float operator[] (size_t idx) {
        if(idx == 0) return x;
        else if (idx == 1) return y;
        throw std::out_of_range("only x and y in vec2");
    }

    Vec2 operator- (Vec2 v2) {
        return Vec2(x-v2.x,y-v2.y);
    }

    Vec2 operator+ (Vec2 v2) {
        return Vec2(x+v2.x,y+v2.y);
    }

    Vec2 operator/ (float i) {
        return Vec2(x/i,y/i);
    }

    Vec2 operator* (float i) {
        return Vec2(x*i,y*i);
    }


    Vec2 unit() {
        float dist = std::sqrt(x*x+y*y);
        if(dist == 0) return Vec2(0,0);
        return Vec2(x/dist, y/dist);
    }

    Vec2 normal() {
        return Vec2(x,-y);
    }

    float length() {
        return std::sqrt(x*x+y*y);
    }

    friend std::ostream& operator<< (std::ostream& stream, const Vec2& vec){
            return stream << "(" << (int)vec.x << ", " << (int)vec.y << ")";
    }

    Vec2() {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}

    static float distance(Vec2 v1, Vec2 v2) {
        return std::sqrt((v2.x-v1.x)*(v2.x-v1.x)+(v2.y-v1.y)*(v2.y-v1.y));
    }

    static float dot(Vec2 v1, Vec2 v2) {
        return v1.x*v2.x + v1.y*v2.y;
    }
};