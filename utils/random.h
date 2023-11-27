/**
* Copyright (C) 2015 by Kyung-jin Kim
* e-mail		: devmachine@naver.com
*
*
* Description	: Random Number Generator Class
* Created		: Jun-19,2015
* Last Updated	: Mar-29,2018
* Version		: Random v1.0
*/

#pragma once

#include <random>

template<typename Type>
class Random {
public:
    static_assert(std::is_arithmetic<Type>::value, "Template argument data must be a arithmetic type");

    using IntType = typename std::conditional<(std::is_integral<Type>::value && sizeof(Type) < 2),
            short,
            Type>::type;

    using Distribution = typename std::conditional<std::is_floating_point<Type>::value,
            std::uniform_real_distribution<Type>,
            std::uniform_int_distribution<IntType>>::type;

    using Engine = typename std::conditional<(std::is_integral<Type>::value && sizeof(Type) > 4),
            std::mt19937_64,
            std::mt19937>::type;

    explicit Random(
            Type min = (std::numeric_limits<Type>::min)(),
            Type max = (std::numeric_limits<Type>::max)(),
            typename Engine::result_type seed = std::random_device()()
    )
            : _engine(seed), _distribution((std::min)(min, max), (std::max)(min, max)) {
    }

    Type operator()() {
        return static_cast<Type>(_distribution(_engine));
    }

private:
    Engine _engine;
    Distribution _distribution;
};