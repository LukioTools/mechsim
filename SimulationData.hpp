#pragma once

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

        while (std::getline(jointsFile, line)) {
            std::vector<std::string> data = splitString(line, ' ');
            int x = std::stoi(data[0]);
            int y = std::stoi(data[1]);
            bool is_ground = (bool)std::stoi(data[2]);
            bool is_input = (bool)std::stoi(data[3]);
            joints->push_back(std::make_shared<Joint>(vec2(x,y), is_ground, is_input));
        }

        while (std::getline(constraintsFile, line)) {
            std::string command = line.substr(0,3);
            std::string attrs = line.substr(4, line.length()-1);
            std::vector<std::string> data = splitString(attrs, ' ');
            if(command == "dis") {
                constraints->push_back(std::make_shared<DistanceConstraint>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], std::stoi(data[2])));
            } else if(command == "lin") {
                constraints->push_back(std::make_shared<LinearConstraint>((*joints)[std::stoi(data[0])], vec2(std::stoi(data[1]), std::stoi(data[2])), vec2(std::stoi(data[3]), std::stoi(data[4]))));
            } else if(command == "dir") {
                constraints->push_back(std::make_shared<DirectionConstraint>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], vec2(std::stoi(data[2]), std::stoi(data[3]))));
            } else if(command == "lim") {
                constraints->push_back(std::make_shared<LimitConstraint>((*joints)[std::stoi(data[0])], vec2(std::stoi(data[1]), std::stoi(data[2])), vec2(std::stoi(data[3]), std::stoi(data[4]))));
            }
        }

        while (std::getline(linkagesFile, line)) {
            std::vector<std::string> data = splitString(line, ' ');
            linkages->push_back(std::make_shared<LineRenderer>((*joints)[std::stoi(data[0])], (*joints)[std::stoi(data[1])], 10));
        }

    }

    void saveToFile() {
        std::ofstream jointsFile("save/joints.txt", std::ios_base::out);
        std::ofstream constraintsFile("save/constraints.txt", std::ios_base::out);
        std::ofstream linkagesFile("save/linkages.txt", std::ios_base::out);


        for (size_t i = 1; i < joints->size(); ++i) {
            std::shared_ptr<Joint> joint = (*joints)[i];
            jointsFile << joint->x << " " << joint->y << " " << joint->is_ground << " " << joint->is_input << std::endl;
        }
 
        for(std::shared_ptr<LineRenderer> linkage : (*linkages)) {
            int j1 = find(joints->begin(), joints->end(), linkage->j1) - joints->begin();
            int j2 = find(joints->begin(), joints->end(), linkage->j2) - joints->begin();
            linkagesFile << j1 << " " << j2 << std::endl;
        }

        for(std::shared_ptr<Constraint> constraint : (*constraints)) {
            if(typeid(constraint) == typeid(DistanceConstraint)) {
                std::shared_ptr<DistanceConstraint> d = std::dynamic_pointer_cast<DistanceConstraint>(constraint);
                int j1 = find(joints->begin(), joints->end(), d->j1) - joints->begin();
                int j2 = find(joints->begin(), joints->end(), d->j2) - joints->begin();
                constraintsFile << "dis " << j1 << " " << j2 << " " << d->target_distance << std::endl;
            } else if(typeid(constraint) == typeid(LinearConstraint)) {
                std::shared_ptr<LinearConstraint> d = std::dynamic_pointer_cast<LinearConstraint>(constraint);
                int j1 = find(joints->begin(), joints->end(), d->joint) - joints->begin();
                constraintsFile << "lin " << j1 << " " << d->position.x << " " << d->position.y << " " << d->direction.x << " " << d->direction.y << std::endl;
            } else if(typeid(constraint) == typeid(DirectionConstraint)) {
                std::shared_ptr<DirectionConstraint> d = std::dynamic_pointer_cast<DirectionConstraint>(constraint);
                int j1 = find(joints->begin(), joints->end(), d->j1) - joints->begin();
                int j2 = find(joints->begin(), joints->end(), d->j2) - joints->begin();
                constraintsFile << "dir " << j1 << " " << j2 << " " << d->direction.x << " " << d->direction.x << std::endl;
            } else if(typeid(constraint) == typeid(LimitConstraint)) {
                std::shared_ptr<LimitConstraint> d = std::dynamic_pointer_cast<LimitConstraint>(constraint);
                int j1 = find(joints->begin(), joints->end(), d->joint) - joints->begin();
                constraintsFile << "lim " << j1 << " " << d->lim1.x << " " << d->lim1.y << " " << d->lim2.x << " " << d->lim2.y << std::endl;
            }
            
        }
        
    }

    std::vector<std::string> splitString(const std::string& line, char c) {
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