#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <iostream>
#include <string>

#define cimg_display 0
#include "lib/CImg.h"
#include "lib/quickgl.h"
#include "lib/scene.h"
#include "lib/camera.h"
#include "lib/bbox.h"
#include "common.h"

static const EGLint configAttribs[] = {
    EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
    EGL_BLUE_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_RED_SIZE, 8,
    EGL_DEPTH_SIZE, 8,
    EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
    EGL_NONE
};


static const int pbufferWidth = 500;
static const int pbufferHeight = 500;

static const EGLint pbufferAttribs[] = {
    EGL_WIDTH, pbufferWidth,
    EGL_HEIGHT, pbufferHeight,
    EGL_NONE,
};


static GLuint framebufferID;
static GLuint depthbufferID;
static GLenum drawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
static GLuint renderTarget;

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
    // "\nlayout(location=0, component=1) out float outColor;"
    "\nout vec4 outColor;"
    "\nconst float far = 20.0;"
    "\nconst float near = 1.0;"
    "\nvoid main() {"
    "\n    gl_FragDepth = (1.0 / gl_FragCoord.w - near) / (far - near);"
    "\n    float z = gl_FragDepth;"
    "\n    outColor = vec4(z, z, z, 1.0);"
    // "\n    outColor = z;"
    "\n}";

// ---------------------------------------

void setup() {
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

    // set up render-to-texture
    CHECK_GL_ERROR(glGenFramebuffers(1, &framebufferID));
    CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, framebufferID));
    CHECK_GL_ERROR(glGenTextures(1, &renderTarget));
    CHECK_GL_ERROR(glBindTexture(GL_TEXTURE_2D, renderTarget));
    CHECK_GL_ERROR(glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, pbufferWidth, pbufferHeight, 0, GL_RED, GL_FLOAT, 0));
    CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    CHECK_GL_ERROR(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    CHECK_GL_ERROR(glGenRenderbuffers(1, &depthbufferID));
    CHECK_GL_ERROR(glBindRenderbuffer(GL_RENDERBUFFER, depthbufferID));
    CHECK_GL_ERROR(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, pbufferWidth, pbufferHeight));
    CHECK_GL_ERROR(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthbufferID));
    CHECK_GL_ERROR(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderTarget, 0));
    CHECK_GL_ERROR(glDrawBuffers(1, drawBuffers));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "framebuffer incomplete!" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void save(std::string filename) {
    // -------------------------------------------------------------------------
    std::vector<float> pixels(pbufferWidth * pbufferHeight * sizeof(float));
    CHECK_GL_ERROR(glReadPixels(0, 0, pbufferWidth, pbufferHeight, GL_RED, GL_FLOAT,  pixels.data()));

    cimg_library::CImg<float> img(pbufferHeight, pbufferWidth);
    for (int r = 0; r < pbufferHeight; r++) {
        for (int c = 0; c < pbufferWidth; c++) {
            float color[] = {
                255 * pixels[r * pbufferWidth + c],
                255 * pixels[r * pbufferWidth + c],
                255 * pixels[r * pbufferWidth + c]
            };
            img.draw_point(c, pbufferHeight - 1 - r, color);        // flip for OpenGL framebuffer
        }
    }
    img.normalize(0.0, 255.0);
    img.save(filename.c_str());
}

void render(Camera& camera, Scene& scene) {
    CHECK_GL_ERROR(glBindFramebuffer(GL_FRAMEBUFFER, framebufferID));

    CHECK_GL_ERROR(glEnable(GL_DEPTH_TEST));
    CHECK_GL_ERROR(glDepthFunc(GL_LESS));
    CHECK_GL_ERROR(glViewport(0, 0, pbufferWidth, pbufferHeight));
    CHECK_GL_ERROR(glClearColor(1.0, 1.0, 1.0, 1.0));
    CHECK_GL_ERROR(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    CHECK_GL_ERROR(glUseProgram(shaderProgram));
    scene.render(shaderProgram, camera, glm::mat4());
    CHECK_GL_ERROR(glUseProgram(0));

    CHECK_GL_ERROR(glFlush());
}

void clean() {
    CHECK_GL_ERROR(glDeleteFramebuffers(1, &framebufferID));
    CHECK_GL_ERROR(glDeleteRenderbuffers(1, &depthbufferID));
    CHECK_GL_ERROR(glDeleteTextures(1, &renderTarget));

    CHECK_GL_ERROR(glDeleteShader(vertShader));
    CHECK_GL_ERROR(glDeleteShader(fragShader));
    CHECK_GL_ERROR(glDeleteProgram(shaderProgram));
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
#if 0
        std::cerr << "usage: ./depth-render-egl <path-to-model>" << std::endl;
#else
        std::cerr << "usage: ./depth-render-egl <path-to-file>" << std::endl;
#endif
        exit(EXIT_FAILURE);
    }
    std::vector<Config> configs;
    parse_config(std::string(argv[1]), configs);

    // 1. Initialize EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    EGLint major, minor;

    eglInitialize(eglDpy, &major, &minor);

    // 2. Select an appropriate configuration
    EGLint numConfigs;
    EGLConfig eglCfg;

    eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);

    // 3. Create a surface
    EGLSurface eglSurf = eglCreatePbufferSurface(eglDpy, eglCfg, pbufferAttribs);

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT, NULL);

    eglMakeCurrent(eglDpy, eglSurf, eglSurf, eglCtx);

    // from now on use your OpenGL context
    setup();
    // --------------------------------------------------------------
    float eyeDist = 10.0f;
    float minDist = 2.0f;

    Camera camera;
    camera.lookAt(
        glm::vec3(0.0f, 0.0f, eyeDist),     // eye
        glm::vec3(0.0f, 0.0f, 0.0f),        // center
        glm::vec3(0.0f, 1.0f, 0.0f)         // up
    );
    camera.perspective(
        glm::radians(45.0f),                                    // fov
        (float) pbufferWidth / (float) pbufferHeight,           // aspect ratio
        1.0f,                                                   // near plane
        20.0f                                                   // far plane
    );

    // --------------------------------------------------------------
    for (auto config : configs) {
        Scene scene;
        scene.load(config.inputFile);
        BoundingBox bbox = scene.getBoundingBox();
        float scaleX = (eyeDist - minDist - bbox.front) / (bbox.right - bbox.left);
        float scaleY = (eyeDist - minDist - bbox.front) / (bbox.top - bbox.bottom);
        float scaleZ = (eyeDist - minDist - bbox.front) / (bbox.front - bbox.back);
        float scale = std::min(std::min(scaleX, scaleY), scaleZ);

        for (size_t i = 0; i < config.outputFiles.size(); i++) {
            glm::vec2 rotation = config.rotations[i];
            std::string outputFile = config.outputFiles[i];

            scene.resetTransform();
            scene.rotate(rotation.x, 0, 1, 0);              // longiture
            scene.rotate(rotation.y, 1, 0, 0);              // latitude
            scene.scale(scale, scale, scale);
            scene.moveToCenter();

            render(camera, scene);
            save(outputFile);

            CHECK_GL_ERROR(eglSwapBuffers(eglDpy, eglSurf));
        }
    }
    clean();

    // 6. Terminate EGL when finished
    eglTerminate(eglDpy);
    return 0;
}
