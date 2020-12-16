configure_file(VoxelsConfig.hpp.in VoxelsConfig.hpp)

add_executable(Voxels main.cpp VoxelApp.cpp VolumeRenderer.cpp Volume.cpp) 
target_link_libraries(Voxels MonarchDebug)
target_include_directories(Voxels PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
