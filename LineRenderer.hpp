#pragma once

#include "Joint.hpp"
#include "Shader/Shader.hpp"
#include "Vec2.hpp"
#include <memory>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct LineRenderer {

    constexpr static float vertices[] = {
        // x, y       u, v
        0.0f, 1.0f,  0.0f, 1.0f, // top-left
        1.0f, 0.0f,  1.0f, 0.0f, // bottom-right
        0.0f, 0.0f,  0.0f, 0.0f, // bottom-left

        0.0f, 1.0f,  0.0f, 1.0f, // top-left
        1.0f, 1.0f,  1.0f, 1.0f, // top-right
        1.0f, 0.0f,  1.0f, 0.0f  // bottom-right
    };

    inline static uint VAO, VBO;
    inline static unsigned int texture;
    inline static Shader shader;
    
    std::shared_ptr<Joint> j1;
    std::shared_ptr<Joint> j2;

    float thickness;
    LineRenderer(std::shared_ptr<Joint> j_1, std::shared_ptr<Joint>  j_2 , float thick) : j1(j_1), j2(j_2), thickness(thick) {}
    
    void draw(glm::mat4 projection) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);

        shader.use();

        shader.setMat4("u_projection", projection);
        shader.setInt("spriteTexture", 0);
        shader.setVec2("point1", (vec2)*j1);
        
        shader.setVec2("point2", (vec2)*j2);
        shader.setFloat("thickness", thickness);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    
    static void init() {

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        unsigned char d[] = {255, 255, 255, 255};
        unsigned char* data = d;

        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGBA,
            1,1,0, 
            GL_RGBA, GL_UNSIGNED_BYTE, data
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);

        shader = Shader("./shader/sprite.vert", "./shader/sprite.frag");

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);

        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
        glEnableVertexAttribArray(1);
    }
};