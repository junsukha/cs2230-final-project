#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cube
{
public:
    Cube(int param1);
    void updateParams(int param1);
    std::vector<float> getCubeData() {
        return m_vertexData; }

    // insert u,v coordinates
    void insertVec2(std::vector<float> &data, glm::vec2 v);
private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);

    std::vector<float> m_vertexData;
    int m_param1;
};
