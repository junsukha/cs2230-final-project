#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>
//#include "utils/shaderloader.h"
#include "utils/sceneparser.h"
//#include "utils/objreader.h"
//#include "utils/objreader2.h"

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();

    glm::mat4 generateViewMatrix();                     // my function
    glm::mat4 generateProjectionMatrix();

    // functions in paintGL()
    void paintGLSphere();
    void paintGLCube();
    void paintGLCone();
    void paintGLCylinder();

    // functions in initializeGL()
    void initializeCone(int param1, int param2);
    void initializeCylinder(int param1, int param2);
    void initializeCube(int param1);
    void initializeSphere(int param1, int param2);

    std::vector<float> generateSphereData(int phiTesselations, int thetaTesselations);
    glm::vec4 sphericalToCartesian(float phi, float theta);
    void pushVec3(glm::vec4 vec, std::vector<float> *data);

    void makeFBO(); // project 6

    /**
     * @brief paintTexture paints texture drawn to m_fbo_texture to default FBO
     * @param texture is m_fbo_texture in my case where the scene is drawn to
     */
    void paintTexture(GLuint texture); // project 6

    /**
     * @brief extraCredit1 runs extra credit 1
     */
    void extraCredit1();

    /**
     * @brief paintShapes is in paintGL() and it print shapes
     */
    void paintShapes();

    /**
     * @brief uniformLight send light data that will be used as uniform variables to default.frag
     */
    void uniformLight();

    /**
     * @brief refreshLights set lights in uniform back to default
     */
    void refreshLights();

    /**
     * @brief fullScreenQuad cover full screen using two triangles and uses four corners of uv plane accordingly. I.e, bottomLeft corner of UV (0,0) matches with bottom Left corner of full screen quad (-1,-1,0)
     */
    void fullScreenQuad();

    /**
     * @brief uniformSharpenFilter saves uniform variable filter1 and filter2 in texture.frag
     */
    void uniformSharpenFilter();

    /**
     * @brief extraCredit2 is for extra credit 2.
     */
    void extraCredit2(RenderShapeData &shape, glm::vec4 cameraPositionWorldSpace);

    /**
     * @brief uniformShape saves uniform data of a shape and send them to default.frag
     * @param shape
     */
    void uniformShape(RenderShapeData &shape);
    void uniformEdgeFilter();

    /**
     * @brief extraCredit4 handles texture mapping
     */
    void extraCredit4(RenderShapeData &shape);

    /**
     * @brief checkShapeTypeAndDraw checks type of shape and draw corresponding mesh
     * @param shape
     */
    void checkShapeTypeAndDraw(RenderShapeData &shape);

    /**
     * @brief kitten is a garbage function made to test texture mapping
     */
    void kitten();
    glm::vec4 getShapeLowestPoint(RenderShapeData &shape);
    glm::vec4 getShapeCenterWorldSpace(RenderShapeData &shape);
public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // my variables
    glm::mat4 m_model = glm::mat4(1);
    glm::mat4 m_view = glm::mat4(1);
    glm::mat4 m_proj = glm::mat4(1);

    GLuint m_shader;
    GLuint m_texture_shader;
    // std::vector<GLuint> vbos{m_sphere_vbo, m_cube_vbo, m_cone_vbo, m_cylinder_vbo};
    // std::vector<GLuint> vaos{m_sphere_vao, m_cube_vao, m_cone_vao, m_cylinder_vao};

    GLuint m_sphere_vbo;
    GLuint m_cube_vbo;
    GLuint m_cone_vbo;
    GLuint m_cylinder_vbo;
    GLuint m_mesh_vbo;

    GLuint m_sphere_vao;
    GLuint m_cube_vao;
    GLuint m_cone_vao;
    GLuint m_cylinder_vao;
    GLuint m_mesh_vao;

    /** Project 6**/
    GLuint m_fbo;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    int m_screen_width;
    int m_screen_height;
    int m_fbo_width;
    int m_fbo_height;
    GLuint m_defaultFBO;
    // project 6 extra credit
    QImage m_image;
    GLuint m_shape_texture;
    GLuint m_kitten_texture;

    std::vector<float> m_sphereData;
    std::vector<float> m_cubeData;
    std::vector<float> m_cylinderData;
    std::vector<float> m_coneData;

    std::vector<float> mesh_data; // extra credit
//    objl::Loader loader;
//    std::vector<tinyobj::shape_t> shapes;
//    std::vector<tinyobj::material_t> materials;
//    tinyobj::attrib_t attrib;

    RenderData metaData;

    // light related
    glm::vec4 m_lightPos; // world space
    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;

    float windowWidth;
    float windowHeight;

    bool isInitializedRun = false;

    float originalParam1;
    float originalParam2;
    bool click = false;

    bool isFalling = true;
    float FLOORSURFACE = -0.243613; // when far plane = 100
    float GRAVITY = 9.8;
    float totalTime = 0;
    float initialTime;
    bool isSceneChanged = false;

    float time = 0.f;
    float v;
//    float v = 0;
//    float g = 10;
//    float t = 0;
//    float dt = 0.001;
//    float rho = 0.75;
//    float tau = 0.10;
//    float hmax = h0;
//    float h = h0;
//    float hstop = 0.01;

//    float t_last = -sqrt(2*h0/g) ;
//    float vmax = sqrt(2 * hmax * g);
};
