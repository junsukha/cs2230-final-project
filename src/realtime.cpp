#include "realtime.h"

#include "settings.h"
#include "shapes/cone.h"
#include "shapes/cube.h"
#include "shapes/cylinder.h"
#include "shapes/sphere.h"
#include <QCoreApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <iostream>

#include "glm/gtc/constants.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "shapes/sphereSolution.h"
#include "utils/objreader2.h"
#include "utils/shaderloader.h"
#include <glm/gtx/string_cast.hpp>

// #include "utils/objreader.h"
//  ================== Project 5: Lights, Camera

// extern objl::Loader loader;
Realtime::Realtime(QWidget *parent) : QOpenGLWidget(parent) {
    m_prev_mouse_pos = glm::vec2(size().width() / 2, size().height() / 2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W] = false;
    m_keyMap[Qt::Key_A] = false;
    m_keyMap[Qt::Key_S] = false;
    m_keyMap[Qt::Key_D] = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space] = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();
    std::cout << "finish called" << std::endl;
    // Students: anything requiring OpenGL calls when the program exits should
    // be done here

    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);

    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteVertexArrays(1, &m_cube_vao);

    glDeleteProgram(m_shader);
    //// from lab 11
    ///
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteBuffers(1, &m_fullscreen_vbo);

    // Task 35: Delete OpenGL memory here
    // glDeleteTextures(1, &m_kitten_texture);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    glDeleteProgram(m_texture_shader);

    this->doneCurrent();
}

