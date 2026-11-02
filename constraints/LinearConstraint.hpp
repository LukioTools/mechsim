#pragma once

#include "Constraint.hpp"

class LinearConstraint : public Constraint {
    public:
        std::shared_ptr<Joint> joint;
        vec2 position;
        vec2 direction; // unit vector

        LinearConstraint(std::shared_ptr<Joint> j, vec2 p, vec2 dir) : joint(j), position(p), direction(dir) { }

        float error() override {
            return -direction.y*(joint->x - position.x) + direction.x*(joint->y - position.y);
        }

        void fill_jacobian_row(Eigen::Ref<Eigen::RowVectorXd> row) const override {
            row.setZero();

            if(joint->variable_index >= 0) {
                int k = 2*joint->variable_index;
                row[k] = -direction.y;
                row[k+1] = direction.x;
            }
        }   

        std::vector<std::shared_ptr<Joint>> get_joints() const override {
            std::vector<std::shared_ptr<Joint>> joints;
            joints.push_back(joint);
            return joints;
        }

        std::string editor() override {
            return "1: joint     p: position     d: direction";
        }

    private:

};