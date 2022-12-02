#include <glm/glm.hpp>
#include <vector>
class SphereSolution {

public:
    glm::vec4 sphericalToCartesian(float phi, float theta);
    void pushVec3(glm::vec4 vec, std::vector<float> *data);
    std::vector<float> generateSphereData(int phiTesselations, int thetaTesselations);
};
