#include "cylinder.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"

Cylinder::Cylinder(int param1, int param2) {
    updateParams(param1, param2);
}

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    if (param2 < 3)
        param2 = 3;
    if (param1 < 1)
        param1 = 1;
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}


void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    makeCylinder();

}

void Cylinder::makeSideTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {

    // trianges where 90 degree is at top left
    insertVec3(m_vertexData, topLeft);
    glm::vec3 normal = glm::normalize(glm::vec3(topLeft.x, 0, topLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topRight);
    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);



    // trianges where 90 degree is at bottom right
    insertVec3(m_vertexData, topRight);
    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
    normal = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));
    insertVec3(m_vertexData, normal);



}

void Cylinder::makeTopTile(glm::vec3 topLeft,
                           glm::vec3 topRight,
                           glm::vec3 bottomLeft,
                           glm::vec3 bottomRight) {
    // trianges where 90 degree is at top left
    insertVec3(m_vertexData, topLeft);
    glm::vec3 normal = glm::vec3(0, 1, 0.f);
//    glm::vec3 normal = glm::cross(bottomLeft - topLeft, topRight - bottomLeft);
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
//    normal = glm::cross(bottomLeft - topRight, bottomRight - bottomLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
//    normal = glm::cross()
    insertVec3(m_vertexData, normal);



    // trianges where 90 degree is at bottom right
    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
//    normal = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));
    insertVec3(m_vertexData, normal);
}

void Cylinder::makeBottomTile(glm::vec3 topLeft,
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

    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);



    // trianges where 90 degree is at bottom right
    insertVec3(m_vertexData, topRight);
//    normal = glm::normalize(glm::vec3(topRight.x, 0, topRight.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomLeft);
//    normal = glm::normalize(glm::vec3(bottomLeft.x, 0, bottomLeft.z));
    insertVec3(m_vertexData, normal);

    insertVec3(m_vertexData, bottomRight);
//    normal = glm::normalize(glm::vec3(bottomRight.x, 0, bottomRight.z));
    insertVec3(m_vertexData, normal);
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    float r = 0.5f;

    for (int i = 0; i < m_param1; ++i) {
        // topLeft
        float phi = glm::radians(45.f + (90.f * float(i) / m_param1));
        float x = sin(currentTheta) * 0.5;
        float y = r - float(i) / float(m_param1);
        float z = cos(currentTheta) * 0.5;

        glm::vec3 topLeft(x,y,z); // for side
        glm::vec3 topPlaneTopLeft(x *  float(i) / float(m_param1), // for top plane
                                   0.5f,
                                  z * float(i) / float(m_param1));


        // topRight
        x = sin(nextTheta) * 0.5;
//        y = r - float(i) / float(m_param1);
        z = cos(nextTheta) * 0.5;

        glm::vec3 topRight(x,y,z);
        glm::vec3 topPlaneTopRight(x *  float(i) / float(m_param1),
                                  0.5f,
                                  z * float(i) / float(m_param1));

        // bottomLeft
        phi = glm::radians(45.f + (90.f * float(i+1) / m_param1));
        x = sin(currentTheta) * 0.5;
        y = r - float(i+1) / float(m_param1);
        z = cos(currentTheta) * 0.5;

        glm::vec3 bottomLeft(x,y,z);
        glm::vec3 topPlaneBottomLeft(x *  float(i+1) / float(m_param1),
                                   0.5f,
                                  z * float(i+1) / float(m_param1));

        // bottomRight
        x = sin(nextTheta) * 0.5;
//        y = r - float(i+1) / float(m_param1);
        z = cos(nextTheta) * 0.5;

        glm::vec3 bottomRight(x,y,z);
        glm::vec3 topPlaneBottomRight(x *  float(i+1) / float(m_param1),
                                   0.5f,
                                  z * float(i+1) / float(m_param1));

        // draw side
        makeSideTile(topLeft, topRight, bottomLeft, bottomRight);

        // draw top plane
        makeTopTile(topPlaneTopLeft, topPlaneTopRight, topPlaneBottomLeft, topPlaneBottomRight);

        // draw bottom plane
        glm::vec3 bottomPlaneTopLeft = glm::vec3(topPlaneTopRight.x, -0.5, topPlaneTopRight.z);
        glm::vec3 bottomPlaneTopRight = glm::vec3(topPlaneTopLeft.x, -0.5, topPlaneTopLeft.z);
        glm::vec3 bottomPlaneBottomLeft = glm::vec3(topPlaneBottomRight.x, -0.5, topPlaneBottomRight.z);
        glm::vec3 bottomPlaneBottomRight = glm::vec3(topPlaneBottomLeft.x, -0.5, topPlaneBottomLeft.z);
        makeBottomTile(bottomPlaneTopLeft, bottomPlaneTopRight, bottomPlaneBottomLeft, bottomPlaneBottomRight);
    }
}

void Cylinder::makeCylinder() {
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
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
