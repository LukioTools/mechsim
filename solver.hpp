#pragma once

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "./constraints/Constraint.hpp"
#include "Vec2.hpp"

void printMatrix(const Eigen::MatrixXd& M)
{
    for (int i = 0; i < M.rows(); ++i)
    {
        for (int j = 0; j < M.cols(); ++j)
        {
            std::ostringstream ss;
            ss << std::setw(12) << std::setprecision(5)
               << std::fixed << std::showpos << M(i, j);

            std::string s = ss.str();

            if (s[0] == '+')
                s[0] = ' ';

            std::cout << s << ' ';
        }
        std::cout << '\n';
    }
}

struct Solver {
    std::shared_ptr<std::map<std::string,std::shared_ptr<Constraint>>> constraints;
    std::vector<std::shared_ptr<Joint>> joints;
    int max_variable_index;

    void get_joints() {
        joints.clear();
        for (auto& [key, constraint] : (*constraints)) {
            for (std::shared_ptr<Joint> joint : constraint->get_joints()) {
                if(std::find(joints.begin(), joints.end(), joint) != joints.end()) continue;
                joints.push_back(joint);
            }
        }
    }

    Eigen::MatrixXd get_jacobian() {
        Eigen::MatrixXd Jacobian(constraints->size(), max_variable_index*2);
        Jacobian.setZero();
        
        int i = 0;
        for (auto& [key, constr] : *constraints) {
            Eigen::RowVectorXd row(2*max_variable_index);
            constr->fill_jacobian_row(row);
            Jacobian.row(i) = row;
            i++;
        }
        return Jacobian;
    }

    Eigen::VectorXd get_errors() {
        Eigen::VectorXd errors(constraints->size());
        int i = 0;
        for (auto& constraint : *constraints) {
            errors[i] = constraint.second->error();
            i++;
        }
        return  errors;
    }

    void fix_variable_indices() {
        for (const auto& [key, constraint] : (*constraints)) {
            for (std::shared_ptr<Joint> joint : constraint->get_joints()) {
                joint->variable_index = grounded;
            }
        }
    }

    /**
    @return return hihest variable index
    */
    int assign_variable_indexes() {
        fix_variable_indices();

        int idx = 0;
        for (auto& [key, constraint] : (*constraints)) {
            for (std::shared_ptr<Joint> joint : constraint->get_joints()) {
                if(joint->is_ground) {joint->variable_index = grounded; continue;}
                if(joint->variable_index != grounded ) {continue;} 
                    joint->variable_index = idx;
                    idx++;                    
            }
        }

        return idx;
    }

    bool checkTolerances(Eigen::VectorXd inputs, double tolerance) {
        for (double i : inputs) {
            if(abs(i) >= tolerance) return false;
        }
        return true;
    }

    void setStartPositions() {
        for (auto& joint : joints) {
            joint->start_position = *joint;
        }
    }

    void resetPositions() {
        for (auto& joint : joints) {
            joint->x = joint->start_position.x;
            joint->y = joint->start_position.y;
        }
    }

    void updateConstraints(std::shared_ptr<std::map<std::string,std::shared_ptr<Constraint>>> ctrs) {
        constraints = ctrs;
        max_variable_index = assign_variable_indexes();
        get_joints();
    }

    bool solve(int max_iterations = 50) {

        Eigen::IOFormat format(
            6,                  // Precision
            Eigen::DontAlignCols,
            " ",                // Coefficient separator
            "\n",               // Row separator
            "",                 // Row prefix
            "",                 // Row suffix
            "",                 // Matrix prefix
            ""                  // Matrix suffix
        );

        setStartPositions();

        for (int iteration = 0; iteration < max_iterations; iteration++) {
            Eigen::MatrixXd J = get_jacobian();
            Eigen::VectorXd errors = get_errors();

            /*
            std::cout << "\n\nerror: \n" << '\n';
            printMatrix(errors);

            std::cout << "derivate: \n";
            printMatrix(J);
            */

            if(checkTolerances(errors, 0.01)) {
                return true;
            }

            Eigen::MatrixXd A = J*J.transpose();

            Eigen::VectorXd delta_q = -J.transpose() * A.ldlt().solve(errors);

            double step_length = delta_q.norm();

            for (std::shared_ptr<Joint> joint : joints) {
                if(joint->variable_index == grounded) continue;
                
                int k = 2 * joint->variable_index;
                Vec2 delta = Vec2(delta_q(k), delta_q(k+1));
                joint->move(delta);
            }
        }
        resetPositions();
        return false;
    }
};