#pragma once

#include <functional>
#include <memory>

#include "Texture.hpp"
#include "ECS.hpp"

class Volume : public Monarch::Component {
public:
    Volume(uint8_t* data, glm::ivec3 dimensions); 

    friend class VolumeRenderer;
private:
    glm::mat4 modelMatrix;
    glm::vec3 dimensions;

    std::unique_ptr<Monarch::Texture> texture; 
};
