#include "VoxelApp.hpp"

#include "CubeRenderer.hpp"
#include "VolumeRenderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


int VoxelApp::width = 800;
int VoxelApp::height = 600;

uint8_t VoxelApp::density(glm::vec3 pos)
{
    if(sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z) < 1.0) return 1;
    else return 0;
}

VoxelApp::VoxelApp() : Monarch::Application("Voxels", width, height) 
{
    Monarch::CubeRenderer::init();
    VolumeRenderer::init();

    camera.addComponent<Monarch::Transform>();
    camera.addComponent<Monarch::FirstPersonController>();
    camera.addComponent<Monarch::Camera>(width, height);

    //sample the density at discrete points, and then use 8 points to generate the actual voxel data
    glm::ivec3 dimensions(10);

    glm::ivec3 dimP1 = dimensions + glm::ivec3(1);
    std::unique_ptr<uint8_t> densityData = std::unique_ptr<uint8_t>(new uint8_t[dimP1.x * dimP1.y * dimP1.z]);

    glm::vec3 start(-5.0, -5.0, -5.0);
    glm::vec3 end(5.0, 5.0, 5.0);
    glm::vec3 step = (end - start) / glm::vec3(dimensions);
    for(int x = 0; x < dimP1.x; x++){
        for(int y = 0; y < dimP1.y; y++){
            for(int z = 0; z < dimP1.z; z++){
                glm::vec3 pos = step * glm::vec3(x, y, z);
                densityData.get()[(dimP1.x * dimP1.y * z) + (dimP1.x * y) + x] = density(pos);
            } 
        } 
    } 

    glm::ivec3 cubePoints[] = {
        glm::ivec3(0, 0, 0),
        glm::ivec3(0, 0, 1),
        glm::ivec3(0, 1, 0),
        glm::ivec3(0, 1, 1),
        glm::ivec3(1, 0, 0),
        glm::ivec3(1, 0, 1),
        glm::ivec3(1, 1, 0),
        glm::ivec3(1, 1, 1)
    };

    std::unique_ptr<uint8_t> volumeData = std::unique_ptr<uint8_t>(new uint8_t[dimensions.x * dimensions.y * dimensions.z]);
    for(int x = 0; x < dimensions.x; x++){
        for(int y = 0; y < dimensions.y; y++){
            for(int z = 0; z < dimensions.z; z++){
                glm::ivec3 base = glm::ivec3(x, y, z);
                uint8_t cornerMask = 0;
                for(int i = 0; i < 8; i++){
                    glm::ivec3 pos = base + cubePoints[i]; 
                    if(densityData.get()[(dimP1.x * dimP1.y * pos.z) + (dimP1.x * pos.y) + pos.x]){
                        cornerMask |= (1 << i);
                        break;
                    }
                }
                if(cornerMask) 
                    volumeData.get()[(dimensions.x * dimensions.y * base.x) + (dimensions.x * base.y) + base.x] = 0xFF;
                else
                    volumeData.get()[(dimensions.x * dimensions.y * base.x) + (dimensions.x * base.y) + base.x] = 0x00; 
            } 
        } 
    } 

    volume.addComponent<Monarch::Transform>();
    volume.addComponent<Volume>(volumeData.get(), dimensions);
}

VoxelApp::~VoxelApp()
{
    Monarch::CubeRenderer::destroy();
    VolumeRenderer::destroy();
}

void VoxelApp::update()
{
    camera.update();
    //use the cube renderer to draw something here 
    Monarch::Camera& cam = camera.getComponent<Monarch::Camera>();
    Monarch::CubeRenderer::setMatrices(cam.getProjectionMatrix(), cam.getViewMatrix());
    VolumeRenderer::setMatrices(cam.getProjectionMatrix(), cam.getViewMatrix());

    VolumeRenderer::drawVolume(volume, glm::vec2(window->getWidth(), window->getHeight()));
}

void VoxelApp::handleEvent(Monarch::Event& e)
{
    e.dispatch<Monarch::KeyPressedEvent>(std::bind(&VoxelApp::handleKey, this, std::placeholders::_1));
    camera.onEvent(e);
}

bool VoxelApp::handleKey(Monarch::KeyPressedEvent& e)
{
    if(e.getKey() == GLFW_KEY_ESCAPE){
        if(cursor) window->disableCursor();
        else window->enableCursor();
        cursor = !cursor;
    }
    return false;
}
