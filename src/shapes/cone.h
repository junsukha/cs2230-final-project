#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    Cone(int param1, int param2);
    void updateParams(int param1, int param2);
    std::vector<float> getConeData() { return m_vertexData; }

    void makeSideTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void makeWedge(float currentTheta, float nextTheta);
    void makeCone();
    void makeBottomTile(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
