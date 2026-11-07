#pragma once

#include "DynamicJoint.hpp"
#include "Joint.hpp"
#include "LineRenderer.hpp"
#include "Vec2.hpp"
#include "constraints/Constraint.hpp"
#include "constraints/DirectionConstraint.hpp"
#include "constraints/DistanceConstraint.hpp"
#include "constraints/LimitConstraint.hpp"
#include "constraints/LinearConstraint.hpp"
#include <fstream>
#include <ios>
#include <memory>
#include <string>
#include <vector>
struct SimulationData {
    SimulationData () { }
    std::shared_ptr<std::vector<std::shared_ptr<Joint>>> joints;
    std::shared_ptr<std::vector<std::shared_ptr<LineRenderer>>> linkages;
    std::shared_ptr<std::vector<std::shared_ptr<Constraint>>> constraints;

    static DynamicJoint::Operator getOperator(char oper) {
        switch (oper) {
            case '+': return DynamicJoint::ADDITION;
            case '-': return DynamicJoint::SUBSTRACTION;
            case '*': return DynamicJoint::MULTIPLICATION;
            case '/': return DynamicJoint::DIVITION;
        }
        return DynamicJoint::ADDITION;
    }

    static char FromOperator(DynamicJoint::Operator oper) {
        switch (oper) {
            case DynamicJoint::ADDITION: return '+';
            case DynamicJoint::SUBSTRACTION: return '-';
            case DynamicJoint::MULTIPLICATION: return '*';
            case DynamicJoint::DIVITION: return '/';
        }
        return '+';
    }

