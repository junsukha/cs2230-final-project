#include "cone.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/rotate_vector.hpp" //I added this
#include <iostream>

Cone::Cone(int param1, int param2) {
    updateParams(param1, param2);
}

void Cone::updateParams(int param1, int param2) {

    m_vertexData = std::vector<float>();
    if (param2 < 3)
        param2 = 3;
    if (param1 < 1)
        param1 = 1;
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeCone();
}

//void Cone::findConicalTopNormal(x, y, z)

void Cone::makeBottomTile(glm::vec3 topLeft,
                               glm::vec3 topRight,
                               glm::vec3 bottomLeft,
                               glm::vec3 bottomRight) {
    // trianges where 90 degree is at top left
    insertVec3(m_vertexData, topLeft);
    glm::vec3 normal = glm::vec3(0, -1, 0.f);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);



    // trianges where 90 degree is at bottom right
    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);


    insertVec3(m_vertexData, bottomRight);
//    normal = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));
    insertVec3(m_vertexData, normal);
}


void Cone::makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

//    auto findNormal = [&](auto vertex) {return glm::normalize(glm::vec3(2*vertex.x, 4*(0.5-vertex.y), 2*vertex.z));};

    auto findNormal = [&](auto vertex) {
        auto normal = glm::vec3(2*vertex.x, 0.25 - 0.5 * vertex.y, 2*vertex.z);
//         if (x,y,z) is the tip of cone, normal = (0,0,0)
        if (normal == glm::vec3(0,0,0)) {
            /*
//            normal = glm::vec3(0,1,0);
//            float newVertexX = (bottomLeft.x + bottomRight.x) / 2.f;
//            float newVertexZ = (bottomLeft.z + bottomRight.z) / 2.f;
            glm::vec3 newVertex = (bottomLeft+bottomRight);
            newVertex = glm::normalize(newVertex);

//            float radiance = sqrt(pow(newVertexX, 2) + pow(newVertexZ, 2));
            float radiance = sqrt(pow(newVertex.x, 2) + pow(newVertex.z, 2));
//            normal = glm::vec3(newVertex.x, radiance*(tan(90.f - atan(2))), newVertex.z);
//            normal = glm::vec3(newVertexX, radiance*(tan(90.f - atan(2))), newVertexZ);
            std::cout << "newVertex: " << "(" << newVertex.x << "," << newVertex.y << "," << newVertex.z << ")" <<  std::endl;
            normal = glm::vec3(newVertex.x, 0.5 * tan(M_PI/2.f - atan(2)), newVertex.z);
            normal = glm::vec3(newVertex.x, (0.5 - bottomLeft.y) * tan(M_PI/2.f - atan(2)), newVertex.z);
//            glm::transform(bottomLeft,)
            */


            // rotate bottomLeft vector. E.g, if thetastep = 120 (when m_param2 = 3), rotate 60
            // use bottomLeft vertex to calculate normal of topLeft or topRight vertex when topLeft or topRight is tip of cone
            float radius = 0.5;
            // multiply by raidus so that newBottomLeft's length is always 0.5
            glm::vec3 newBottomLeft = rotateY(glm::normalize(bottomLeft), 360.f/(m_param2*2)) * radius;
//            normal = glm::vec3(2*bottomLeft.x, 0.25 - 0.5 * bottomLeft.y, 2*bottomLeft.z);

            // use the middle point of bottomLeft vertex and bottomRight vertex as (x,z) direction of the normal of the tip
            glm::vec3 newVertex = (bottomLeft+bottomRight);
            newVertex = glm::normalize(newVertex) * glm::vec3(0.5f);
            // use y = (x,z) vector length * (tan(90 - atan(2)) as y value of the normal af the tip
            normal = glm::vec3(newVertex.x, sqrt(pow(newVertex.x,2) + pow(newVertex.z,2))*(tan(M_PI/2.f - atan(2))), newVertex.z);
//            normal = glm::vec3(newBottomLeft.x, 0.5*(tan(M_PI/2.f - atan(2))), newBottomLeft.z);

            // I use bottomLeft and bottomRight to get x,z of vertex at the tip of cone. y is always  0.5*(tan(M_PI/2 - atan(2))
        }
        return glm::normalize(normal);
    };

    // trianges where 90 degree is at top left
    insertVec3(m_vertexData, topLeft);
//    glm::vec3 normal = glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z));
    glm::vec3 normal = findNormal(topLeft);

