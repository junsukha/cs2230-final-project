#pragma once

#include "scenedata.h"
#include <string>
#include <vector>

// Struct which contains data for a single primitive, to be used for rendering
struct RenderShapeData {
    ScenePrimitive primitive;
    glm::mat4 ctm; // the cumulative transformation matrix
    // final project
    glm::vec4 velocity{0, 0, 0, 0};
    bool isFalling;
    bool onlyOnce;
    bool stop;
    float speed;
    float v;
    int index;
};

// Struct which contains all the data needed to render a scene
struct RenderData {
    SceneGlobalData globalData;
    SceneCameraData cameraData;

    std::vector<SceneLightData> lights;
    std::vector<RenderShapeData> shapes;
};

class SceneParser {
  public:
    // Parse the scene and store the results in renderData.
    // @param filepath    The path of the scene file to load.
    // @param renderData  On return, this will contain the metadata of the
    // loaded scene.
    // @return            A boolean value indicating whether the parse was
    // successful.
    static bool parse(std::string filepath, RenderData &renderData);

    static void parseHelper(SceneNode *currentNode, RenderData &renderData,
                            glm::mat4 ctm);
    static glm::mat4
    getCTM(std::vector<SceneTransformation *> &transformations);
};
