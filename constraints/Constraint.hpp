#pragma once

#include <eigen3/Eigen/Core>
#include <string>
#include "../Joint.hpp"

class Constraint {
    public:
        std::string comment;
        Constraint(std::string c) : comment(c) { };
        virtual float error() = 0;
        virtual void fill_jacobian_row(
            Eigen::Ref<Eigen::RowVectorXd> row
        ) const = 0;
        virtual std::vector<std::shared_ptr<Joint>> get_joints() const = 0;
        virtual std::string editor() = 0;
};