void Realtime::initializeSphere(int param1, int param2) {

    // Generate and bind VBO
    glGenBuffers(1, &m_sphere_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);

    Sphere sphere(param1, param2);
    m_sphereData = sphere.getSphereData();

    glBufferData(GL_ARRAY_BUFFER, m_sphereData.size() * sizeof(GLfloat),
                 m_sphereData.data(), GL_STATIC_DRAW);
    // Generate, and bind vao
    glGenVertexArrays(1, &m_sphere_vao);
    glBindVertexArray(m_sphere_vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::initializeCube(int param1) {

    glGenBuffers(1, &m_cube_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);

    Cube cube(param1);
    m_cubeData = cube.getCubeData();

    glBufferData(GL_ARRAY_BUFFER, m_cubeData.size() * sizeof(GLfloat),
                 m_cubeData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_cube_vao);
    glBindVertexArray(m_cube_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::initializeCylinder(int param1, int param2) {
    glGenBuffers(1, &m_cylinder_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);

    Cylinder cylinder(param1, param2);
    m_cylinderData = cylinder.getCylinderData();

    glBufferData(GL_ARRAY_BUFFER, m_cylinderData.size() * sizeof(GLfloat),
                 m_cylinderData.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_cylinder_vao);
    glBindVertexArray(m_cylinder_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void *>(0));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::initializeGL() { // only called once when run program.
    isInitializedRun = true;
    m_devicePixelRatio = this->devicePixelRatio();

    // from lab 11
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    m_timer = startTimer(1000 / 60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err)
                  << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION)
              << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    // glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio,
               size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should
    // be done here
    glClearColor(0, 0, 0, 1);

    m_shader = ShaderLoader::createShaderProgram(
        ":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(
        ":/resources/shaders/postprocessing.vert",
        ":/resources/shaders/postprocessing.frag");

    // extra credit 3
    //    if(settings.extraCredit3) {
    ////        initializeMesh();
    //        initializeMesh2();
    //    }

    int param1 = settings.shapeParameter1;
    int param2 = settings.shapeParameter2;

    initializeSphere(param1, param2);

    initializeCube(param1);

    // clean up binding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /***************** Project 6 **********************/
    if (settings.extraCredit4) {
        glUseProgram(m_shader);
        GLuint textureID = glGetUniformLocation(m_shader, "shapeTexture");
        glUniform1i(textureID, 0); // this is not unbind. bind GL_TEXTURE0
        glUseProgram(0);

    } else {
        glUseProgram(m_texture_shader); // can write this here? not earlier?
        GLuint textureID = glGetUniformLocation(m_texture_shader, "myTexture");
        // 0 (or GL_TEXTURE0) is a texture slot we want to use and sample from
        // Set the uniform value for your sampler2D you created in your fragment
        // shader to be the same texture slot number we bound our texture to
        glUniform1i(textureID,
                    0); // We're setting a sampler (in our case, GL_TEXTURE0) to
                        // uniform sample2D myTexture in texture.frag
        // Return to default state of program 0
        glUseProgram(0);

        fullScreenQuad();
    }
    // In project 6, this is where I draw my scene on. For extra credit, I will
    // draw my scenes with shape texture attached..
    makeFBO();
}

void Realtime::fullScreenQuad() {
    std::vector<GLfloat> fullscreen_quad_data = {-1.f, 1.0f, 0.0f, 0.f,  1.f,

                                                 -1.f, -1.f, 0.0f, 0.0f, 0.0f,

                                                 1.f,  -1.f, 0.0f, 1.f,  0.0f,

                                                 1.f,  1.f,  0.0f, 1.f,  1.f,

                                                 -1.f, 1.f,  0.0f, 0.0f, 1.f,

                                                 1.f,  -1.f, 0.0f, 1.f,  0.0f};

    // Generate and bind a VBO and a VAO for a fullscreen quad
    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size() * sizeof(GLfloat),
                 fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);
    // Task 14: modify the code below to add a second attribute to the vertex
    // attribute array
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    // Unbind the fullscreen quad's VBO and VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::makeFBO() { // in initializeGL()
    // Task 19: Generate and bind an empty texture, set its min/mag filter
    // interpolation, then unbind Before we configure our FBO, we need to
    // generate containers (texture object) for its attachments. As stated
    // previously, these can either be Textures or Renderbuffers.
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0); // sampler
    glBindTexture(
        GL_TEXTURE_2D,
        m_fbo_texture); // m_fbo_texture is a texture object. This call means
                        // that use that texture object for texture 2D data.
                        // glTexImage2D will uses whatever texture object was
                        // bound to GL_TEXTURE_2D
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // return to default state by unbinding our texture once we haveset all its
    // parameters. "texture" here means GL_TEXTURE_2D I think.
    glBindTexture(GL_TEXTURE_2D, 0);
    // Task 20: Generate and bind a renderbuffer of the right size, set its
    // format, then unbind
    glGenRenderbuffers(
        1, &m_fbo_renderbuffer); // m_fbo_renderbuffer is a renderbuffer object
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width,
                          m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    // Task 18: Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); // m_fbo is a frame buffer object
    // Task 21: Add our texture as a color attachment, and our renderbuffer as a
    // depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, m_fbo_renderbuffer);
    // Task 22: Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::extraCredit1() {
    int numOfObjects = metaData.shapes.size();
    if (settings.extraCredit1) {
        int param1 = 2.f * settings.shapeParameter1 / numOfObjects;
        int param2 = 2.f * settings.shapeParameter2 / numOfObjects;
        initializeSphere(param1, param2);

        initializeCube(param1);

    } else { // if click is true, turned on extrCredit at least once
        int param1 = settings.shapeParameter1;
        int param2 = settings.shapeParameter2;
        initializeSphere(param1, param2);

        initializeCube(param1);
    }
}

void Realtime::refreshLights() {
    for (int i = 0; i < 8; ++i) {
        GLuint isSpotId = glGetUniformLocation(
            m_shader,
            ("lights[" + std::to_string(i) + "].isSpotLight").c_str());
        glUniform1f(isSpotId, false);

        GLuint directionalLight = glGetUniformLocation(
            m_shader,
            ("lights[" + std::to_string(i) + "].isDirectional").c_str());
        glUniform1f(directionalLight, false);

        GLint color = glGetUniformLocation(
            m_shader, ("lights[" + std::to_string(i) + "].color").c_str());
        glUniform4f(color, 0.f, 0, 0, 0);
    }
}

void Realtime::uniformLight() {
    // send number of colors to uniform
    int numOfLights = metaData.lights.size();
    GLint num = glGetUniformLocation(m_shader, "numOfLights");
    glUniform1i(num, numOfLights);

    refreshLights();

    // send colors to uniform
    for (int i = 0; i < metaData.lights.size(); ++i) {

        GLint function = glGetUniformLocation(
            m_shader, ("lights[" + std::to_string(i) + "].function").c_str());
        glUniform3fv(function, 1, &metaData.lights[i].function[0]);

        GLint color = glGetUniformLocation(
            m_shader, ("lights[" + std::to_string(i) + "].color").c_str());
        glUniform4fv(color, 1, &metaData.lights[i].color[0]);

        // if light is directional, send false to uniform isDirectionaLight
        GLuint directionalLight = glGetUniformLocation(
            m_shader,
            ("lights[" + std::to_string(i) + "].isDirectional").c_str());
        if (metaData.lights[i].type != LightType::LIGHT_DIRECTIONAL) {
            glUniform1f(directionalLight, false);
            m_lightPos = metaData.lights[i].pos;

            GLuint uniformLightPosition = glGetUniformLocation(
                m_shader,
                ("lights[" + std::to_string(i) + "].position").c_str());
            glUniform4fv(uniformLightPosition, 1, &m_lightPos[0]);

            if (metaData.lights[i].type == LightType::LIGHT_SPOT) {
                GLuint isSpotId = glGetUniformLocation(
                    m_shader,
                    ("lights[" + std::to_string(i) + "].isSpotLight").c_str());
                glUniform1f(isSpotId, true);
                // set angle
                GLuint angleID = glGetUniformLocation(
                    m_shader,
                    ("lights[" + std::to_string(i) + "].angle").c_str());
                glUniform1f(angleID, metaData.lights[i].angle);

                // set penumbra
                GLuint penumbra = glGetUniformLocation(
                    m_shader,
                    ("lights[" + std::to_string(i) + "].penumbra").c_str());
                glUniform1f(penumbra, metaData.lights[i].penumbra);

                // set direction
                // final project
                if (settings.finalProject) {
                    for (auto &shape : metaData.shapes) {
                        if (shape.primitive.type ==
                            PrimitiveType::PRIMITIVE_SPHERE) {
                            metaData.lights[i].dir = glm::normalize(
                                shape.ctm * glm::vec4{0, 0, 0, 1.f} -
                                metaData.lights[i].pos);
                        }
                    }
                }
                GLuint dirID = glGetUniformLocation(
                    m_shader,
                    ("lights[" + std::to_string(i) + "].direction").c_str());
                glUniform4fv(dirID, 1, &metaData.lights[i].dir[0]);
            }

        } else { // (metaData.lights[i].type == LightType::LIGHT_DIRECTIONAL){
            //                    std::cout << metaData.lights[i].dir[0] << " "
            //                    << metaData.lights[i].dir[1] << " " <<
            //                    metaData.lights[i].dir[2] << std::endl;

            // final project
            // directional light lights where camera is looking at
            if (settings.finalProject) {
                metaData.lights[i].dir = metaData.cameraData.look;
            }

            glUniform1f(directionalLight, true);
            // send direction of directional light to uniform variable
            GLuint loc0 = glGetUniformLocation(
                m_shader,
                ("lights[" + std::to_string(i) + "].direction").c_str());
            glUniform4fv(loc0, 1, &metaData.lights[i].dir[0]);
            //            glUniform4f(loc0, metaData.lights[i].dir[0],
            //            metaData.lights[i].dir[1], metaData.lights[i].dir[2],
            //            metaData.lights[i].dir[3]);
        }
    } // light

    GLint loc1 =
        glGetUniformLocation(m_shader, "k_a"); // uniform variable in frag
    glUniform1f(loc1, metaData.globalData.ka); // variable in realtime.h

    GLint loc2 = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(loc2, metaData.globalData.kd);

    GLint loc3 = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(loc3, metaData.globalData.ks);
}

void Realtime::extraCredit2(RenderShapeData &shape,
                            glm::vec4 cameraPositionWorldSpace) {
    if (settings.extraCredit2) {
        glm::vec4 objectOriginWorldSpace = shape.ctm * glm::vec4(0, 0, 0, 1);
        float dist =
            glm::distance(cameraPositionWorldSpace, objectOriginWorldSpace);
        //        int remain = dist % 10;
        //        switch(dist %)

        int param1 = settings.shapeParameter1 / (dist * 5 / 100);
        int param2 = settings.shapeParameter2 / (dist * 5 / 100);
        //                if(dist <= 12.f) {
        //                    param1 = settings.shapeParameter1;
        //                    param2 = settings.shapeParameter2;
        //                }
        initializeSphere(param1, param2);

        initializeCube(param1);
    }
}

void Realtime::extraCredit4(RenderShapeData &shape) {
    // texture mapping ec
    //  extra credit Texture Mapping
    if (settings.extraCredit4) {
        std::string textureFileName =
            shape.primitive.material.textureMap.filename;
        QString qstrTextureFileName = QString::fromStdString(textureFileName);

        QImage qImage;
        if (!qImage.load(qstrTextureFileName)) {
            std::cout << "Your Image Path: "
                      << qstrTextureFileName.toStdString() << " is not valid."
                      << std::endl;
            // exit(1);
        }

        m_image = QImage(qstrTextureFileName);
        m_image = m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();

        glGenTextures(1, &m_shape_texture);
        glActiveTexture(GL_TEXTURE0);
        // basically, this is binding texture (m_fbo_texture) to GL_TEXTURE0,
        // which means telling sampler GL_TEXTURE0 where to sample from.
        glBindTexture(GL_TEXTURE_2D, m_shape_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image.width(),
                     m_image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     m_image.bits());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        glUseProgram(0);
    }
}

void Realtime::uniformShape(RenderShapeData &shape) {
    // for texture mapping extra credit
    extraCredit4(shape);

    GLint loc4 = glGetUniformLocation(m_shader, "shininess");
    float shininess = shape.primitive.material.shininess;
    glUniform1f(loc4, shininess);
    GLint loc5 = glGetUniformLocation(m_shader, "o_a");
    glUniform4fv(loc5, 1, &shape.primitive.material.cAmbient[0]);
    GLint loc6 = glGetUniformLocation(m_shader, "o_d");
    glUniform4fv(loc6, 1, &shape.primitive.material.cDiffuse[0]);
    GLint loc7 = glGetUniformLocation(m_shader, "o_s");
    glUniform4fv(loc7, 1, &shape.primitive.material.cSpecular[0]);
    GLint blendID = glGetUniformLocation(m_shader, "blend");
    glUniform1f(blendID, shape.primitive.material.blend);

    // calculate model matrix
    m_model = shape.ctm;
    GLuint uniformMatrixID = glGetUniformLocation(m_shader, "modelMatrix");
    glUniformMatrix4fv(uniformMatrixID, 1, GL_FALSE, &m_model[0][0]);
}

void Realtime::checkShapeTypeAndDraw(RenderShapeData &shape) {
    if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glBindVertexArray(m_cube_vao);

        GLint locWireframe = glGetUniformLocation(m_shader, "wireframe");
        glUniform1i(locWireframe, true);

        if (settings.extraCredit4) {
            glBindTexture(GL_TEXTURE_2D, m_shape_texture);
            glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 8);

        } else {
            glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size() / 6);
        }
        glBindVertexArray(0);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    else if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {

        GLint locWireframe = glGetUniformLocation(m_shader, "wireframe");
        glUniform1i(locWireframe, false);

        glBindVertexArray(m_sphere_vao);
        glDrawArrays(GL_TRIANGLES, 0, m_sphereData.size() / 6);
        glBindVertexArray(0);
    }
}

