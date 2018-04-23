cmake_minimum_required(VERSION 3.4)
project(gl_play)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -g -O2" )

set(SOURCE_FILES
    main.cpp
    Material.hpp
    ProcessingSVG.cpp
    ProcessingGL.cpp
    Processing.hpp
    Random.cpp
    Random.hpp
    Triangulate.cpp
    Triangulate.hpp
    Window.cpp
    Window.hpp
    Definitions.hpp
        Blob.cpp Blob.hpp)
set(REF_SOURCE_FILES
    imgui_impl/glfw_gl3.cpp
    imgui_impl/glfw_gl3.h
    imgui_impl/imgui.cpp
    imgui_impl/imgui_demo.cpp
    imgui_impl/imgui_draw.cpp
    imgui_impl/imguifilesystem.cpp
    ref/FastNoise/FastNoise.cpp
	ref/FastNoise/FastNoise.h
	ref/glad/src/glad.c
    ref/image.capnp.c++
	)

add_executable(gl_play ${SOURCE_FILES} ${REF_SOURCE_FILES})
include_directories("ref/glad/include")
include_directories("ref")
include_directories("imgui_impl")
include_directories("ref/stb")
set(LIBRARIES "dl")


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/git/capnproto-c++-0.6.1/cmake/")
find_package(CapnProto REQUIRED)
target_link_libraries(gl_play capnp)

if(WIN32)
    set(GLFW_PATH $ENV{GLFW_SDK})
    include_directories("${GLFW_PATH}/include")
    include_directories($ENV{RANGES_INCLUDE})
    include_directories("C:/Users/matt/git/glm")

    target_link_libraries(gl_play "${GLFW_PATH}/lib-vc2015/glfw3.lib")
    # TODO: Link to png

ELSE()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_libraries(gl_play glfw ${GLFW_LIBRARIES})
    target_link_libraries(gl_play ${LIBRARIES})
    find_library(LIBPNG png)
    target_link_libraries(gl_play ${LIBPNG})

ENDIF()

# yet another way to get shaders

set(SHADERS 
    shaders/basic.vert
	shaders/basic.frag
    svgbuilder.py
	)
foreach(shader ${SHADERS})
	configure_file( "${shader}" ${shader} COPYONLY)
endforeach()
