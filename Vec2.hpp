#pragma once

#include <cmath>
#include <stdexcept>
#include <ostream>

struct vec2 {
    float x,y = 0;
    float operator[] (size_t idx) {
        if(idx == 0) return x;
        else if (idx == 1) return y;
        throw std::out_of_range("only x and y in vec2");
    }

    vec2 operator- (vec2 v2) {
        return vec2(x-v2.x,y-v2.y);
    }

    vec2 operator+ (vec2 v2) {
        return vec2(x+v2.x,y+v2.y);
    }

    vec2 operator/ (float i) {
        return vec2(x/i,y/i);
    }

    vec2 operator* (float i) {
        return vec2(x*i,y*i);
    }


    vec2 unit() {
        float dist = std::sqrt(x*x+y*y);
        if(dist == 0) return vec2(0,0);
        return vec2(x/dist, y/dist);
    }

    vec2 normal() {
        return vec2(x,-y);
    }

    float length() {
        return std::sqrt(x*x+y*y);
    }

    friend std::ostream& operator<< (std::ostream& stream, const vec2& vec){
            return stream << "(" << (int)vec.x << ", " << (int)vec.y << ")";
    }

    vec2() {}
    vec2(float _x, float _y) : x(_x), y(_y) {}

    static float distance(vec2 v1, vec2 v2) {
        return std::sqrt((v2.x-v1.x)*(v2.x-v1.x)+(v2.y-v1.y)*(v2.y-v1.y));
    }

    static float dot(vec2 v1, vec2 v2) {
        return v1.x*v2.x + v1.y*v2.y;
    }
};