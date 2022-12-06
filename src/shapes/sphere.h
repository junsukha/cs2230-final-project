#pragma once

#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
    Sphere(int param1, int param2);
    void updateParams(int param1, int param2);
//    std::vector<float> generateShape() { return m_vertexData; }
    std::vector<float> getSphereData() {
        return m_vertexData; }
    // insert u,v coordinates
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    glm::vec2 findUV(glm::vec3 &point);
private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();

    std::vector<float> m_vertexData;
    float m_radius = 0.5;
    int m_param1;
    int m_param2;
};
