#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/shader.h"
#include "graphics/tile.h"
#include "graphics/line.h"
#include "graphics/gui/font/font.h"
#include "input/input.h"
#include "utils/random.h"
#include "utils/fft.h"
#include "audio/sound.h"
#include "game/game.h"

#include <ft2build.h>
#include FT_FREETYPE_H

const int32_t width = 1920, height = 1080;

const uint32_t framerate = 120;

extern void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

extern void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);

/**
 * C++ 20 | OpenGL 3.3.0 | MinGW CMAKE
 */
int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize glfw";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow *window = glfwCreateWindow(width, height, "Rhythm Game", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Error creating window!";
        glfwTerminate();
        return -1;
    }

    const GLFWvidmode *vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwSetWindowPos(window, (vidmode->width - width) / 2, (vidmode->height - height) / 2);

    glfwMakeContextCurrent(window);
    glfwShowWindow(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    std::clog << "OpenGL: " << glGetString(GL_VERSION) << "\n";

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetErrorCallback([](int error, const char *description) -> void {
        std::cerr << "Error " << error << ": " << description << std::endl;
    });

    if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT") == AL_FALSE) {
        std::cerr << "Enumeration extension not available!" << std::endl;
    }

    ALCdevice *device = alcOpenDevice(alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER));
    if (!device) {
        std::cerr << "Unable to open default device!" << std::endl;
        return -1;
    }

    std::clog << "OpenAL Device: " << alcGetString(device, ALC_DEVICE_SPECIFIER) << std::endl;

    ALCcontext *context = alcCreateContext(device, nullptr);
    if (!alcMakeContextCurrent(context)) {
        std::cerr << "Failed to make default context!" << std::endl;
        return -1;
    }

    alListener3f(AL_POSITION, 0, 0, 1.0f);
    alListener3f(AL_VELOCITY, 0, 0, 0);
    ALfloat listenerOri[] = {0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};
    alListenerfv(AL_ORIENTATION, listenerOri);

    // Wire - Frame mode
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::string path = "assets/resources/yesterday.wav";

    Game level(path);

    Sound sound(path, false);

    int fps = 0;

    double currentTime, lastTime = glfwGetTime(), frameTime = glfwGetTime();

    sound.play();

    while (!glfwWindowShouldClose(window)) {
        if (input.is_key_down(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window, true);

        fps += 1;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        currentTime = glfwGetTime();

        if (currentTime - frameTime >= 1.0) {
            std::cout << fps << " fps\n";
            fps = 0, frameTime = currentTime;
        }

        // Todo. update
        level.update();

        if (currentTime - lastTime >= 1.0 / framerate) {
            lastTime = currentTime;
        }

        level.render();

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    sound.stop();
    level.clear();

    glfwTerminate();
    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}