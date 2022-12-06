#include "sphere.h"
#include <iostream>
Sphere::Sphere(int param1, int param2) {
    updateParams(param1, param2);
}

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    if (param1 < 2)
        param1 = 2;
    if (param2 < 3)
        param2 = 3;
    m_param1 = param1;
    m_param2 = param2;
    setVertexData();
}

glm::vec2 Sphere::findUV(glm::vec3 &point) {
    float u, v;
    // get u
    float theta = atan2(point.z, point.x);
    if (theta <= 0) {
        u = -theta/(2*M_PI);
    } else {
        u = 1 - theta / (2*M_PI);
    }
    // get v
    float phi = asin(point.y / 0.5);
    v = phi / M_PI + 1 / 2.f;

    if (v == 0.f || v == 1.f)
        u = 0.5;

    return glm::vec2(u,v);
}
void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Task 5: Implement the makeTile() function for a Sphere
    // Note: this function is very similar to the makeTile() function for Cube,
    //       but the normals are calculated in a different way!

    // trianges where 90 degree is at left bottom
    insertVec3(m_vertexData, topLeft);
    glm::vec3 normal = glm::normalize(topLeft);
    insertVec3(m_vertexData, normal);
    glm::vec2 uv = findUV(topLeft);
    insertVec2(m_vertexData, uv);

    insertVec3(m_vertexData, bottomLeft);
    normal = glm::normalize(bottomLeft);
    insertVec3(m_vertexData, normal);
    uv = findUV(bottomLeft);
    insertVec2(m_vertexData, uv);

    insertVec3(m_vertexData, bottomRight);
    normal = glm::normalize(bottomRight);
    insertVec3(m_vertexData, normal);
    uv = findUV(bottomRight);
    insertVec2(m_vertexData, uv);

    // trianges where 90 degree is at right top
    insertVec3(m_vertexData, topLeft);
    normal = glm::normalize(topLeft);
    insertVec3(m_vertexData, normal);
    uv = findUV(topLeft);
    insertVec2(m_vertexData, uv);

    insertVec3(m_vertexData, bottomRight);
    normal = glm::normalize(bottomRight);
    insertVec3(m_vertexData, normal);
    uv = findUV(bottomRight);
    insertVec2(m_vertexData, uv);

    insertVec3(m_vertexData, topRight);
    normal = glm::normalize(topRight);
    insertVec3(m_vertexData, normal);
    uv = findUV(topRight);
    insertVec2(m_vertexData, uv);

}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    // Task 6: create a single wedge of the sphere using the
    //         makeTile() function you implemented in Task 5
    // Note: think about how param 1 comes into play here!

    float r = 0.5f;
    for (int i = 0; i < m_param1; ++i) {
        // topLeft
        float phi = glm::radians(180.f * (float(i) / m_param1));

        float x = r * sin(currentTheta) * sin(phi);
        float y = r * cos(phi);
        float z = r * sin(phi) * cos(currentTheta);

        glm::vec3 topLeft(x,y,z);

        // topRight
        phi = glm::radians(180.f * (float(i) / m_param1));

        x = r * sin(nextTheta) * sin(phi);
        y = r * cos(phi);
        z = r * sin(phi) * cos(nextTheta);

        glm::vec3 topRight(x,y,z);

        // bottomLeft
        phi = glm::radians(180.f * (float(i+1) / m_param1));

        x = r * sin(currentTheta) * sin(phi);
        y = r * cos(phi);
        z = r * sin(phi) * cos(currentTheta);

        glm::vec3 bottomLeft(x,y,z);

        // bottomRight
        phi = glm::radians(180.f * (float(i+1) / m_param1));

        x = r * sin(nextTheta) * sin(phi);
        y = r * cos(phi);
        z = r * sin(phi) * cos(nextTheta);

        glm::vec3 bottomRight(x,y,z);

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }


