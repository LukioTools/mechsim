#pragma once

#include <memory>
#include <string>
#include "Constraint.hpp"

class DistanceConstraint : public Constraint {
    public:
        std::shared_ptr<Joint> j1;
        std::shared_ptr<Joint> j2;
        std::shared_ptr<Vec2> target_distance;

        DistanceConstraint(std::shared_ptr<Joint> joint1, std::shared_ptr<Joint> joint2, std::shared_ptr<Joint> distance, std::string c)
            : j1(joint1), j2(joint2), target_distance(distance), Constraint(c) {}

        float error() override {
            float dx = j2->x - j1->x;
            float dy = j2->y - j1->y;

            return sqrt(dx*dx + dy*dy)-target_distance->length();
        }

        void fill_jacobian_row(Eigen::Ref<Eigen::RowVectorXd> row) const override {
            row.setZero();

            double dx = j2->x - j1->x;
            double dy = j2->y - j1->y;
            double r = sqrt(dx*dx+dy*dy);

            if (j1->variable_index >= 0) {
                int k = 2 * j1->variable_index;
                row[k] = -dx/r;
                row[k + 1] = -dy/r;
            }

            if (j2->variable_index >= 0) {
                int k = 2 * j2->variable_index;
                row [k] = dx/r;
                row [k+1] = dy/r;
            }
        }

        std::vector<std::shared_ptr<Joint>> get_joints() const override {
            std::vector<std::shared_ptr<Joint>> joints;
            joints.push_back(j1);
            joints.push_back(j2);
            return joints;
        }

        std::string editor() override {
            return "1: joint 1     2: joint 2     d: distance     r: delete";
        }

    private:
};