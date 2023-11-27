//
// Created by 김준용 on 2023-11-19.
//

#ifndef SOUND_H
#define SOUND_H

#pragma once

#include <iostream>
#include <string>

#include <AL/al.h>
#include <AL/alc.h>

#include "../utils/wav.h"

class Sound {
private:
    unsigned int buffer = 0, source = 0;
    bool stopped = true;

    std::string path;

    inline ALenum to_al_format(int8_t channels, int8_t samples) {
        bool stereo = (channels > 1);

        if (samples == 8) return stereo ? AL_FORMAT_STEREO8 : AL_FORMAT_MONO8;
        if (samples == 16) return stereo ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
        return -1;
    }

    inline void fail(const std::string &message) {
        int error = alGetError();
        if (error != AL_NO_ERROR) {
            std::cerr << "OpenAL " << error << " : " << message << "\n";
        }
    }

public:
    Sound() = default;

    Sound(const std::string &path, bool loops = true) : path(path) {
        alGenSources(1, &source);
        alSourcef(source, AL_PITCH, 1);
        alSourcef(source, AL_GAIN, 1);
        alSource3f(source, AL_POSITION, 0, 0, 0);
        alSource3f(source, AL_VELOCITY, 0, 0, 0);
        alSourcei(source, AL_LOOPING, (loops ? AL_TRUE : AL_FALSE));

        alGenBuffers(1, &buffer);

        int channels, sampleRate, bps, size;
        char *data = Audio::load(path, channels, sampleRate, bps, size);

        alBufferData(buffer, to_al_format(channels, bps), data, size, sampleRate);

        fail("Buffer Data");

        alSourcei(source, AL_BUFFER, buffer);
    }

    ~Sound() {
        alDeleteSources(1, &source);
        alDeleteBuffers(1, &buffer);
    }

    void play() {
        int state = 0;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) {
            stopped = false;
            alSourcei(source, AL_POSITION, 0);
        }

        if (stopped) {
            alSourcePlay(source);
            stopped = false;
        }
    }

    void stop() {
        if (!stopped) {
            alSourceStop(source);
            stopped = true;
        }
    }

    bool playing() {
        int state = -1;
        alGetSourcei(source, AL_SOURCE_STATE, &state);
        if (state == AL_STOPPED) stopped = true;
        return !stopped;
    }
};

#endif // SOUND_H
