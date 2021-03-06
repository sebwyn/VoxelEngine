#pragma once

#include <string>
#include <glm/glm.hpp>

class Octree {
public:
    Octree(glm::vec3 center, float halfWidth, int depth); 

    struct OctreeNode {
        uint16_t childPtr;
        uint8_t validMask, leafMask;
    };

    //should be 8 kb in total
    struct OctreeBlock {
        uint32_t header;
        //OctreeNodes are 32 bits or 4 bytes
        OctreeNode data[2047];
    };

    void drawRegions();
    void softwareRender(int width, int height, glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
    void raycast(glm::vec3 origin, glm::vec3 direction, std::vector<OctreeNode>& visitedLeaves);
private:
    //struct for drawing
    glm::vec3 depthToColor[6] = {
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 0, 1),
        glm::vec3(1, 1, 0),
        glm::vec3(1, 0, 1),
        glm::vec3(0, 1, 1)
    };

    int depth;
    glm::vec3 center;
    float halfWidth;
    OctreeNode* root;

    void drawNode(OctreeNode& node, int d);
    
    bool density(glm::vec3 pos);
    short createNode(std::string id, glm::vec3 center, float radius, int depth, int currDepth, OctreeNode& node);

    //used for raycasting
    int a = 0;
    int first_node(double tx0, double ty0, double tz0, double txm, double tym, double tzm);
    int new_node(double txm, int x, double tym, int y, double tzm, int z);
    void proc_subtree (double tx0, double ty0, double tz0, double tx1, double ty1, double tz1, OctreeNode& node, std::vector<OctreeNode>& visitedLeaves);
};
