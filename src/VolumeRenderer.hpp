#pragma once

#include <string>

#include "ShaderProgram.hpp"
#include "ECS.hpp"

class VolumeRenderer {
public:
    static void init(); 
    static void destroy();

    static void setMatrices(glm::mat4 pMatrix, glm::mat4 vMatrix)
    {
        projectionMatrix = pMatrix;
        viewMatrix = vMatrix;
    }

    static void drawVolume(Monarch::Entity entity, glm::vec2 winDimensions);
private:
    static std::string vertPath, fragPath;

    static Monarch::ShaderProgram* shader;
    static glm::mat4 projectionMatrix, viewMatrix;
};
