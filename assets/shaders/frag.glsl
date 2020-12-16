#version 330 core

uniform mat4 projectionMatrix, inverseMat;
uniform vec3 camPos;
uniform vec2 viewport;
uniform vec3 dimensions;

uniform sampler3D volume;

out vec4 color;

void main(){
    vec4 ndcSpace;
    ndcSpace.x = gl_FragCoord.x / viewport.x * 2. - 1.; 
    ndcSpace.y = gl_FragCoord.y / viewport.y * 2. - 1.;
    ndcSpace.z = 1.;
    ndcSpace.w = 1.;

    float T1 = projectionMatrix[2][2];
    float T2 = projectionMatrix[3][2];
    float cameraspaceZ = -T2 / (ndcSpace.z + T1);
    vec4 clipSpace = -cameraspaceZ * ndcSpace;

    vec3 worldCoord = vec3(inverseMat * clipSpace);
    
    vec3 rayDir = normalize(worldCoord - camPos); 
    vec3 deltaDir = 1 / rayDir;
    //get the entry voxel
    ivec3 entry = ivec3(worldCoord);
    vec3 start = vec3(0) - (vec3(dimensions) / 2.); 
    vec3 end = start + vec3(dimensions);  
    if(start.x > camPos.x || end.x < camPos.x ||
       start.y > camPos.y || end.x < camPos.y ||
       start.z > camPos.z || end.z < camPos.z)
    {
        //if any of these are less than zero, disregard
        float tMinX = -dimensions.x - camPos.x / deltaDir.x; 
        float tMaxX =  dimensions.x - camPos.x / deltaDir.x;
        float tMinY = -dimensions.y - camPos.y / deltaDir.y; 
        float tMayY =  dimensions.y - camPos.y / deltaDir.y;
        float tMinZ = -dimensions.z - camPos.z / deltaDir.z; 
        float tMazZ =  dimensions.z - camPos.z / deltaDir.z;
    }

    color = vec4(rayDir, 1); 
}
