//
// Created by 김준용 on 2023-11-22.
//

#ifndef GAME_H
#define GAME_H

#pragma once

#include <iostream>
#include <list>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../audio/sound.h"

#include "../graphics/gui/font/font.h"
#include "../graphics/hint.h"
#include "../graphics/line.h"
#include "../graphics/shader.h"
#include "../graphics/tile.h"

#include "../utils/fft.h"
#include "../utils/random.h"
#include "../utils/wav.h"

extern const int32_t width, height;

class Game {
private:
    const std::string path;
    const std::vector<std::vector<int>> permutation[5] = {
            {},
            {{1},       {2},       {3},       {4}},
            {{1, 2},    {1, 3},    {1, 4},    {2, 3}, {2, 4}, {3, 4}},
            {{1, 2, 3}, {1, 2, 4}, {1, 3, 4}, {2, 3, 4}},
            {{1, 2, 3, 4}}
    };
    const std::vector<int> keys = {GLFW_KEY_D, GLFW_KEY_F, GLFW_KEY_J, GLFW_KEY_K};

    Random<int> random = Random(0, 23);

    Shader text_shader = Shader("font.vert", "font.frag");
    Shader line_shader = Shader("line.vert", "line.frag");
    Shader tile_shader = Shader("tile.vert", "tile.frag");
    Shader hint_shader = Shader("hint.vert", "hint.frag");

    Font font = Font("Jetbrains.ttf");

    std::vector<Hint> hints;
    std::vector<Line> lines;
    std::vector<Tile> tiles;
    std::vector<double> tile_time = std::vector<double>(16, 0.0);

    std::list<int> lane_front[4];
    std::list<double> lane_time[4];

    Audio audio;

    double start_time;
    int current_time = -1;

    std::vector<int> peaks;

    int score = 0;

public:
    explicit Game(const std::string &path) : path(path) {
        audio = Audio(path);

        if (audio.length() <= 3000) {
            throw std::runtime_error("Audio file too short!");
        }

        for (auto &v: fft(audio, (int) audio.sample_rate() / 4, 20)) {
            peaks.push_back(std::min((int) v.size(), 4));
        }

        for (int i = -2; i <= 2; i++) {
            lines.emplace_back(glm::vec3(-2, 0, i * 0.05), glm::vec3(200, 0, i * 0.05), glm::vec3(1, 1, 1));
        }
        lines.emplace_back(glm::vec3(0, 0, -5), glm::vec3(0, 0, 5), glm::vec3(1, 0, 1));

        for (int i = 0; i < 16; i++) {
            tiles.emplace_back(glm::vec3(1, 0, 0));
            tiles[i].position = glm::vec3(235, 0, -0.075 + (i % 4) * 0.05);
        }

        for (int i = 0; i < 4; i++) {
            hints.emplace_back(glm::vec4(0.2f, 0.7f, 1.0f, 0.5f));
            hints[i].position.z = -0.075f + 0.05f * i;
        }

        glm::mat4 projection = glm::perspective(glm::radians(0.5f), (float) width / (float) height, 0.1f, 200.0f);
        glm::mat4 view = glm::lookAt(glm::vec3(-15, 0.05, 0), glm::vec3(200, -0.05, 0), glm::vec3(0, 1.f, 0.f));
        line_shader.enable();
        line_shader.setUniformMat4f("projection", projection);
        line_shader.setUniformMat4f("view", view);

        tile_shader.enable();
        tile_shader.setUniformMat4f("projection", projection);
        tile_shader.setUniformMat4f("view", view);

        hint_shader.enable();
        hint_shader.setUniformMat4f("projection", projection);
        hint_shader.setUniformMat4f("view", view);

        glm::mat4 orthographic = glm::ortho(0.0f, static_cast<float>(width), 0.0f, static_cast<float>(height));
        text_shader.enable();
        text_shader.setUniformMat4f("projection", orthographic);

        start_time = glfwGetTime();
    }

    void update() {
        double time = glfwGetTime();
        if (current_time != int((time - start_time) * 4)) {
            current_time = int((time - start_time) * 4);
            if (current_time < 8) return;
            // Todo. fix time
            if (current_time + 4 < peaks.size() && peaks[current_time + 4] != 0) {
                auto v = permutation[peaks[current_time + 4]][random() % permutation[peaks[current_time + 4]].size()];
                for (auto &x: v) {
                    tiles[(current_time % 4) * 4 + x - 1].position.x = 200;
                    tiles[(current_time % 4) * 4 + x - 1].color = glm::vec3(1, 0, 0);
                    tile_time[(current_time % 4) * 4 + x - 1] = time;
                    lane_time[x - 1].push_back(time);
                    lane_front[x - 1].push_back((current_time % 4) * 4 + x - 1);
                }
            }
        }

        for (int i = 0; i < 16; i++) {
            if (tiles[i].position.x > 200) continue;
            if (tiles[i].position.x < 0.01 || time - tile_time[i] > 1) {
                tiles[i].position.x = 230;
                if (!lane_time[i % 4].empty() && lane_time[i % 4].front() == tile_time[i]){
                    lane_front[i % 4].pop_front();
                    lane_time[i % 4].pop_front();
                }
                continue;
            }
            tiles[i].position.x = float(200.0 * (1 - time + tile_time[i]) * (1 - time + tile_time[i]));
        }

        for (int i = 0; i < 4; i++) {
            if (input.is_key_down(keys[i])) {
                if (!lane_time[i].empty() && time - lane_time[i].front() >= 0.75) {
                    score += 100 * std::pow(10, time - lane_time[i].front());
                    tiles[lane_front[i].front()].color = glm::vec3(0, 1, 0);
                    lane_time[i].pop_front();
                    lane_front[i].pop_front();
                }
                hints[i].show = true;
            } else hints[i].show = false;
        }
    }

    void render() {
        glDisable(GL_DEPTH_TEST);
        font.render(text_shader, "Score", 5.0f, height - 40.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        font.render(text_shader, std::to_string(score), 180.0f, height - 40.0f, 1.0f, glm::vec3(0.5f, 0.5f, 1.0f));
        glEnable(GL_DEPTH_TEST);
        Line::render(line_shader, lines);
        Tile::render(tile_shader, tiles);
        Hint::render(hint_shader, hints);
    }

    void clear() {
        for (auto &line: lines) line.clear();
        for (auto &tile: tiles) tile.clear();
    }
};

#endif // GAME_H
