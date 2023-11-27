//
// Created by 김준용 on 2023-11-10.
//

#ifndef WAV_H
#define WAV_H

#pragma once

#include <iostream>
#include <cstdint>
#include <cassert>
#include <fstream>
#include <utility>
#include <vector>

class Audio {
protected:
#pragma pack(push, 1) // 44 Byte 이므로 안 붙여도 상관은 없음
    struct Header {
        uint8_t chunk[4] = {'R', 'I', 'F', 'F'};
        uint32_t chunk_size{};
        uint8_t format[4] = {'W', 'A', 'V', 'E'};
        uint8_t Subchunk1ID[4] = {'f', 'm', 't', ' '};
        uint32_t Subchunk1Size = 16;
        uint16_t audio_format = 1;
        uint16_t channels = 2;
        uint32_t sample_rate = 44100;
        uint32_t byte_rate = 176400;
        uint16_t block_align = 4;
        uint16_t bits_per_sample = 16;
        uint8_t Subchunk2ID[4] = {'d', 'a', 't', 'a'};
        uint32_t Subchunk2Size{};
    } header;
#pragma pack(pop)

private:
    std::vector<std::pair<int16_t, int16_t>> data;

    std::vector<uint8_t> sub_header;

public:
    Audio() = default;

    explicit Audio(const std::string &path) {
        std::ifstream in(path, std::iostream::binary);
        if (!in) {
            throw std::runtime_error("Can't open the file " + path);
        }
        in.read((char *) &header, 36);

        sub_header.resize(4);
        for (int i = 0; i < 4; i++) in.read((char *) &sub_header[i], 1);
        while (!(sub_header[sub_header.size() - 1] == 'a' && sub_header[sub_header.size() - 2] == 't' && sub_header[sub_header.size() - 3] == 'a' && sub_header[sub_header.size() - 4] == 'd')) {
            sub_header.push_back(0);
            in.read((char *) &sub_header.back(), 1);
        }
        in.read((char *) &header.Subchunk2Size, 4);

        uint32_t size = header.Subchunk2Size / (header.channels * (header.bits_per_sample / 8));

        // https://stackoverflow.com/questions/63929283/what-is-a-list-chunk-in-a-riff-wav-header
        // Todo. fix LIST header

        data.resize(size);
        for (int i = 0; i < size; i++) {
            in.read((char *) &data[i].first, sizeof(int16_t));
            in.read((char *) &data[i].second, sizeof(int16_t));
        }
        in.close();
    }

    // ms
    explicit Audio(uint32_t length) {
        auto size = uint32_t((int64_t) header.sample_rate * length / 1000);

        header.Subchunk2Size = size * (header.channels * (header.bits_per_sample / 8));
        header.chunk_size = 36 + header.Subchunk2Size;

        data.resize(size, {0, 0});
    }

    void write(const std::string &path) {
        std::ofstream out(path, std::iostream::binary);
        if (!out) {
            throw std::runtime_error("Can't open the file " + path);
        }

        // Check header
        uint32_t size = data.size();

        header.Subchunk2Size = size * (header.channels * (header.bits_per_sample / 8));
        header.chunk_size = 36 + sub_header.size() + header.Subchunk2Size;

        out.write((char *) &header, (sub_header.empty() ? 40 : 36));
        if (!sub_header.empty()) out.write((char *) sub_header.data(), (int) sub_header.size());
        out.write((char *) &header.Subchunk2Size, 4);

        for (auto &[a, b]: data) {
            out.write((char *) &a, sizeof(int16_t));
            out.write((char *) &b, sizeof(int16_t));
        }

        out.close();
    }

    std::pair<int16_t, int16_t> &operator[](int x) {
        if (x >= data.size()) { // Index out
            throw std::runtime_error("Expected value between 0 and " + std::to_string(data.size() - 1)
                                     + ", but found " + std::to_string(x) + "!");
        }
        return data[x];
    }

    /**
     * @return Size of data file
     */
    std::size_t size() {
        return data.size();
    }

    void push_back(const std::pair<int16_t, int16_t> &a) {
        data.push_back(a);
    }

    /**
     * @return Length of wav file in ms
     */
    uint32_t length() {
        return data.size() * 1000 / header.sample_rate;
    }

    [[nodiscard]] uint32_t sample_rate() const {
        return header.sample_rate;
    }

    static char *load(const std::string &path, int &channel, int &samplerate, int &bps, int &size) {
        std::ifstream in(path, std::ios::binary);
        Header header;

        in.read((char *) &header, 36);

        std::vector<char> sub_header(4);
        for (int i = 0; i < 4; i++) in.read((char *) &sub_header[i], 1);
        while (!(sub_header[sub_header.size() - 1] == 'a' && sub_header[sub_header.size() - 2] == 't' && sub_header[sub_header.size() - 3] == 'a' && sub_header[sub_header.size() - 4] == 'd')) {
            sub_header.push_back(0);
            in.read((char *) &sub_header.back(), 1);
        }
        in.read((char *) &header.Subchunk2Size, 4);

        channel = header.channels;
        samplerate = header.sample_rate;
        bps = header.bits_per_sample;
        size = header.Subchunk2Size;

        char *data = new char[size];

        in.read(data, size);

        return data;
    }
};

#endif // WAV_H
