#include "VolumeRenderer.hpp"

#include "components/Transform.hpp"
#include "Volume.hpp"
#include "VoxelsConfig.hpp"

std::string VolumeRenderer::vertPath = std::string(VOXELS_PROJECT_DIR) + std::string("assets/shaders/vert.glsl");
std::string VolumeRenderer::fragPath = std::string(VOXELS_PROJECT_DIR) + std::string("assets/shaders/frag.glsl");
Monarch::ShaderProgram* VolumeRenderer::shader;
glm::mat4 VolumeRenderer::projectionMatrix; 
glm::mat4 VolumeRenderer::viewMatrix;

void VolumeRenderer::init()
{
    shader = new Monarch::ShaderProgram(vertPath.c_str(), fragPath.c_str());
}

void VolumeRenderer::destroy()
{
   delete shader; 
}

void VolumeRenderer::drawVolume(Monarch::Entity entity, glm::vec2 winDimensions)
{
    Monarch::Transform& transform = entity.getComponent<Monarch::Transform>();
    glm::mat4 modelMatrix = transform.calcModelMatrix();

    glm::mat4 viewI = glm::inverse(viewMatrix);
    glm::mat4 projI = glm::inverse(projectionMatrix);
    glm::mat4 modelI = glm::inverse(modelMatrix);
    //get the camera position relative to the object
    glm::vec3 camPos = modelI * viewI * glm::vec4(0, 0, 0, 1); 

    //bind the texture
    shader->start();
    shader->setUniformMat4(projectionMatrix, "projectionMatrix");
    shader->setUniformMat4(modelI * viewI * projI, "inverseMat");

    shader->setUniformVec3(camPos, "camPos");
    shader->setUniformVec2(winDimensions, "viewport"); 

    Volume& volume = entity.getComponent<Volume>();  
    volume.texture->bind(0);
    shader->setUniformVec3(volume.dimensions, "dimensions");

    glDrawArrays(GL_TRIANGLES, 0, 6);
}
