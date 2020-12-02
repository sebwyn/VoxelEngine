#include "Octree.hpp"

#include <iostream>

#include "CubeRenderer.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

Octree::Octree(glm::vec3 center, float halfWidth, int depth) : center(center), halfWidth(halfWidth), depth(depth)
{
    root = new OctreeNode();
    createNode("o", center, halfWidth, depth, 0, *root);
}

void Octree::drawNode(OctreeNode& node, int d)
{
    Monarch::CubeRenderer::CubeData data;
    data.center = node.center;
    //data.color = depthToColor[d];
    data.halfWidth = halfWidth / pow(2, d) - 0.01 * d;
    Monarch::CubeRenderer::drawCube(data);
    if(node.childMask != 0 && node.childMask != -1 && d != depth){
        for(int i = 0; i < 8; i++){
            if(node.childMask & (3 << (i * 2)))
                drawNode(node.children[i], d+1); 
        }
    }
}

void Octree::drawRegions()
{
    Monarch::CubeRenderer::beginBatch();

    //march through the octree adding cubes as we go
    drawNode(*root, 0); 

    Monarch::CubeRenderer::endBatch();
    Monarch::CubeRenderer::flush();
}

void Octree::raycast(glm::vec3 origin, glm::vec3 direction)
{
    a = 0;
    // fixes for rays with negative direction
    if(direction.x < 0){
        origin.x = root->center.x - origin.x;
        direction.x = - direction.x;
        a |= 4 ; //bitwise OR (latest bits are XYZ)
    }
    if(direction.y < 0){
        origin.y = root->center.y - origin.y;
        direction.y = - direction.y;
        a |= 2 ; 
    }
    if(direction.z < 0){
        origin.z = root->center.z - origin.z;
        direction.z = - direction.z;
        a |= 1 ; 
    }

    double divx = 1 / direction.x; // IEEE stability fix
    double divy = 1 / direction.y;
    double divz = 1 / direction.z;

    double tx0 = (root->center.x - halfWidth - origin.x) * divx;
    double tx1 = (root->center.x + halfWidth - origin.x) * divx;
    double ty0 = (root->center.y - halfWidth - origin.y) * divy;
    double ty1 = (root->center.y + halfWidth - origin.y) * divy;
    double tz0 = (root->center.z - halfWidth - origin.z) * divz;
    double tz1 = (root->center.z + halfWidth - origin.z) * divz;

    if( fmax(fmax(tx0,ty0),tz0) < fmin(fmin(tx1,ty1),tz1) ){
        proc_subtree(tx0,ty0,tz0,tx1,ty1,tz1,*(root));
    }
}

//the density function is just a sphere 
bool Octree::density(glm::vec3 pos)
{
    bool d = sqrt(pos.x*pos.x + pos.y*pos.y + pos.z*pos.z) < 1;
    std::cout << "Density at " << glm::to_string(pos) << " is " << d << std::endl;
    return d; 
}

short Octree::createNode(std::string id, glm::vec3 center, float radius, int depth, int currDepth, OctreeNode& node)
{
    node.childMask= 0;
    //node.debugID = id;
    node.center = center;
    if(currDepth == depth){
        node.children = nullptr;    
        char mask;
        //use the radius because we are sampling the corners not the centers of the octets
        mask |= density(center + glm::vec3(-radius, -radius, -radius)) << 0;
        mask |= density(center + glm::vec3(-radius, -radius,  radius)) << 1;
        mask |= density(center + glm::vec3(-radius,  radius, -radius)) << 2;
        mask |= density(center + glm::vec3(-radius,  radius,  radius)) << 3;
        mask |= density(center + glm::vec3( radius, -radius, -radius)) << 4;
        mask |= density(center + glm::vec3( radius, -radius,  radius)) << 5;
        mask |= density(center + glm::vec3( radius,  radius, -radius)) << 6;
        mask |= density(center + glm::vec3( radius,  radius,  radius)) << 7;
        if(mask == 0){
            return 0;
        } else if(mask == -1){
            return 3;
        } else {
            return 1;
        }
    } else {
        float hr = radius / 2;
        int newDepth = currDepth + 1;
        OctreeNode* children = new OctreeNode[8];
        node.childMask |= createNode(id + "1", center + glm::vec3(-hr, -hr, -hr), hr, depth, newDepth, children[0]) << 0;
        node.childMask |= createNode(id + "2", center + glm::vec3(-hr, -hr,  hr), hr, depth, newDepth, children[1]) << 2;
        node.childMask |= createNode(id + "3", center + glm::vec3(-hr,  hr, -hr), hr, depth, newDepth, children[2]) << 4;
        node.childMask |= createNode(id + "4", center + glm::vec3(-hr,  hr,  hr), hr, depth, newDepth, children[3]) << 6;
        node.childMask |= createNode(id + "5", center + glm::vec3( hr, -hr, -hr), hr, depth, newDepth, children[4]) << 8;
        node.childMask |= createNode(id + "6", center + glm::vec3( hr, -hr,  hr), hr, depth, newDepth, children[5]) << 10;
        node.childMask |= createNode(id + "7", center + glm::vec3( hr,  hr, -hr), hr, depth, newDepth, children[6]) << 12;
        node.childMask |= createNode(id + "8", center + glm::vec3( hr,  hr,  hr), hr, depth, newDepth, children[7]) << 14;
        
        if(node.childMask == 0 || node.childMask == -1){
            //if the child nodes are solid air or solid material
            delete[] children;
            node.children = nullptr;

            if(node.childMask == 0) return 0;
            else return 3;
        } else {
            //if the child nodes contain edges
            node.children = children;
            return 1; 
        }
    }
}

