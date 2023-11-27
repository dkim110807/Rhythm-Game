cmake_minimum_required(VERSION 3.26)
project(Sound)

set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES main.cpp opengl/glad/src/glad.c input/input.cpp)
set(HEADER_FILES utils/wav.h utils/random.h utils/stb_image.h graphics/shader.h input/input.h graphics/vertex.h audio/sound.h graphics/gui/font/font.h utils/fft.h graphics/line.h graphics/tile.h game/game.h graphics/hint.h)

include_directories(include)

include_directories(opengl/glad/include)
include_directories(opengl/glm/include)
include_directories(opengl/openal/include)

find_package(Freetype REQUIRED)

file(COPY assets/resources DESTINATION ${dir}/build)
file(COPY assets/shaders DESTINATION ${dir}/build)

add_subdirectory(opengl/glfw/glfw-3.3.8)

add_executable(Sound ${SOURCE_FILES} ${HEADER_FILES})

link_libraries(${CMAKE_SOURCE_DIR}/opengl/openal/lib)

target_link_libraries(Sound ${FREETYPE_LIBRARIES})
target_include_directories(Sound PRIVATE ${FREETYPE_INCLUDE_DIRS})
target_link_libraries(Sound ${CMAKE_SOURCE_DIR}/opengl/openal/lib/OpenAL32.lib)
target_include_directories(Sound PRIVATE ${OPENAL_INCLUDE_DIR})
target_link_libraries(Sound glfw opengl32)