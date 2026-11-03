// Your First C++ Program
#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/LU>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float2x2.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>
#include <eigen3/Eigen/Dense>

#include "SimulationData.hpp"
#include "Vec2.hpp"
#include "constraints/LimitConstraint.hpp"
#include "solver.hpp"
#include "constraints/DirectionConstraint.hpp"
#include "constraints/DistanceConstraint.hpp"
#include "constraints/LinearConstraint.hpp"
#include "./Shader/Shader.hpp"
#include "Line.hpp"
#include "LineRenderer.hpp"
#include "./tui/TUI.hpp"
#include "./tui/TopBar.hpp"
#include "tui/ConstraintsTab.hpp"
#include "tui/JointsTab.hpp"
#include "tui/LinkagesTab.hpp"
#include "tui/WindowManager.hpp"
#include "tui/WindowStatus.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}  


std::vector<Line> lines = {
    {{54,23}, {200, 200}, 20.0f},
    {{100, 100}, {300, 100}, 10.0f}
};

int window_init(GLFWwindow** window) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(800, 600, "Mech", NULL, NULL);
    if(window == NULL) {
        glfwTerminate();
        return -1;
    } 

    glfwMakeContextCurrent(*window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(*window, framebuffer_size_callback);  

    return 0;
}

Vec2 cursorPos(0,0);
int windowHeight = 0;
std::string userInputs;
bool IsLeftMouseButtonPressed;
double cursorScaling = 1;

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    cursorPos.x = xpos;
    cursorPos.y = windowHeight - ypos;
}

void char_callback(GLFWwindow* window, unsigned int codepoint) {
    userInputs += static_cast<char>(codepoint);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cursorScaling = std::clamp(cursorScaling + yoffset/5, 1., 100.) ;    
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        IsLeftMouseButtonPressed = true;
    else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) 
        IsLeftMouseButtonPressed = false;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if(action != GLFW_PRESS) return;

    if(key == GLFW_KEY_ESCAPE) {
        userInputs += '\e';
    } else if(key == GLFW_KEY_ENTER) {
        userInputs += '\r';
    } else if(key == GLFW_KEY_BACKSPACE) {
        userInputs += '\b';
    }
}

SimulationData simulationData;

int main() {

    GLFWwindow* window = nullptr;

    if(window_init(&window) == -1) return -1;

    simulationData.loadFromFiles();
    
    std::shared_ptr<Solver> solver = std::make_shared<Solver>();

    glfwSetCursorPosCallback(window, cursor_position_callback);

    Vec2 terminalSize = TUI::ScreenSize(); 
    std::shared_ptr<Window> jointsWindow = std::make_shared<JointsTab>(terminalSize, simulationData.joints);
    std::shared_ptr<Window> linkagesWindow = std::make_shared<linkagesTab>(terminalSize, simulationData.linkages, simulationData.joints);
    std::shared_ptr<Window> constraintsWindow = std::make_shared<ConstraintsTab>(terminalSize, simulationData.constraints, simulationData.joints, solver);
    
    std::shared_ptr<Window> topBarWindow = std::make_shared<TopBar>(jointsWindow, linkagesWindow, constraintsWindow);

    glfwSetCharCallback(window, char_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    WindowManager windowManager(terminalSize);
    
    windowManager.openWindow(topBarWindow);
    windowManager.selectWindow(topBarWindow);


    ENABLE_ALT_BUF();

    bool leftDragging = false;
    Vec2 initPosLeft;
    Vec2 offset;

    LineRenderer::init();

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        int width;
        glfwGetFramebufferSize(window, &width, &windowHeight);

        if (IsLeftMouseButtonPressed && !leftDragging) {
            initPosLeft = cursorPos;
            leftDragging = true;
        }

        if (leftDragging) {
            Vec2 realOffset = offset + (cursorPos - initPosLeft);

            if (!IsLeftMouseButtonPressed) {
                offset = realOffset;
                leftDragging = false;
            }
        }

        Vec2 realOffset = offset;

        if(leftDragging) {
            realOffset = offset + (cursorPos - initPosLeft);
        }


        glm::mat4 projection = glm::ortho(0.0f - realOffset.x, (float)(width*cursorScaling) - realOffset.x, 0.0f - realOffset.y, (float)(windowHeight*cursorScaling) - realOffset.y, -1.0f, 1.0f);

        (*simulationData.joints)[0]->setPosition(cursorPos*cursorScaling-realOffset);

        for (std::shared_ptr<LineRenderer> line : (*simulationData.linkages)) {
            line->draw(projection);
        }

        TUI::clear();
        WindowStatus status = windowManager.update();
        if(!userInputs.empty()) {
            windowManager.keyboardUpdate(userInputs);
            userInputs.clear();
        }

        // Solver:
        TUI::setCursor(Vec2(70, 0));
        std::cout << "before solving: " << status.stopSimulation << std::flush;
        if(!status.stopSimulation) {
            solver->updateConstraints(simulationData.constraints);
            bool fixed_result = solver->solve(1000);
            if(fixed_result) {
                TUI::setCursor(Vec2(100, 0));
                std::cout << "solved" << std::flush;
            } else {
                TUI::setCursor(Vec2(100, 0));
                std::cout << "unsolved" << std::flush;
            }
        } else {
            TUI::setCursor(Vec2(100, 0));
            std::cout << "paused" << std::flush;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    DISABLE_ALT_BUF();
    glfwTerminate();
    return 0;
}