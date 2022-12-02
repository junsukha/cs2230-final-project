#include "cube.h"
#include "src/settings.h"
Cube::Cube(int param1) {
    updateParams(param1);
}

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    if (param1 < 1)
        param1 = 1;
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.

    // triange where 90 degree is at left bottom
    insertVec3(m_vertexData, topLeft);
    glm::vec3 normal = glm::normalize(glm::cross((bottomLeft - topLeft), (bottomRight - bottomLeft)));
    insertVec3(m_vertexData, normal);
    if(settings.extraCredit4) {
        glm::vec2 uv = glm::vec2(topLeft.x+0.5, topLeft.y+0.5);
        insertVec2(m_vertexData, uv);
    }

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    if(settings.extraCredit4) {
        glm::vec2 uv = glm::vec2(bottomLeft.x+0.5, bottomLeft.y+0.5);
        insertVec2(m_vertexData, uv);
    }

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    if(settings.extraCredit4) {
        glm::vec2 uv = glm::vec2(topLeft.x+0.5, topLeft.y+0.5);
        insertVec2(m_vertexData, uv);
    }

    // triange where 90 degree is at top right
    insertVec3(m_vertexData, topLeft);
    normal = glm::normalize(glm::cross((bottomRight - topLeft), (topRight - bottomRight)));

    insertVec3(m_vertexData, normal);
    if(settings.extraCredit4) {
        glm::vec2 uv = glm::vec2(topLeft.x+0.5, topLeft.y+0.5);
        insertVec2(m_vertexData, uv);
    }

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);
    if(settings.extraCredit4) {
        glm::vec2 uv = glm::vec2(bottomRight.x+0.5, bottomRight.y+0.5);
        insertVec2(m_vertexData, uv);
    }

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    if(settings.extraCredit4) {
        glm::vec2 uv = glm::vec2(topRight.x+0.5, topRight.y+0.5);
        insertVec2(m_vertexData, uv);
    }
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    glm::vec3 unitHeightLength = (bottomLeft - topLeft) / glm::vec3(m_param1, m_param1, m_param1);
    glm::vec3 unitWidthLength = (topRight - topLeft) / glm::vec3(m_param1, m_param1, m_param1);
    glm::vec3 normal;
    glm::vec3 newTopLeft;
    glm::vec3 newTopRight;
    glm::vec3 newBottomLeft;
    glm::vec3 newBottomRight;
    for (int col = 0; col < m_param1; ++col) {
        for (int row = 0; row < m_param1; ++row) {

            newTopLeft =  topLeft  + unitHeightLength * glm::vec3(row, row, row)
                    + unitWidthLength * glm::vec3(col,col,col);

            newBottomLeft =  topLeft + unitHeightLength * glm::vec3(row+1, row+1, row+1)
                    + unitWidthLength * glm::vec3(col,col,col);

            newBottomRight = topLeft + unitHeightLength * glm::vec3(row+1, row+1, row+1) + unitWidthLength * glm::vec3(col+1, col+1, col+1);

            newTopRight = topLeft + unitWidthLength * glm::vec3(col+1,col+1,col+1)
                    + unitHeightLength * glm::vec3(row, row, row);


            makeTile(newTopLeft, newTopRight, newBottomLeft, newBottomRight);

            /*
            // triange where 90 degree is at left bottom
            insertVec3(m_vertexData, newTopLeft);
            normal = glm::normalize(glm::cross((newBottomLeft - newTopLeft), (newBottomRight - newBottomLeft)));
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, newBottomLeft);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, newBottomRight);
            insertVec3(m_vertexData, normal);

            // triange where 90 degree is at top right
            insertVec3(m_vertexData, newTopLeft);
            normal = glm::normalize(glm::cross((newBottomRight - newTopLeft), (newTopRight - newBottomRight)));
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, newBottomRight);
            insertVec3(m_vertexData, normal);

            insertVec3(m_vertexData, newTopRight);
            insertVec3(m_vertexData, normal);
            */
        }
    }
}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

//     makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
//              glm::vec3( 0.5f,  0.5f, 0.5f),
//              glm::vec3(-0.5f, -0.5f, 0.5f),
//              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:
    // front side
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f));

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
    // right side
    makeFace(glm::vec3(0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f));

    // left side
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));

    // back side
    makeFace(glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));

    // top side
    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, 0.5f),
             glm::vec3(0.5f, 0.5f, 0.5f));

    // bottom side
    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f));

}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

// for textur mapping extra credit
void Cube::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
