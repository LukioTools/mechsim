#pragma once

#include "Constraint.hpp"
#include <cmath>
#include <iostream>

class DirectionConstraint : public Constraint {
    public:
        std::shared_ptr<Joint> j1;
        std::shared_ptr<Joint> j2;
        std::shared_ptr<Vec2> direction_point;

        DirectionConstraint(std::shared_ptr<Joint> j1, std::shared_ptr<Joint> j2, std::shared_ptr<Joint> dir) : j1(j1), j2(j2), direction_point(dir) { }

        float error() override { 
            Vec2 direction = (*direction_point)-(*j1);

            float Ax = j2->x-j1->x;
            float Ay = j2->y-j1->y;
            
            float cross = Ax*direction.y-Ay*direction.x;
            float dot = Ax*direction.x+Ay*direction.y;

            return atan2(cross, dot);
        }

        void fill_jacobian_row(Eigen::Ref<Eigen::RowVectorXd> row) const override {
            
            row.setZero();

            float den = (j1->x-j2->x)*(j1->x-j2->x)+(j1->y-j2->y)*(j1->y-j2->y);

            if(den < 1e-12) return;

            if(j1->variable_index >= 0) {
                int k = 2*j1->variable_index;

                row[k] = (j1->y-j2->y)/den;
                row[k+1] = -(j1->x-j2->x)/den;
            }

            if(j2->variable_index >= 0) {
                int k = 2*j2->variable_index;
                row[k] = -(j1->y-j2->y)/den;
                row[k+1] = (j1->x-j2->x)/den;
            }
        }   

        std::vector<std::shared_ptr<Joint>> get_joints() const override {
            std::vector<std::shared_ptr<Joint>> joints;
            joints.push_back(j1);
            joints.push_back(j2);
            return joints;
        }

        std::string editor() override {
            return "1: joint 1     2: joint 2     d: direction     r: delete";
        }

    private:

};