#include "Monarch.hpp"
#include "EntryPoint.hpp"

#include "VoxelApp.hpp"

Monarch::Application* Monarch::createApplication(){
    return new VoxelApp();
}