//    if(normal == glm::vec3(0.f,0,0))
//        std::cout << "check normal" << std::endl;
//        normal = glm::vec3(topLeft.x,0,topLeft.z);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    normal = findNormal(bottomLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    normal = findNormal(topRight);
    insertVec3(m_vertexData, normal);


    // trianges where 90 degree is at bottom right
    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    normal = findNormal(topRight);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    normal = findNormal(bottomLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
//    normal = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));
    normal = findNormal(bottomRight);
    insertVec3(m_vertexData, normal);
}

void Cone::makeWedge(float currentTheta, float nextTheta) {
    float r = 0.5f;
    float yTop = 0.5;
    float slope = 2;

    float exceptioinalThetaStep= glm::radians(360.f / (m_param2 * 2));



    for (int i = 0; i < m_param1; ++i) {

        // topLeft vertex
        float y = yTop - float(i) / float(m_param1);

        // set radius of a cone at given y
//        float radiusAtY = (yTop - y) / slope;
        float radiusAtY = 0.5*(0.5-y);


        float x = sin(currentTheta) * radiusAtY;
        float z = cos(currentTheta) * radiusAtY;

        // when vertex is the tip of cone
        if (x == 0.f && z == 0.f) {
            // add additional theta
            float newCurrentTheata = currentTheta + exceptioinalThetaStep;

            x = sin(newCurrentTheata) * radiusAtY;
            z = cos(newCurrentTheata) * radiusAtY;
        }

        glm::vec3 topLeft(x,y,z); // for side

        // radius for bottom plane is always r
        // bottom plane topRight vertex
        x = sin(currentTheta) * r;
        z = cos(currentTheta) * r;
        glm::vec3 bottomPlaneTopRight(x *  float(i) / float(m_param1), // for top plane
                                   - 0.5f,
                                  z * float(i) / float(m_param1));

        // topRight
        x = sin(nextTheta) * radiusAtY;
        z = cos(nextTheta) * radiusAtY;
        if (x == 0.f && z == 0.f) {
            // add additional theta
            float newCurrentTheata = nextTheta + exceptioinalThetaStep;

            x = sin(newCurrentTheata) * radiusAtY;
            z = cos(newCurrentTheata) * radiusAtY;
        }

        glm::vec3 topRight(x,y,z);

        // bottom plane topLeft vertex
        y = yTop - float(i) / float(m_param1);

        radiusAtY = 0.5*(0.5-y);

        x = sin(nextTheta) * r;
        z = cos(nextTheta) * r;
        glm::vec3 bottomPlaneTopLeft(x *  float(i) / float(m_param1),
                                  - 0.5f,
                                  z * float(i) / float(m_param1));

        // bottomLeft
        y = yTop - float(i+1) / float(m_param1);
        radiusAtY = (yTop - y) / slope;

        x = sin(currentTheta) * radiusAtY;
        z = cos(currentTheta) * radiusAtY;

        glm::vec3 bottomLeft(x,y,z);

        // bottom plane bottomRight vertex
        x = sin(currentTheta) * r;
        z = cos(currentTheta) * r;
        glm::vec3 bottomPlaneBottomRight(x *  float(i+1) / float(m_param1),
                                   - 0.5f,
                                  z * float(i+1) / float(m_param1));

        // bottomRight
        y = yTop - float(i+1) / float(m_param1);
        radiusAtY = (yTop - y) / slope;

        x = sin(nextTheta) * radiusAtY;
        z = cos(nextTheta) * radiusAtY;

        glm::vec3 bottomRight(x,y,z);

        // bottom plane bottomLeft vertex
        x = sin(nextTheta) * r;
        z = cos(nextTheta) * r;
        glm::vec3 bottomPlaneBottomLeft(x *  float(i+1) / float(m_param1),
                                   - 0.5f,
                                  z * float(i+1) / float(m_param1));

        // draw side
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);
        makeBottomTile(bottomPlaneTopLeft, bottomPlaneTopRight, bottomPlaneBottomLeft, bottomPlaneBottomRight);
    }
}

void Cone::makeCone() {
    // Task 7: create a full sphere using the makeWedge() function you
    //         implemented in Task 6
    // Note: think about how param 2 comes into play here!
    float thetaStep;
    float currentTheta;
    float nextTheta;

    for (int i = 0; i < m_param2; ++i) {
        thetaStep= glm::radians(360.f / m_param2);
        currentTheta = i * thetaStep;
        nextTheta = (i+1) * thetaStep;

        makeWedge(currentTheta, nextTheta); // this includes top plane, side, bottom plane
    }

//     makeWedge
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