/*
//    for (int row = 0; row < m_param1; row++) {
//        float r = 1.f;
//        float phi = glm::radians(180.f * (row / m_param1));
//        float theta = glm::radians(0.f);

//        float x = r * sin(theta) * sin(phi);
//        float y = r * cos(phi);
//        float z = r * sin(phi) * cos(theta);

//        glm::vec3 topLeft(x,y,z);
//        glm::vec3 topRight(x,y,z);

//        // left
//        r = 1.f;
//        phi = glm::radians(180.f * ((row+1) / m_param1));
//    //    theta = glm::radians(currentTheta);

//        x = r * sin(currentTheta) * sin(phi);
//        y = r * cos(phi);
//        z = r * sin(phi) * cos(currentTheta);

//        glm::vec3 bottomLeft(x,y,z);

//        // right
//        r = 1.f;
//        phi = glm::radians(180.f * ((row+1) / m_param1));
//    //    theta = glm::radians(nextTheta);

//        x = r * sin(nextTheta) * sin(phi);
//        y = r * cos(phi);
//        z = r * sin(phi) * cos(nextTheta);

//        glm::vec3 bottomRight(x,y,z);

//        makeTile(topLeft, topRight, bottomLeft, bottomRight);
//    }
*/

   /*
    // for now let's try only m_param1 = 2
    // upper hemisphere
    // top
    float r = 1.f;
    float phi = glm::radians(0.f);
    float theta = glm::radians(0.f);

    float x = r * sin(theta) * sin(phi);
    float y = r * cos(phi);
    float z = r * sin(phi) * cos(theta);

    glm::vec3 topLeft(x,y,z);
    glm::vec3 topRight(x,y,z);

    // left
    r = 1.f;
    phi = glm::radians(90.f);
//    theta = glm::radians(currentTheta);

    x = r * sin(currentTheta) * sin(phi);
    y = r * cos(phi);
    z = r * sin(phi) * cos(currentTheta);

    glm::vec3 bottomLeft(x,y,z);

    // right
    r = 1.f;
    phi = glm::radians(90.f);
//    theta = glm::radians(nextTheta);

    x = r * sin(nextTheta) * sin(phi);
    y = r * cos(phi);
    z = r * sin(phi) * cos(nextTheta);

    glm::vec3 bottomRight(x,y,z);

    makeTile(topLeft, topRight, bottomLeft, bottomRight);



//    float r = 1.f;
//    float phi = glm::radians(0.f);
//    float theta = glm::radians(0.f);

//    float x = r * sin(phi) * cos(theta);
//    float y = r * cos(phi);
//    float z = r * sin(phi) * sin(theta);

//    glm::vec3 topLeft(x,y,z);
//    glm::vec3 topRight(x,y,z);
//    // left
//    r = 1.f;
//    phi = glm::radians(90.f);
//    theta = glm::radians(135.f);

//    x = r * sin(phi) * cos(theta);
//    y = r * cos(phi);
//    z = r * sin(phi) * sin(theta);

//    glm::vec3 bottomLeft(x,y,z);
//    // right
//    r = 1.f;
//    phi = glm::radians(90.f);
//    theta = glm::radians(0.f);

//    x = r * sin(phi) * cos(theta);
//    y = r * cos(phi);
//    z = r * sin(phi) * sin(theta);

//    glm::vec3 bottomRight(x,y,z);

//    makeTile(topLeft, topRight, bottomLeft, bottomRight);


    // lower hemisphere
    r = 1.f;
    phi = glm::radians(90.f);
//    theta = glm::radians(currentTheta);

    x = r * sin(currentTheta) * sin(phi);
    y = r * cos(phi);
    z = r * sin(phi) * cos(currentTheta);

    topLeft = glm::vec3(x,y,z);


    r = 1.f;
    phi = glm::radians(180.f);
    theta = glm::radians(0.f);

    x = r * sin(theta) * sin(phi);
    y = r * cos(phi);
    z = r * sin(phi) * cos(theta);

    bottomLeft = glm::vec3(x,y,z);
    bottomRight = glm::vec3(x,y,z);
    // right
    r = 1.f;
    phi = glm::radians(90.f);
//    theta = glm::radians(nextTheta);

    x = r * sin(nextTheta) * sin(phi);
    y = r * cos(phi);
    z = r * sin(phi) * cos(nextTheta);
    topRight = glm::vec3(x,y,z);


    makeTile(topLeft, topRight, bottomLeft, bottomRight);



//    makeTile();
*/
}

void Sphere::makeSphere() {
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

        makeWedge(currentTheta, nextTheta);
    }

//     makeWedge
}

void Sphere::setVertexData() {
    // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

//     float thetaStep = glm::radians(360.f / m_param2);
//     float currentTheta = 0 * thetaStep;
//     float nextTheta = 1 * thetaStep;
//     makeWedge(currentTheta, nextTheta);

    // Uncomment these lines to make sphere for Task 7:

     makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