void Realtime::paintShapes() {
    glUseProgram(m_shader);

    // calculate view matrix
    m_view = generateViewMatrix();
    // calculate projection matrix
    m_proj = generateProjectionMatrix();
    // Task 7: pass in m_view and m_proj
    GLuint uniformViewMatrixID = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(uniformViewMatrixID, 1, GL_FALSE, &m_view[0][0]);

    GLuint uniformProjectMatrixID =
        glGetUniformLocation(m_shader, "projectionMatrix");
    glUniformMatrix4fv(uniformProjectMatrixID, 1, GL_FALSE, &m_proj[0][0]);
    // Students: anything requiring OpenGL calls every frame should be done here

    // get world space camera position
    glm::vec4 cameraPositionWorldSpace =
        inverse(m_view) * glm::vec4(0, 0, 0, 1.f);
    GLint loc8 = glGetUniformLocation(m_shader, "worldSpaceCameraPosition");
    glUniform4fv(loc8, 1, &cameraPositionWorldSpace[0]);

    uniformLight();

    // final project. texture
    int idx = 0;

    for (auto &shape : metaData.shapes) {
        // extra credit 2
        extraCredit2(shape, cameraPositionWorldSpace);

        /********* for texture of final project ***********/
        if (settings.texture)
            useTexture(shape, idx);
        /********* for texture ***********/

        uniformShape(shape);

        checkShapeTypeAndDraw(shape);

        idx++;
    } // shapes
    glUseProgram(0);
}

