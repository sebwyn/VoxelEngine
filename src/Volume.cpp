#include "Volume.hpp"

Volume::Volume(uint8_t* data, glm::ivec3 dimensions) 
 : dimensions(dimensions)
{
    texture = std::unique_ptr<Monarch::Texture>(new Monarch::Texture(data, dimensions.x, dimensions.y, dimensions.z, 1));
}
