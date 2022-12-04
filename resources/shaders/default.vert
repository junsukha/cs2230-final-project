#version 330 core

// vertex shader read in data contained in the VAO currently bound at the time of drawing
layout(location = 0) in vec3 objectSpacePosition;
layout(location = 1) in vec3 objectSpaceNormal;
// add u,v to each shape generator
layout(location = 2) in vec2 uvPosition;


uniform mat4 modelMatrix; // constant across all vertices // will store model matrix (m_model) data here
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// Task 5: declare `out` variables for the world-space position and normal,
//         to be passed to the fragment shader
out vec4 worldSpacePosition;
out vec4 worldSpaceNormal;
// for texture mapping
out vec2 uvCoord;

void main() {
    // for texture mapping
    uvCoord = uvPosition;

    // Task 8: compute the world-space position and normal, then pass them to
    //         the fragment shader using the variables created in task 5

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.
    worldSpacePosition =  modelMatrix * vec4(objectSpacePosition,1);
//    vec3 worldSpaceNormalVec3 = inverse(transpose(mat3(modelMatrix))) * objectSpaceNormal;
//    worldSpaceNormal = viewMatrix * vec4(worldSpaceNormalVec3, 0);
    worldSpaceNormal =  vec4(inverse(transpose(mat3(modelMatrix))) * normalize(objectSpaceNormal), 0);

    // Task 9: set gl_Position to the object space position transformed to clip space
    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(objectSpacePosition, 1); // gl_Position is vec4

    // final project

}
