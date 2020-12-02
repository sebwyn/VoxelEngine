#include "VoxelApp.hpp"

#include "CubeRenderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>


int VoxelApp::width = 800;
int VoxelApp::height = 600;

VoxelApp::VoxelApp() : Monarch::Application("Voxels", width, height), octree(Octree(glm::vec3(0, 0, 0), 2.5, 5))
{
    Monarch::CubeRenderer::init();

    camera.addComponent<Monarch::Transform>();
    camera.addComponent<Monarch::FirstPersonController>();
    camera.addComponent<Monarch::Camera>(width, height);
}

VoxelApp::~VoxelApp()
{
    Monarch::CubeRenderer::destroy();
}

void VoxelApp::update()
{
    camera.update();
    //use the cube renderer to draw something here 
    Monarch::Camera& cam = camera.getComponent<Monarch::Camera>();
    Monarch::CubeRenderer::setMatrices(cam.getProjectionMatrix(), cam.getViewMatrix());
    
    octree.drawRegions();
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