int Octree::first_node(double tx0, double ty0, double tz0, double txm, double tym, double tzm)
{
    unsigned char answer = 0;   // initialize to 00000000
    // select the entry plane and set bits
    if(tx0 > ty0){
        if(tx0 > tz0){ // PLANE YZ
            if(tym < tx0) answer|=2;    // set bit at position 1
            if(tzm < tx0) answer|=1;    // set bit at position 0
            return (int) answer;
        }
    }
    else {
        if(ty0 > tz0){ // PLANE XZ
            if(txm < ty0) answer|=4;    // set bit at position 2
            if(tzm < ty0) answer|=1;    // set bit at position 0
            return (int) answer;
        }
    }
    // PLANE XY
    if(txm < tz0) answer|=4;    // set bit at position 2
    if(tym < tz0) answer|=2;    // set it at position 1
    return (int) answer;
}

int Octree::new_node(double txm, int x, double tym, int y, double tzm, int z)
{
    if(txm < tym){
        if(txm < tzm){return x;}  // YZ plane
    }
    else{
        if(tym < tzm){return y;} // XZ plane
    }
    return z; // XY plane;
}

void Octree::proc_subtree (double tx0, double ty0, double tz0, double tx1, double ty1, double tz1, OctreeNode& node){
    float txm, tym, tzm;
    int currNode;

    //std::cout << "Reached Node: " << node.debugID << std::endl;
    if(tx1 < 0 || ty1 < 0 || tz1 < 0) return;
    if(!node.children){
        if(node.childMask){
            std::cout << "Reached filled node " << std::endl;
        }
        return;
    }
    //else{ std::cout << "Reached node " << std::endl;}

    txm = 0.5*(tx0 + tx1);
    tym = 0.5*(ty0 + ty1);
    tzm = 0.5*(tz0 + tz1);

    //so apparently this is supposed to just be nvim in vscode just like this
    currNode = first_node(tx0,ty0,tz0,txm,tym,tzm);
    do{
        switch (currNode)
        {
        case 0: { 
            proc_subtree(tx0,ty0,tz0,txm,tym,tzm,node.children[a]);
            currNode = new_node(txm,4,tym,2,tzm,1);
            break;}
        case 1: { 
            proc_subtree(tx0,ty0,tzm,txm,tym,tz1,node.children[1^a]);
            currNode = new_node(txm,5,tym,3,tz1,8);
            break;}
        case 2: { 
            proc_subtree(tx0,tym,tz0,txm,ty1,tzm,node.children[2^a]);
            currNode = new_node(txm,6,ty1,8,tzm,3);
            break;}
        case 3: { 
            proc_subtree(tx0,tym,tzm,txm,ty1,tz1,node.children[3^a]);
            currNode = new_node(txm,7,ty1,8,tz1,8);
            break;}
        case 4: { 
            proc_subtree(txm,ty0,tz0,tx1,tym,tzm,node.children[4^a]);
            currNode = new_node(tx1,8,tym,6,tzm,5);
            break;}
        case 5: { 
            proc_subtree(txm,ty0,tzm,tx1,tym,tz1,node.children[5^a]);
            currNode = new_node(tx1,8,tym,7,tz1,8);
            break;}
        case 6: { 
            proc_subtree(txm,tym,tz0,tx1,ty1,tzm,node.children[6^a]);
            currNode = new_node(tx1,8,ty1,8,tzm,7);
            break;}
        case 7: { 
            proc_subtree(txm,tym,tzm,tx1,ty1,tz1,node.children[7^a]);
            currNode = 8;
            break;}
        }
    } while (currNode<8);
}
