#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#include "lib/quickgl.h"
#include "lib/scene.h"
#include "lib/camera.h"
#include "common.h"

static bool mousePressed = false;
static int mouseX = 0;
static int mouseY = 0;
static int prevMouseX = 0;
static int prevMouseY = 0;
static glm::mat4 globalXform = glm::mat4();
static GLenum polygonMode = GL_FILL;

static void error_callback(int error, const char* description)
{
    fputs(description, stderr);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (polygonMode == GL_FILL) polygonMode = GL_LINE;
        else if (polygonMode == GL_LINE) polygonMode = GL_FILL;
        CHECK_GL_ERROR(glPolygonMode(GL_FRONT_AND_BACK, polygonMode));
    }
}

static void mousePosEvent(GLFWwindow* window, double xpos, double ypos) {
    prevMouseX = mouseX;
    prevMouseY = mouseY;
    mouseX = xpos;
    mouseY = ypos;
    if (mousePressed) {
        float diffX = mouseX - prevMouseX;
        globalXform = glm::rotate(globalXform, glm::radians(diffX), glm::vec3(0.0, 1.0, 0.0));
    }
}

static void mouseButtonEvent(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS)
        mousePressed = true;
    else if (action == GLFW_RELEASE)
        mousePressed = false;
}

static void mouseScrollEvent(GLFWwindow* window, double xoffset, double yoffset) {
    if (yoffset > 0)
        globalXform = glm::scale(globalXform, 1.1f * glm::vec3(1.0, 1.0, 1.0));
    else if (yoffset < 0)
        globalXform = glm::scale(globalXform, 0.9f * glm::vec3(1.0, 1.0, 1.0));
}

static float ratio;
static int pbufferWidth, pbufferHeight;

static GLuint shaderProgram;
static GLuint vertShader;
static GLuint fragShader;

static const char* vertShaderSrc =
    "#version 450"
    "\nuniform mat4 model;"
    "\nuniform mat4 view;"
    "\nuniform mat4 proj;"
    "\nlayout(location = 0) in vec3 inPosition;"
    "\nlayout(location = 1) in vec3 inColor;"
    "\nout vec3 fragColor;"
    "\nout gl_PerVertex {"
    "\n    vec4 gl_Position;"
    "\n};"
    "\nvoid main() {"
    "\n    gl_Position = proj * view * model * vec4(inPosition, 1.0);"
    "\n    fragColor = inColor;"
    "\n}";

static const char* fragShaderSrc =
    "#version 450"
    "\nin vec3 fragColor;"
    "\nout vec4 outColor;"
    "\nconst float far = 20.0;"
    "\nconst float near = 1.0;"
    "\nvoid main() {"
    "\n    float z = (1.0 / gl_FragCoord.w - near) / (far - near);"
    "\n    outColor = vec4(z, z, z, 1.0);"
    "\n}";

// ---------------------------------------

static void setup() {
    glewExperimental=GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "glew init fails!" << std::endl;
        exit(EXIT_SUCCESS);
    }
    // set up shader program
    CHECK_GL_ERROR(vertShader = glCreateShader(GL_VERTEX_SHADER));
    CHECK_GL_ERROR(fragShader = glCreateShader(GL_FRAGMENT_SHADER));
    int vlength = strlen(vertShaderSrc) + 1;
    int flength = strlen(fragShaderSrc) + 1;
    CHECK_GL_ERROR(glShaderSourceARB(vertShader, 1, &vertShaderSrc, &vlength));
    CHECK_GL_ERROR(glShaderSourceARB(fragShader, 1, &fragShaderSrc, &flength));
    CHECK_GL_ERROR(glCompileShaderARB(vertShader));
    CHECK_GL_ERROR(glCompileShaderARB(fragShader));

    CheckShaderCompilation(vertShader);
    CheckShaderCompilation(fragShader);

    CHECK_GL_ERROR(shaderProgram = glCreateProgram());
    CHECK_GL_ERROR(glAttachShader(shaderProgram, vertShader));
    CHECK_GL_ERROR(glAttachShader(shaderProgram, fragShader));
    CHECK_GL_ERROR(glLinkProgram(shaderProgram));

    CheckProgramLinkage(shaderProgram);
}

