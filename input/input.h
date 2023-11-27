//
// Created by 김준용 on 2023-11-15.
//

#pragma once

extern struct Input {
    int keys[GLFW_KEY_LAST], buttons[GLFW_MOUSE_BUTTON_LAST];

    bool is_key_down(int key) {
        return keys[key] != GLFW_RELEASE;
    }

    bool is_mouse_down(int button) {
        return buttons[button] != GLFW_RELEASE;
    }
} input;