    void loadFromFiles() {
        std::ifstream jointsFile("save/joints.txt", std::ios_base::in);
        std::ifstream constraintsFile("save/constraints.txt", std::ios_base::in);
        std::ifstream linkagesFile("save/linkages.txt", std::ios_base::in);

        joints = std::make_shared<std::vector<std::shared_ptr<Joint>>>();
        linkages = std::make_shared<std::vector<std::shared_ptr<LineRenderer>>>();
        constraints = std::make_shared<std::vector<std::shared_ptr<Constraint>>>();

        joints->clear();
        linkages->clear();
        constraints->clear();

        std::string line;

        std::cout << "starting loading ..." << std::endl;

        while (std::getline(jointsFile, line)) {
            std::vector<std::string> data = splitString(line, ' ');
            if(data[0] == "s") {
                std::cout << "static" << std::endl;
                int x = std::stoi(data[1]);
                int y = std::stoi(data[2]);
                bool is_ground = (bool)std::stoi(data[3]);
                bool is_input = (bool)std::stoi(data[4]);
                joints->push_back(std::make_shared<Joint>(Vec2(x,y), is_ground, is_input));
            } else if(data[0] == "d") {
                std::cout << "dynamic" << std::endl;
                joints->push_back(std::make_shared<DynamicJoint>((*joints)[std::stoi(data[1])], (*joints)[std::stoi(data[3])], getOperator(data[2][0])));
            }
        }

        std::cout << "loaded joints" << std::endl;

        while (std::getline(constraintsFile, line)) {
            std::cout << line << std::endl;
            std::string command = line.substr(0,3);
            std::string attrs = line.substr(4, line.length()-1);
            std::vector<std::string> data = splitString(attrs, ' ');
            if(command == "dis") {
                constraints->push_back(std::make_shared<DistanceConstraint>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], (*joints)[std::stoi(data[2])]));
            } else if(command == "lin") {
                constraints->push_back(std::make_shared<LinearConstraint>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], (*joints)[std::stoi(data[2])]));
            } else if(command == "dir") {
                constraints->push_back(std::make_shared<DirectionConstraint>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], (*joints)[std::stoi(data[2])]));
            } else if(command == "lim") {
                constraints->push_back(std::make_shared<LimitConstraint>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], (*joints)[std::stoi(data[2])]));
            }
        }

        std::cout << "loaded constraints" << std::endl;


        while (std::getline(linkagesFile, line)) {
            std::vector<std::string> data = splitString(line, ' ');
            linkages->push_back(std::make_shared<LineRenderer>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], 10));
        }

        std::cout << "loaded linkages" << std::endl;

    }

    void saveToFile() {
        std::ofstream jointsFile("save/joints.txt", std::ios_base::out);
        std::ofstream constraintsFile("save/constraints.txt", std::ios_base::out);
        std::ofstream linkagesFile("save/linkages.txt", std::ios_base::out);


        for (size_t i = 0; i < joints->size(); ++i) {
            if(auto d = std::dynamic_pointer_cast<DynamicJoint>((*joints)[i]); d) {
                int j1 = find(joints->begin(), joints->end(), d->joint1) - joints->begin();
                int j2 = find(joints->begin(), joints->end(), d->joint2) - joints->begin();
                jointsFile << "d " << j1 << " " << FromOperator(d->oper) << " " << j2 << std::endl;
            }else {
                std::shared_ptr<Joint> joint = (*joints)[i];
                jointsFile << "s " << (int)joint->x << " " << (int)joint->y << " " << joint->is_ground << " " << joint->is_input << std::endl;
            }
        }
        
        std::cout << "saved joints" << std::endl;
 
        for(std::shared_ptr<LineRenderer> linkage : (*linkages)) {
            int j1 = find(joints->begin(), joints->end(), linkage->j1) - joints->begin();
            int j2 = find(joints->begin(), joints->end(), linkage->j2) - joints->begin();
            linkagesFile << j1 << " " << j2 << std::endl;
        }

        std::cout << "saved linkages" << std::endl;

        for(std::shared_ptr<Constraint> constraint : (*constraints)) {
            if(auto d = std::dynamic_pointer_cast<DistanceConstraint>(constraint); d) {
                int j1 = find(joints->begin(), joints->end(), d->j1) - (joints)->begin();
                int j2 = find(joints->begin(), joints->end(), d->j2) - joints->begin();
                int td = find(joints->begin(), joints->end(), d->target_distance) - joints->begin();
                constraintsFile << "dis " << j1 << " " << j2 << " " << td << std::endl;
            } else if(auto d = std::dynamic_pointer_cast<LinearConstraint>(constraint); d) {
                int j1 = find(joints->begin(), joints->end(), d->joint) - joints->begin();
                int pos = find(joints->begin(), joints->end(), d->position) - joints->begin();
                int dir = find(joints->begin(), joints->end(), d->direction) - joints->begin();
                constraintsFile << "lin " << j1 << " " << pos << " " << dir << std::endl;
            } else if(auto d = std::dynamic_pointer_cast<DirectionConstraint>(constraint); d) {
                int j1 = find(joints->begin(), joints->end(), d->j1) - joints->begin();
                int j2 = find(joints->begin(), joints->end(), d->j2) - joints->begin();
                int dir = find(joints->begin(), joints->end(), d->direction) - joints->begin();
                constraintsFile << "dir " << j1 << " " << j2 << " " << dir << std::endl;
            } else if(auto d = std::dynamic_pointer_cast<LimitConstraint>(constraint); d) {
                int j1 = find(joints->begin(), joints->end(), d->joint) - joints->begin();
                int lim1 = find(joints->begin(), joints->end(), d->lim1) - joints->begin();
                int lim2 = find(joints->begin(), joints->end(), d->lim2) - joints->begin();
                constraintsFile << "lim " << j1 << " " << lim1 << " " << lim2 << std::endl;
            }
        }

        std::cout << "saved constraints" << std::endl;

    }

    static std::vector<std::string> splitString(const std::string& line, char c) {
        std::vector<std::string> resultList;
        std::size_t last = 0;

        for (std::size_t i = 0; i < line.size(); i++) {
            if (line[i] == c) {
                resultList.push_back(line.substr(last, i - last));
                last = i + 1;
            }
        }

        resultList.push_back(line.substr(last));

        return resultList;
    }
};