//
// Created by 김준용 on 2023-11-20.
//

#ifndef FFT_H
#define FFT_H

#pragma once

#include <algorithm>
#include <cassert>
#include <complex>
#include <vector>

#include "wav.h"

// DFT & IDFT
void fft(std::vector<std::complex<double>> &a, bool inv = false) {
    int n = (int) a.size();
    assert(n == (1 << __builtin_ctz(n)));

    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        while (!((j ^= bit) & bit)) bit >>= 1;
        if (i < j) swap(a[i], a[j]);
    }

    for (int i = 1; i < n; i <<= 1) {
        double x = inv ? M_PI / i : -M_PI / i;
        std::complex<double> w(cos(x), sin(x));
        for (int j = 0; j < n; j += i << 1) {
            std::complex<double> p(1, 0);
            for (int k = 0; k < i; k++) {
                std::complex<double> tmp = a[i + j + k] * p;
                a[i + j + k] = a[j + k] - tmp;
                a[j + k] += tmp;
                p *= w;
            }
        }
    }
    if (inv)
        for (int i = 0; i < n; i++) a[i] /= n;
}

// Todo. Fix
std::vector<int> fft(Audio &audio) {
    std::vector<int> peak;

    std::vector<std::complex<double>> a;
    int n = 1;
    while (n < audio.size()) n <<= 1;
    a.resize(n);
    for (int i = 0; i < audio.size(); i++) a[i] = {(double) audio[i].first, 0};
    fft(a);

    std::ofstream out("test.txt");
    for (int j = 0; j < n / 2; j++) {
        double magnitude = std::sqrt(a[j].real() * a[j].real() + a[j].imag() * a[j].imag());
        int freq = j * audio.sample_rate() / n;
        out << freq << " " << magnitude << "\n";
    }

    return peak;
}

// Todo. Maybe better peak detection
std::vector<std::vector<int>> fft(Audio &audio, int bucket, int min_dist = 10) {
    std::vector<std::vector<int>> peaks;

    // assert(bucket == (1 << __builtin_ctz(bucket)));

    int n = 1;
    while (n < bucket) n <<= 1;

    for (int i = 0; i < (audio.size() + bucket - 1) / bucket; i++) {
        std::vector<std::complex<double>> a(n);
        for (int j = i * bucket; j < std::min((i + 1) * bucket, (int) audio.size()); j++)
            a[j - i * bucket] = {(double) audio[j].first, 0};
        fft(a);

        std::vector<std::pair<int, double>> frequencies;

        double avg = 0;

        for (int j = 0; j < bucket / 2; j++) {
            double magnitude = std::sqrt(a[j].real() * a[j].real() + a[j].imag() * a[j].imag());
            int freq = int(int64_t(j) * audio.sample_rate() / n);
            avg += magnitude / bucket * 2;
            if (!frequencies.empty() && freq == frequencies.back().first)
                frequencies.back().second = std::max(frequencies.back().second, magnitude);
            else frequencies.emplace_back(freq, magnitude);
        }

        std::vector<std::pair<double, int>> possible;
        for (int descent = 0, j = 1; j < frequencies.size(); j++) {
            if (frequencies[j].second > frequencies[j - 1].second) {
                descent = 0;
            } else {
                if (descent == 0) {
                    possible.emplace_back(std::log10(frequencies[j].second), frequencies[j].first);
                }
                descent = 1;
            }
        }

        std::sort(possible.begin(), possible.end(),[&](const std::pair<double, int> &a, const std::pair<double, int> &b) {
                      return a.first > b.first;
        });

        std::vector<int> peak;
        for (auto &[x, y]: possible) {
            if (x > 1.7 + std::log10(avg) && (peak.empty() || std::abs(peak.back() - y) > min_dist))
                peak.push_back(y);
        }

        peaks.push_back(peak);
    }

    return peaks;
}

#endif // FFT_H
