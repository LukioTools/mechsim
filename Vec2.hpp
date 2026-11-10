#pragma once

#include <cmath>
#include <stdexcept>
#include <ostream>

struct Vec2 {
    double x = 0;
    double y = 0;
    double operator[] (size_t idx) {
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

    Vec2 operator/ (double i) {
        return Vec2(x/i,y/i);
    }

    Vec2 operator* (double i) {
        return Vec2(x*i,y*i);
    }


    Vec2 unit() {
        double dist = std::sqrt(x*x+y*y);
        if(dist == 0) return Vec2(0,0);
        return Vec2(x/dist, y/dist);
    }

    Vec2 toUnit() {
        double dist = std::sqrt(x*x+y*y);
        if(dist == 0) return Vec2(0,0);
        x = x/dist; 
        y = y/dist;
    }

    Vec2 normal() {
        return Vec2(x,-y);
    }

    double length() {
        return std::sqrt(x*x+y*y);
    }

    friend std::ostream& operator<< (std::ostream& stream, const Vec2& vec){
            return stream << "(" << (int)vec.x << ", " << (int)vec.y << ")";
    }

    Vec2() {}
    Vec2(double _x, double _y) : x(_x), y(_y) {}
    virtual ~Vec2() = default;

    static double distance(Vec2 v1, Vec2 v2) {
        return std::sqrt((v2.x-v1.x)*(v2.x-v1.x)+(v2.y-v1.y)*(v2.y-v1.y));
    }

    static double dot(Vec2 v1, Vec2 v2) {
        return v1.x*v2.x + v1.y*v2.y;
    }
};