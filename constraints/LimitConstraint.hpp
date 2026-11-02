#pragma once

#include "Constraint.hpp"
#include <cmath>
#include <glm/common.hpp>

class LimitConstraint : public Constraint {
    public:
        std::shared_ptr<Joint> joint;
        vec2 lim1;
        vec2 lim2; // unit vector

        LimitConstraint(std::shared_ptr<Joint> j, vec2 l1, vec2 l2) : joint(j), lim1(l1), lim2(l2) { }

        float error() override {
            if(joint->x >= lim1.x && joint->x <= lim2.x && joint->y >= lim1.y && joint->y <= lim2.y) return 0;
            vec2 posOnLims;
            posOnLims.x = glm::clamp(joint->x, lim1.x, lim2.x);
            posOnLims.y = glm::clamp(joint->y, lim1.y, lim2.y);
        
            vec2 d = posOnLims - *joint;

            return d.length();
        
        }

        void fill_jacobian_row(Eigen::Ref<Eigen::RowVectorXd> row) const override {
            row.setZero();

            if(joint->x >= lim1.x && joint->x <= lim2.x && joint->y >= lim1.y && joint->y <= lim2.y) return;
            
            vec2 posOnLims;
            posOnLims.x = glm::clamp(joint->x, lim1.x, lim2.x);
            posOnLims.y = glm::clamp(joint->y, lim1.y, lim2.y);
        
            vec2 d = posOnLims - *joint;
            float r = d.length();

            if (joint->variable_index >= 0) {
                int k = 2 * joint->variable_index;
                row[k] = -d.x/r;
                row[k + 1] = -d.y/r;
            }

        }   

        std::vector<std::shared_ptr<Joint>> get_joints() const override {
            std::vector<std::shared_ptr<Joint>> joints;
            joints.push_back(joint);
            return joints;
        }

        std::string editor() override {
            return "j: joint     1: limit 1     2: limit 2";
        }

    private:

};