// final project
void Realtime::useTexture(RenderShapeData &shape, int idx) {
    if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {

        glGenTextures(0, &m_shape_texture);

        glActiveTexture(GL_TEXTURE0);

        glBindTexture(GL_TEXTURE_2D, m_shape_texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textures[idx].width(),
                     textures[idx].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                     textures[idx].bits());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);

        GLuint textureID = glGetUniformLocation(m_shader, "myTexture2");
        glUniform1i(
            textureID,
            0); // this is not unbinding. This is sending GL_TEXTURE0 as a
                // uniform variable into "myTexture2", i.e., GL_TEXTURE0 == 0
    }
}

void Realtime::paintGL() {
    // Clear screen color and depth before painting
    // call this once. If called for each shape, only the last shape will appear
    // on the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Project6. Currently, when inverse is on, only shapes get inverted. This
    // is because background is default FBO. I need to, although I can invert
    // shapes already, send shapes and background to my FBO and invert color.
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, m_fbo_width, m_fbo_height);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // extra credit 1
    extraCredit1();

    //    if(settings.extraCredit4) {
    //    /************************texture mapping extra
    //    credit********************************/
    //        // m_fullscreen_vao has data of full quad
    //        glBindVertexArray(m_fullscreen_vao);
    //        // Task 10: Bind "texture" to slot 0
    //        // Bind the input parameter texture to texture slot 0;
    //        GL_TEXTURE_2D is bound to GL_TEXTURE0 in initializeGL.
    //        // basically, this is binding texture (m_fbo_texture) to
    //        GL_TEXTURE0, which means telling sampler GL_TEXTURE0 where to
    //        sample from. glBindTexture(GL_TEXTURE_2D, m_shape_texture);

    //        // since m_fullscreen_vao is bound, six uv coordinates in quad
    //        data will be sent glDrawArrays(GL_TRIANGLES, 0, 6);// 6 or
    //        fullscreen_quad_data.size() / 5

    //        glBindTexture(GL_TEXTURE_2D, 0);
    //        glBindVertexArray(0);
    //    /************************texture mapping extra
    //    credit********************************/
    //    }

    // uses defualt.frag and default.vert and m_shader...
    // doesn't matter which shader/vert/frag is used.. as long as FBO is used..
    // shapes will be drawn to m_fbo_texture. why? because m_fbo_texture is
    // attached to m_fbo in makeFBO(). m_fbo_texture will be drawn to default
    // FBO by calling paintTexture(m_fbo_texture)
    paintShapes(); // == paintExampleGeometry();

    /**************** project 6 ***************/
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0, 0, m_screen_width * 1.25f, m_screen_height * 1.25f);

    //    glClearColor(0,0,1,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // uses m_texture_shader, texture.frag, texture.vert
    // paint texture that's drawn on FBO? to default?
    paintTexture(m_fbo_texture);
    // paintTexture(m_shape_texture);
}

void Realtime::uniformSharpenFilter() {
    std::vector<float> filter1{0.f, 0.f, 0.f, 0, 2.f, 0, 0, 0, 0};
    std::vector<float> filter2{1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};
    for (int i = 0; i < 9; ++i) {
        GLuint ele1 = glGetUniformLocation(
            m_texture_shader, ("filter1[" + std::to_string(i) + "]").c_str());
        glUniform1f(ele1, filter1[i]);

        GLuint ele2 = glGetUniformLocation(
            m_texture_shader, ("filter2[" + std::to_string(i) + "]").c_str());
        glUniform1f(ele2, filter2[i]);
    }
}

void Realtime::uniformEdgeFilter() {
    GLuint edge = glGetUniformLocation(m_texture_shader, "isEdgeFilterOn");
    glUniform1i(edge, true);

    std::vector<float> filter1{1.f, 1.f, 1.f, 1, -8.f, 1, 1, 1, 1};

    for (int i = 0; i < 9; ++i) {
        GLuint ele1 = glGetUniformLocation(
            m_texture_shader, ("filter1[" + std::to_string(i) + "]").c_str());
        glUniform1f(ele1, filter1[i]);
    }
}