static void clean() {
    CHECK_GL_ERROR(glDeleteShader(vertShader));
    CHECK_GL_ERROR(glDeleteShader(fragShader));
    CHECK_GL_ERROR(glDeleteProgram(shaderProgram));
}

static void render(GLFWwindow* window, Camera& camera, Scene& scene)
{
    CHECK_GL_ERROR(glDisable(GL_CULL_FACE));
    CHECK_GL_ERROR(glEnable(GL_DEPTH_TEST));
    CHECK_GL_ERROR(glDepthFunc(GL_LESS));
    CHECK_GL_ERROR(glViewport(0, 0, pbufferWidth, pbufferHeight));
    CHECK_GL_ERROR(glClearColor(1.0, 1.0, 1.0, 1.0));
    CHECK_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    CHECK_GL_ERROR(glUseProgram(shaderProgram));
    scene.render(shaderProgram, camera, globalXform);
    CHECK_GL_ERROR(glUseProgram(0));

    // glBegin(GL_TRIANGLES);
    // for (auto mesh : scene.getMeshes()) {
    //     std::vector<Vertex>& vertices = mesh->getVertices();
    //     std::vector<uint32_t>& indices = mesh->getIndices();
    //     for (size_t i = 0; i < indices.size(); i++) {
    //         glm::vec3 pos = vertices[i].position;
    //         glm::vec3 col = vertices[i].position;
    //         glColor3f(col.r, col.g, col.b);
    //         glVertex3f(pos.x, pos.y, pos.z);
    //     }
    // }
    // glEnd();

    CHECK_GL_ERROR(glFlush());

    glfwSwapBuffers(window);

}

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        std::cerr << "usage: ./depth-render-glfw <path-to-model>" << std::endl;
        exit(EXIT_FAILURE);
    }

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(500, 500, "depth-render", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mousePosEvent);
    glfwSetMouseButtonCallback(window, mouseButtonEvent);
    glfwSetScrollCallback(window, mouseScrollEvent);

    glfwGetFramebufferSize(window, &pbufferWidth, &pbufferHeight);
    ratio = pbufferWidth / (float) pbufferHeight;

    setup();
    // --------------------------------------------------------------
    float eyeDist = 10.0f;
    float minDist = 2.0f;
    // --------------------------------------------------------------
    Scene scene;
    scene.load(argv[1]);
    BoundingBox bbox = scene.getBoundingBox();
    float scaleX = (eyeDist - minDist - bbox.front) / (bbox.right - bbox.left);
    float scaleY = (eyeDist - minDist - bbox.front) / (bbox.top - bbox.bottom);
    float scaleZ = (eyeDist - minDist - bbox.front) / (bbox.front - bbox.back);
    float scale = std::min(std::min(scaleX, scaleY), scaleZ);
    // std::cout << "scaleX: " << scaleX << std::endl
    //           << "scaleY: " << scaleY << std::endl
    //           << "scaleZ: " << scaleZ << std::endl;
    scene.scale(scale, scale, scale);
    scene.moveToCenter();
    // std::cout << bbox.center().x << ", " << bbox.center().y  << ", " << bbox.center().z << std::endl;

    Camera camera;
    camera.lookAt(
        glm::vec3(0.0f, 0.0f, eyeDist),     // eye
        glm::vec3(0.0f, 0.0f, 0.0f),        // center
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    camera.perspective(
        glm::radians(45.0f),                                   // fov
        ratio,                                                 // aspect ratio
        1.0,                                                   // near plane
        20.0f                                                  // far plane
    );
    // --------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        render(window, camera, scene);
    }
    clean();

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
