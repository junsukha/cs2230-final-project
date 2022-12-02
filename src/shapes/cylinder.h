#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    Cylinder(int param1, int param2);
    void updateParams(int param1, int param2);
    std::vector<float> getCylinderData() { return m_vertexData; }

    void makeCylinder();
    void makeWedge(float currentTheta, float nextTheta);
    void makeSideTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeBottomTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeTopTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
