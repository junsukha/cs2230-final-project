#include "sceneparser.h"
#include "scenefilereader.h"
#include "glm/gtx/transform.hpp"

#include <chrono>
#include <memory>
#include <iostream>

bool SceneParser::parse(std::string filepath, RenderData &renderData) {
    ScenefileReader fileReader = ScenefileReader(filepath);
    bool success = fileReader.readXML();
    if (!success) {
        return false;
    }

    // TODO: Use your code from Lab 5 here.
    // Task 4: populate renderData with global data, lights, and camera data
       renderData.globalData = fileReader.getGlobalData();
       renderData.cameraData = fileReader.getCameraData();
       renderData.lights = fileReader.getLights();

       // Task 5: populate renderData's list of primitives and their transforms.
       //         This will involve traversing the scene graph, and we recommend you
       //         create a helper function to do so!

       // get the rood node of the scene graph
       SceneNode* rootNode = fileReader.getRootNode();

       renderData.shapes.clear(); // datatype of renderData.shapes is std::vector<RenderShapeData>

       // ctm tracks all transformations and multiply them in each node
       glm::mat4 ctm (1.f);

       parseHelper(rootNode, renderData, ctm);
    return true;
}

void SceneParser::parseHelper(SceneNode* currentNode, RenderData &renderData, glm::mat4 ctm) {

    // update ctm
    // ctm = parent ctm * current trasnformations
    ctm = ctm * getCTM(currentNode->transformations); // order

    // curretNode is leaf node.. base case
    RenderShapeData renderShapeData;
    if (currentNode->children.size() == 0) {
        for (int i = 0; i < currentNode->primitives.size(); i++) {
            ScenePrimitive primitive = *currentNode->primitives[i];
            renderShapeData = RenderShapeData{primitive, ctm}; //ctm is the same for every primitive in this node
            // add current RenderShapeData to renderData.shapes
            renderData.shapes.push_back(renderShapeData);
        }// for

        /* don't need this because I'm not using reference
        //before returning, clear transformation saved vector
        ctm = glm::mat4 (1.0f);
        */
        return;
    }
    /** non-leaf nodes **/
    /* already did at the very beginning
    // when you arrive at a node, update ctm first
    // make matrix using all SceneTransformations in a vector transformations
    ctm = ctm * getCTM(currentNode->transformations); //order matters
    */

    // this is non-leaf nodes, but if there're primitives, add to renderData. cmt is already updated
    for (auto i = 0; i < currentNode->primitives.size(); i++) {
        ScenePrimitive primitive = *(currentNode->primitives[i]);
        renderShapeData = RenderShapeData{primitive, ctm};// these primitives are in the same node so have the same ctm
        renderData.shapes.push_back(renderShapeData);
    }

    // run parseHelper for each node in children vector
    for (auto i = 0; i < currentNode->children.size(); i++) {
//        SceneNode current = *(currentNode->children[i]); // this one works
//        parseHelper(&current, renderData, ctm);

//        SceneNode* nextNode = currentNode->children[i]; // this works
//        parseHelper(nextNode, renderData, ctm); // works

//        currentNode = currentNode->children[i]; // this doesn't work because currentNode is updated but it's still in use in for loop
//        parseHelper(currentNode, renderData, ctm);

        parseHelper(currentNode->children[i], renderData, ctm); // works
    }

    // we update renderData with reference. no need to return anything
//    return renderShapeData;
}

// multiply all transformations in a vector "transformations"
glm::mat4 SceneParser::getCTM(std::vector<SceneTransformation*> &transformations) {
    glm::mat4 matrix (1.f);
    glm::mat4 cmt (1.f);
    // for each transformation in a vector transformations, make a glm::mat4
    for (int i = 0; i < transformations.size(); i++){
        SceneTransformation* currentTransformation = transformations[i];
        switch(currentTransformation->type){
            case TransformationType::TRANSFORMATION_TRANSLATE:
                matrix = glm::translate(currentTransformation->translate);
                break;
            case TransformationType::TRANSFORMATION_SCALE:
                matrix = glm::scale(currentTransformation->scale);
                break;
            case TransformationType::TRANSFORMATION_ROTATE:
                matrix = glm::rotate(currentTransformation->angle, currentTransformation->rotate);
                break;
            case TransformationType::TRANSFORMATION_MATRIX:
                matrix = currentTransformation->matrix;
                break;
            }
        // multiply each matrix
        cmt = cmt * matrix;
    }

    return cmt;
}
