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
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
struct SimulationData {
    SimulationData () { }
    std::shared_ptr<std::map<typename std::string, std::shared_ptr<Joint>>> joints;
    std::shared_ptr<std::map<typename std::string, std::shared_ptr<LineRenderer>>> linkages;
    std::shared_ptr<std::map<typename std::string, std::shared_ptr<Constraint>>> constraints;

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

        joints = std::make_shared<std::map<typename std::string, std::shared_ptr<Joint>>>();
        linkages = std::make_shared<std::map<typename std::string, std::shared_ptr<LineRenderer>>>();
        constraints = std::make_shared<std::map<typename std::string, std::shared_ptr<Constraint>>>();

        joints->clear();
        linkages->clear();
        constraints->clear();

        std::string line;

        std::cout << "starting loading ..." << std::endl;

        while (std::getline(jointsFile, line)) {
            std::vector<std::string> data = splitString(line, ' ');
            if(data.size() <= 1) continue;
            if(data[0] == "s") {
                if(data.size() != 6) throw std::invalid_argument("wrong amount of arguments at: " + line);

                std::cout << "static" << std::endl;
                int x = std::stoi(data[1]);
                int y = std::stoi(data[2]);
                bool is_ground = (bool)std::stoi(data[3]);
                bool is_input = (bool)std::stoi(data[4]);
                joints->insert({data[5], std::make_shared<Joint>(Vec2(x,y), is_ground, is_input, data[5])});
            } else if(data[0] == "d") {
                if(data.size() != 6) throw std::invalid_argument("wrong amount of arguments at: " + line);
                std::cout << "dynamic" << std::endl;
                joints->insert({ data[5], std::make_shared<DynamicJoint>((*joints)[data[1]], (*joints)[data[3]], getOperator(data[2][0]), std::stoi(data[4]), data[5] )});
            } else if(data[0] == "//") {
                // its a comment
                continue;
            } 
            else {
                throw std::invalid_argument("invalid joint type at line: " + line);
            }
        }

        std::cout << "loaded joints" << std::endl;
        std::cout << "starting loading constraints... " << std::endl;

        while (std::getline(constraintsFile, line)) {
            std::cout << line << std::endl;
            if(line.empty()) continue;
            if(line.size() < 3) throw std::invalid_argument("not enough arguments at line: " + line); 
            std::string command = line.substr(0,3);
            std::string attrs = line.substr(4, line.length()-1);
            std::vector<std::string> data = splitString(attrs, ' ');
            if(command == "dis") {
                if(data.size() != 4) throw std::invalid_argument("wrong amount of arguments " + std::to_string(data.size()) + " at: " + line);
                constraints->insert({data[3], std::make_shared<DistanceConstraint>((*joints)[data[0]], (*joints)[data[1]], (*joints)[data[2]], data[3])});
            } else if(command == "lin") {
                if(data.size() != 4) throw std::invalid_argument("wrong amount of arguments at: " + line);
                constraints->insert({data[3], std::make_shared<LinearConstraint>((*joints)[data[0]], (*joints)[data[1]], (*joints)[data[2]], data[3])});
            } else if(command == "dir") {
                if(data.size() != 4) throw std::invalid_argument("wrong amount of arguments at: " + line);
                constraints->insert({data[3], std::make_shared<DirectionConstraint>((*joints)[data[0]], (*joints)[data[1]], (*joints)[data[2]], data[3])});
            } else if(command == "lim") {
                if(data.size() != 4) throw std::invalid_argument("wrong amount of arguments at: " + line);
                constraints->insert({data[3], std::make_shared<LimitConstraint>((*joints)[data[0]], (*joints)[data[1]], (*joints)[data[2]], data[3])});
            } else {
                throw std::invalid_argument("invalid constraint type at line: " + line);
            }
        }

        std::cout << "loaded constraints" << std::endl;

        while (std::getline(linkagesFile, line)) {
            std::vector<std::string> data = splitString(line, ' ');
            if(data.size() != 3) throw std::invalid_argument("wrong amount of arguments at: " + line);
            linkages->insert({data[2],std::make_shared<LineRenderer>((*joints)[data[0]], (*joints)[data[1]], 10)});
        }