// paint texture drawn to myFBO back to default FBO
void Realtime::paintTexture(GLuint texture) {
    glUseProgram(m_texture_shader);

    // save filter1 and filter2 data as uniform in texture.frag
    uniformSharpenFilter();
    // kernel filter extra credit
    // uniformEdgeFilter();

    GLuint textureWidthID =
        glGetUniformLocation(m_texture_shader, "textureWidth");
    glUniform1i(textureWidthID, m_fbo_width);

    GLuint textureHeightID =
        glGetUniformLocation(m_texture_shader, "textureHeight");
    glUniform1i(textureHeightID, m_fbo_height);
    // Task 32: Set your bool uniform on whether or not to filter the texture
    // drawn
    GLuint togglePixelID =
        glGetUniformLocation(m_texture_shader, "isPixelPostProcessingOn");
    glUniform1i(togglePixelID, settings.perPixelFilter);

    GLuint toggleKernelID =
        glGetUniformLocation(m_texture_shader, "isKernelPostProcessingOn");
    glUniform1i(toggleKernelID, settings.kernelBasedFilter);

    if (settings.extraCredit4) {
        // glBindVertexArray(m_cube_vao);
    } else {
        // m_fullscreen_vao has data of full quad
        glBindVertexArray(m_fullscreen_vao);
    }
    // Task 10: Bind "texture" to slot 0
    // Bind the input parameter texture to texture slot 0; GL_TEXTURE_2D is
    // bound to GL_TEXTURE0 in initializeGL. basically, this is binding texture
    // (m_fbo_texture) to GL_TEXTURE0, which means telling sampler GL_TEXTURE0
    // where to sample from.
    glBindTexture(GL_TEXTURE_2D, texture);

    if (settings.extraCredit4) {
        //        glDrawArrays(GL_TRIANGLES, 0, m_cubeData.size()/8);
    } else {
        // since m_fullscreen_vao is bound, six uv coordinates in quad data will
        // be sent
        glDrawArrays(GL_TRIANGLES, 0,
                     6); // 6 or fullscreen_quad_data.size() / 5
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

glm::mat4 Realtime::generateProjectionMatrix() {
    // need to use theta_width and theta height (or aspect ratio?)
    // aspect ratio = width / height
    float heightAngle = metaData.cameraData.heightAngle; // in radians
    float ratio = windowWidth / windowHeight;
    float widthAngle = heightAngle * (1 / ratio);
    widthAngle = 2 * atan(windowWidth / windowHeight * tan(heightAngle / 2.f));

    // if I have width and height, I don't need angles
    glm::mat4 mR =
        glm::mat4(1.f / (settings.farPlane *
                         (windowWidth / windowHeight * tan(heightAngle / 2.f))),
                  0, 0, 0, // this is a column
                  0, 1.f / (settings.farPlane * tan(heightAngle / 2.f)), 0, 0,
                  0, 0, 1.f / settings.farPlane, 0, 0, 0, 0, 1);

    float c = -settings.nearPlane / settings.farPlane;
    glm::mat4 mM = glm::mat4(1.f, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 / (1 + c), -1, 0,
                             0, -c / (1.f + c), 0);

    glm::mat4 mL =
        glm::mat4(1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, -2.f, 0, 0, 0, -1.f, 1.f);
    return mL * mM * mR;
}

glm::mat4 Realtime::generateViewMatrix() {
    glm::vec3 vec3Look = glm::vec3(metaData.cameraData.look);
    glm::vec3 vec3Up = glm::vec3(metaData.cameraData.up);

    glm::vec3 w = -glm::normalize(vec3Look);
    glm::vec3 v = glm::normalize(vec3Up - glm::dot(vec3Up, w) * w);

    glm::vec3 u = glm::cross(v, w);

    glm::mat4 R =
        glm::transpose(glm::mat4{glm::vec4{u, 0.f}, glm::vec4{v, 0.f},
                                 glm::vec4{w, 0.f}, glm::vec4{0.f, 0, 0, 1}});

    glm::mat4 T{glm::vec4{1.f, 0, 0, 0}, glm::vec4{0.f, 1, 0, 0},
                glm::vec4{0.f, 0, 1, 0},
                glm::vec4{glm::vec3{-metaData.cameraData.pos}, 1}};

    glm::mat4 viewMatrix = R * T;

    return viewMatrix;
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio,
               size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should
    // be done here

    // use w and h to correctly update project matrix?
    // can't use perspective
    // m_proj = glm::perspective(glm::radians(45.0),1.0 * w / h,0.01,100.0); //
    // copied from lab10
    windowWidth = w;
    windowHeight = h;

    /** Project 6**/
    // Task 34: Delete Texture, Renderbuffer, and Framebuffer memory
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    // Task 34: Regenerate your FBOs
    makeFBO();

    m_proj = generateProjectionMatrix();
}

void Realtime::sceneChanged() {

    isSceneChanged = true;

    // parse here. yes load a scene here
    bool success = SceneParser::parse(":/directionalLightFinal.xml", metaData);

    if (!success) {
        std::cerr << "Error loading scene: \"" << settings.sceneFilePath << "\""
                  << std::endl;
        exit(1);
    } else {
        std::cout << "Successfully parsed" << std::endl;
    }
    makeCurrent();
    if (isInitializedRun) {
        glDeleteBuffers(1, &m_sphere_vbo);
        glDeleteVertexArrays(1, &m_sphere_vao);

        glDeleteBuffers(1, &m_cube_vbo);
        glDeleteVertexArrays(1, &m_cube_vao);

        glDeleteBuffers(1, &m_cone_vbo);
        glDeleteVertexArrays(1, &m_cone_vao);

        glDeleteBuffers(1, &m_cylinder_vbo);
        glDeleteVertexArrays(1, &m_cylinder_vao);
    }
    // call codes in initializeGL

    int param1 = settings.shapeParameter1;
    int param2 = settings.shapeParameter2;

    initializeSphere(param1, param2);

    initializeCube(param1);

    // save textures of shapes. final project
    if (settings.texture)
        saveTextures();

    // clean up binding
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // final project
    for (auto &shape : metaData.shapes) {
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
            shape.isFalling = true;
            shape.stop = false;
            shape.speed = 0.f;
            shape.onlyOnce = true;
            shape.radius = 0.5;
            shape.sign = -1;
            shape.v = glm::vec3{0};
            defaultSphere = shape;
        }
    }

    setPlaneParams();

    update(); // asks for a PaintGL() call to occur
}

void Realtime::saveTextures() {
    for (auto &shape : metaData.shapes) {
        std::string textureFileName =
            shape.primitive.material.textureMap.filename;
        QString qstrTextureFileName = QString::fromStdString(textureFileName);

        QImage qImage;
        if (!qImage.load(qstrTextureFileName)) {
            std::cout << "Your Image Path: "
                      << qstrTextureFileName.toStdString()
                      << " is not valid or there's no texture." << std::endl;
            // exit(1);
            QImage image = QPixmap(10, 10).toImage(); // garbage image
            textures.push_back(image);
        } else {
            m_image = QImage(qstrTextureFileName);
            m_image =
                m_image.convertToFormat(QImage::Format_RGBA8888).mirrored();

            textures.push_back(m_image);
        }
    }
}

void Realtime::settingsChanged() {
    if (isInitializedRun) {
        makeCurrent();

        int param1 = settings.shapeParameter1;
        int param2 = settings.shapeParameter2;

        // params
        initializeCube(param1);

        // call codes in initializeGL
        initializeSphere(param1, param2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        update(); // asks for a PaintGL() call to occur;
    }
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos =
            glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        // rotate horizontally
        float c = glm::cos(deltaX / 1000.f);
        float s = glm::sin(deltaX / 1000.f);
        float x = 0.f;
        float y = 1.f;
        float z = 0.f;

        glm::mat4 rotate((1 - c) * pow(x, 2) + c, (1 - c) * x * y + s * z,
                         (1 - c) * x * z - s * y, 0, (1 - c) * x * y - s * z,
                         (1 - c) * pow(y, 2) + c, (1 - c) * y * z + s * x, 0,
                         (1 - c) * x * z + s * y, (1 - c) * y * z - s * x,
                         (1 - c) * pow(z, 2) + c, 0, 0, 0, 0, 1);

        metaData.cameraData.look = rotate * metaData.cameraData.look;

        // rotate vertically
        glm::vec3 perpLookAndUp =
            glm::cross(glm::vec3(metaData.cameraData.look),
                       glm::vec3(metaData.cameraData.up));

        c = glm::cos(deltaY / 100000.f);
        s = glm::sin(deltaY / 100000.f);
        x = perpLookAndUp.x;
        y = perpLookAndUp.y;
        z = perpLookAndUp.z;

        rotate = glm::mat4((1 - c) * pow(x, 2) + c, (1 - c) * x * y + s * z,
                           (1 - c) * x * z - s * y, 0, (1 - c) * x * y - s * z,
                           (1 - c) * pow(y, 2) + c, (1 - c) * y * z + s * x, 0,
                           (1 - c) * x * z + s * y, (1 - c) * y * z - s * x,
                           (1 - c) * pow(z, 2) + c, 0, 0, 0, 0, 1);

        metaData.cameraData.look = rotate * metaData.cameraData.look;
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::addBall() {
    // adds a ball at a random position
    // Add a shape to renderData and call paintGL again
    defaultSphere.index = 1;
    metaData.shapes.push_back(defaultSphere);
    std::cout << "pushed" << std::endl;
    update();
}

glm::vec4 Realtime::getShapeCenterWorldSpace(RenderShapeData &shape) {
    return m_proj * m_view * shape.ctm * glm::vec4{0.f, 0.0, 0, 1};
}

glm::vec4 Realtime::getShapeLowestPoint(RenderShapeData &shape) {
    return m_proj * m_view * shape.ctm * glm::vec4{0.f, -0.5, 0, 1};
}

void Realtime::translateSphereTowardRight(float &deltaTime) {
    for (auto &shape : metaData.shapes) {
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
            float c = glm::cos(deltaTime);
            float s = glm::sin(deltaTime);
            float x = 0.f;
            float y = 0.f;
            float z = 1.f;

            glm::mat4 translate(
                (1 - c) * pow(x, 2) + c, (1 - c) * x * y + s * z,
                (1 - c) * x * z - s * y, 0, (1 - c) * x * y - s * z,
                (1 - c) * pow(y, 2) + c, (1 - c) * y * z + s * x, 0,
                (1 - c) * x * z + s * y, (1 - c) * y * z - s * x,
                (1 - c) * pow(z, 2) + c, 0, 0, 0, 0, 1);
            glm::mat4 invTranslate = glm::inverse(translate);
            shape.ctm = invTranslate * shape.ctm;
        }
    }
}

void Realtime::translateSphereTowardLeft(float &deltaTime) {
    for (auto &shape : metaData.shapes) {
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
            float c = glm::cos(deltaTime);
            float s = glm::sin(deltaTime);
            float x = 0.f;
            float y = 0.f;
            float z = 1.f;

            glm::mat4 translate(
                (1 - c) * pow(x, 2) + c, (1 - c) * x * y + s * z,
                (1 - c) * x * z - s * y, 0, (1 - c) * x * y - s * z,
                (1 - c) * pow(y, 2) + c, (1 - c) * y * z + s * x, 0,
                (1 - c) * x * z + s * y, (1 - c) * y * z - s * x,
                (1 - c) * pow(z, 2) + c, 0, 0, 0, 0, 1);

            //            glm::mat4 rotateCWZ{cos(deltaTime), -sin(deltaTime),
            //            0, 0,
            //            // first column
            //                             sin(deltaTime), cos(deltaTime), 0,
            //                             0.f, 0,0,1,0, 0,0,0,1};
            //            shape.ctm = rotateCWZ * shape.ctm;

            shape.ctm = translate * shape.ctm;
        }
    }
}

void Realtime::tiltFloor(RenderShapeData &shape, float &deltaTime) {
    // rotate counter clockwise
    glm::mat4 rotateCCWZ{
        cos(deltaTime),  sin(deltaTime), 0, 0, // first column
        -sin(deltaTime), cos(deltaTime), 0, 0.f, 0, 0, 1, 0, 0, 0, 0, 1};

    // rotate clockwise
    glm::mat4 rotateCWZ{
        cos(deltaTime), -sin(deltaTime), 0, 0, // first column
        sin(deltaTime), cos(deltaTime),  0, 0.f, 0, 0, 1, 0, 0, 0, 0, 1};

    glm::mat4 rotateCWX{1,
                        0,
                        0,
                        0,
                        0,
                        cos(deltaTime),
                        sin(deltaTime),
                        0,
                        0,
                        -sin(deltaTime),
                        cos(deltaTime),
                        0,
                        0,
                        0,
                        0,
                        1};

    glm::mat4 rotateCCWX{1,
                         0,
                         0,
                         0,
                         0,
                         cos(deltaTime),
                         -sin(deltaTime),
                         0,
                         0,
                         sin(deltaTime),
                         cos(deltaTime),
                         0,
                         0,
                         0,
                         0,
                         1};

    if (m_keyMap[Qt::Key_Left] == true) {
        shape.ctm = rotateCCWZ * shape.ctm;
        translateSphereTowardLeft(deltaTime);
    } else if (m_keyMap[Qt::Key_Right] == true) {
        shape.ctm = rotateCWZ * shape.ctm;

        // translate sphere accordingly as floor rotates.
        translateSphereTowardRight(deltaTime);

    } else if (m_keyMap[Qt::Key_Down] == true) {
        shape.ctm = rotateCWX * shape.ctm;
    } else if (m_keyMap[Qt::Key_Up] == true) {
        shape.ctm = rotateCCWX * shape.ctm;
    }
}

void ::Realtime::setPlaneParams() {
    metaData.planeParams = {};
    for (auto &shape : metaData.shapes) {
        if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
            // find parameters A, B, C, D for plane in here
            for (auto plane : cubePlanes) {
                for (auto &point : plane) {
                    point = shape.ctm * point;
                    // import vertices
                    metaData.cubeVertices.push_back(point);
                }
                glm::vec3 normal =
                    glm::normalize(glm::cross(glm::vec3(plane[1] - plane[0]),
                                              glm::vec3(plane[1] - plane[3])));
                float D = -glm::dot(normal, glm::vec3(plane[2]));
                metaData.planeParams.push_back({normal, D});
            }
        }
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();
    if (isSceneChanged) {
        time = 1e-2;
    }

    if (settings.finalProject) {
        // final project portion
        for (auto &shape : metaData.shapes) {
            if (shape.primitive.type == PrimitiveType::PRIMITIVE_CUBE) {
                if (m_keyMap[Qt::Key_Left]) {
                    // multiply CTM when left key is pressed
                    glm::mat4 rotationMatLeft =
                        glm::rotate(deltaTime * 0.1f, glm::vec3{0, -1, 0});
                    shape.ctm = rotationMatLeft * shape.ctm;
                }
                if (m_keyMap[Qt::Key_Right]) {
                    // multiply CTM when left key is pressed
                    glm::mat4 rotationMatRight =
                        glm::rotate(deltaTime * 0.1f, glm::vec3{0, 1, 0});
                    shape.ctm = rotationMatRight * shape.ctm;
                }
                if (m_keyMap[Qt::Key_Up]) {
                    // multiply CTM when left key is pressed
                    glm::mat4 rotationMatUp =
                        glm::rotate(deltaTime * 0.1f, glm::vec3{-1, 0, 0});
                    shape.ctm = rotationMatUp * shape.ctm;
                }
                if (m_keyMap[Qt::Key_Down]) {
                    // multiply CTM when left key is pressed
                    glm::mat4 rotationMatDown =
                        glm::rotate(deltaTime * 0.1f, glm::vec3{1, 0, 0});
                    shape.ctm = rotationMatDown * shape.ctm;
                }
                setPlaneParams();
            }

            // find bounding points for cube
            float minX = INFINITY, minY = INFINITY, minZ = INFINITY,
                  maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;
            for (auto &vertex : metaData.cubeVertices) {
                if (vertex[0] > maxX) {
                    maxX = vertex[0];
                }
                if (vertex[1] > maxY) {
                    maxY = vertex[1];
                }
                if (vertex[2] > maxZ) {
                    maxZ = vertex[2];
                }
                if (vertex[0] < minX) {
                    minX = vertex[0];
                }
                if (vertex[1] < minY) {
                    minY = vertex[1];
                }
                if (vertex[2] < minZ) {
                    minZ = vertex[2];
                }
            }
            if (shape.primitive.type == PrimitiveType::PRIMITIVE_SPHERE) {
                glm::vec3 &v = shape.v;
                // if shape's implicit world space position is within
                // [radius] units from any point on the cube, bounce can get
                // shape's implicit WSP from object space position * ctm
                glm::vec4 worldSpacePosition =
                    shape.ctm * glm::vec4{0, 0, 0, 1};

                // confirm it is within box

                if (worldSpacePosition[0] > minX &&
                    worldSpacePosition[0] < maxX &&
                    worldSpacePosition[1] > minY &&
                    worldSpacePosition[1] < maxY &&
                    worldSpacePosition[2] > minZ &&
                    worldSpacePosition[2] < maxZ) {

                    float minDistance = INFINITY; // for testing
                    glm::vec4 currentParam;
                    for (auto &param : metaData.planeParams) {

                        // distance from point to a plane
                        float distance =
                            abs(glm::dot(worldSpacePosition, param)) /
                            glm::length(glm::vec3(param));

                        //                        std::cout <<
                        //                        glm::to_string(param) <<
                        //                        distance
                        //                                  << std::endl;
                        if (distance < minDistance) {
                            minDistance = distance;
                            currentParam = param;
                        }
                        if (distance < shape.radius) {
                            // check if ball is moving away from box
                            glm::vec3 directionToCenter =
                                glm::vec3{0, 0, 0} -
                                glm::vec3(worldSpacePosition);
                            if (glm::dot(shape.v, directionToCenter) < 0) {
                                v = glm::reflect(v, glm::vec3(param));
                            }
                        }
                    }
                    //                    std::cout << minDistance
                    //                              <<
                    //                              glm::to_string(worldSpacePosition)
                    //                              << glm::length(shape.v) <<
                    //                              std::endl;

                    // Check collision with other spheres
                    for (auto &otherShape : metaData.shapes) {
                        if (otherShape.primitive.type ==
                            PrimitiveType::PRIMITIVE_SPHERE) {
                            // make sure it's not our own
                            if (otherShape.ctm != shape.ctm) {
                                glm::vec4 otherShapeWorldPosition =
                                    otherShape.ctm * glm::vec4{0, 0, 0, 1};
                                // Check for distance, knowing that all spheres
                                // have radius 0.5
                                glm::vec3 distance =
                                    glm::vec3(otherShapeWorldPosition) -
                                    glm::vec3(worldSpacePosition);
                                if (glm::length(distance) <= 1) {
                                    // find point of intersection, reflect
                                    // across normal
                                    v = glm::reflect(v, glm::vec3{0, -1, 0});
                                }
                            }
                        }
                    }
                }
                // update position
                glm::mat4 transformation = glm::translate(
                    v * deltaTime +
                    (GRAVITY * deltaTime * deltaTime) * (float)0.5);
                // update velocity
                v += GRAVITY * deltaTime * (float)0.5;
                shape.ctm = shape.ctm * transformation;

                // add sphere's current position to
            }
        }
    }

    // Use deltaTime and m_keyMap here to move around
    if (m_keyMap[Qt::Key_W] == true) {
        metaData.cameraData.pos += deltaTime * metaData.cameraData.look;
        //        m_view = generateViewMatrix();
    } else if (m_keyMap[Qt::Key_S] == true) {
        metaData.cameraData.pos -= deltaTime * metaData.cameraData.look;
    } else if (m_keyMap[Qt::Key_A] == true) {
        glm::vec3 dir = glm::cross(glm::vec3(metaData.cameraData.up),
                                   glm::vec3(metaData.cameraData.look));
        metaData.cameraData.pos += deltaTime * glm::vec4(dir, 0.f);
    } else if (m_keyMap[Qt::Key_D] == true) {
        glm::vec3 dir = glm::cross(glm::vec3(metaData.cameraData.look),
                                   glm::vec3(metaData.cameraData.up));
        metaData.cameraData.pos += deltaTime * glm::vec4(dir, 0.f);
    } else if (m_keyMap[Qt::Key_Space] == true) {
        metaData.cameraData.pos += 5 * deltaTime * glm::vec4(0, 1.f, 0, 0);
    } else if (m_keyMap[Qt::Key_Control] == true) {
        metaData.cameraData.pos += 5 * deltaTime * glm::vec4(0, -1.f, 0, 0);
    }

    update(); // asks for a PaintGL() call to occur
}
