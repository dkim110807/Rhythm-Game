//
// Created by 김준용 on 2023-11-15.
//

#include <GLFW/glfw3.h>

struct Input {
    int keys[GLFW_KEY_LAST], buttons[GLFW_MOUSE_BUTTON_LAST];

    bool is_key_down(int key) {
        return keys[key] != GLFW_RELEASE;
    }

    bool is_mouse_down(int button) {
        return buttons[button] != GLFW_RELEASE;
    }
} input;

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key != GLFW_KEY_UNKNOWN)
        input.keys[key] = action;
}

void cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {

}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    input.buttons[button] = action;
}