        std::cout << "loaded linkages" << std::endl;

    }

    void saveToFile() {
        std::ofstream jointsFile("save/joints.txt", std::ios_base::out);
        std::ofstream constraintsFile("save/constraints.txt", std::ios_base::out);
        std::ofstream linkagesFile("save/linkages.txt", std::ios_base::out);


        for (auto& [key, joint] : *joints) {
            if(auto d = std::dynamic_pointer_cast<DynamicJoint>(joint); d) {
                std::string j1 = findJointkey(std::dynamic_pointer_cast<Joint>(d->joint1), joints);
                std::string j2 = findJointkey(std::dynamic_pointer_cast<Joint>(d->joint2), joints);
                
                jointsFile << "d " << j1 << " " << FromOperator(d->oper) << " " << j2 << " " << " " << d->is_visible << " " <<  key << std::endl;
            }else {
                jointsFile << "s " << (int)joint->x << " " << (int)joint->y << " " << joint->is_ground << " " << joint->is_visible << " " << key << std::endl;
            }
        }
        
        std::cout << "saved joints" << std::endl;
 
        for(auto& [key, linkage] : (*linkages)) {
            std::string j1 = findJointkey(std::dynamic_pointer_cast<Joint>(linkage->j1), joints);
            std::string j2 = findJointkey(std::dynamic_pointer_cast<Joint>(linkage->j2), joints);

            linkagesFile << j1 << " " << j2 << " " << key << std::endl;
        }

        std::cout << "saved linkages" << std::endl;

        for(auto& [key, constraint] : (*constraints)) {
            if(auto d = std::dynamic_pointer_cast<DistanceConstraint>(constraint); d) {
                std::string j1 = findJointkey(std::dynamic_pointer_cast<Joint>(d->j1), joints);
                std::string j2 = findJointkey(std::dynamic_pointer_cast<Joint>(d->j2), joints);
                std::string td = findJointkey(std::dynamic_pointer_cast<Joint>(d->target_distance), joints);
                constraintsFile << "dis " << j1 << " " << j2 << " " << td << " " << key << std::endl;

            } else if(auto d = std::dynamic_pointer_cast<LinearConstraint>(constraint); d) {
                std::string j1 = findJointkey(std::dynamic_pointer_cast<Joint>(d->joint), joints);
                std::string pos = findJointkey(std::dynamic_pointer_cast<Joint>(d->position), joints);
                std::string dir = findJointkey(std::dynamic_pointer_cast<Joint>(d->sharedDirection), joints);
                constraintsFile << "lin " << j1 << " " << pos << " " << dir << " " << key << std::endl;

            } else if(auto d = std::dynamic_pointer_cast<DirectionConstraint>(constraint); d) {
                std::string j1 = findJointkey(std::dynamic_pointer_cast<Joint>(d->j1), joints);
                std::string j2 = findJointkey(std::dynamic_pointer_cast<Joint>(d->j2), joints);
                std::string dir = findJointkey(std::dynamic_pointer_cast<Joint>(d->direction), joints);
                constraintsFile << "dir " << j1 << " " << j2 << " " << dir << " " << key << std::endl;

            } else if(auto d = std::dynamic_pointer_cast<LimitConstraint>(constraint); d) {
                std::string j1 = findJointkey(std::dynamic_pointer_cast<Joint>(d->joint), joints);
                std::string lim1 = findJointkey(std::dynamic_pointer_cast<Joint>(d->lim1), joints);
                std::string lim2 = findJointkey(std::dynamic_pointer_cast<Joint>(d->lim2), joints);
                constraintsFile << "lim " << j1 << " " << lim1 << " " << lim2 << " " << key << std::endl;
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

    static std::string findJointkey(std::shared_ptr<Joint> joint, std::shared_ptr<std::map<typename std::string, std::shared_ptr<Joint>>> joints) {
        for (const auto& [key, value] : *joints) {
            if(value == joint) {return key;}
        }
        return "";
    }
};