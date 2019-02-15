cmake_minimum_required(VERSION 3.4)
project(gl_play)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -g -O3" )

set(PROC_SOURCE_FILES
    proc/Blob.cpp
    proc/Blob.hpp
    proc/Flame.cpp
    proc/Flame.hpp
    proc/flowmap.cpp
    proc/flowmap.hpp
    proc/Cellular.cpp proc/Cellular.hpp
    proc/proc_map.cpp 
    proc/proc_map.hpp
    proc/Streamline.cpp
    proc/Streamline.hpp
    proc/Triangulate.cpp
    proc/Triangulate.hpp
        proc/fuzzy_net.hpp proc/Superformula.hpp)
set(ENGINE_SOURCE_FILES
    Definitions.hpp
    hexlib.hpp
    main.cpp
    Material.cpp
    Material.hpp
    Plotter.cpp 
    Plotter.hpp
    Processing.hpp
    ProcessingGL.cpp
    ProcessingSVG.cpp
    Random.cpp
    Random.hpp
    RandomCache.cpp
    RandomCache.hpp 
    RenderTarget.cpp
    RenderTarget.hpp
    sketches.hpp
    Texture.hpp
    Window.cpp
    Window.hpp
    )
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
        ref/fmt/src/format.cc
        proc/cached_flowmap.cpp proc/cached_flowmap.hpp)

include_directories("ref/fmt/include")
include_directories(".")

include_directories("ref/DonerSerializer/DonerSerializer/include")
include_directories("ref/DonerSerializer/DonerSerializer/thirdparty/DonerReflection/DonerReflection/include")
add_executable(gl_play ${PROC_SOURCE_FILES} ${ENGINE_SOURCE_FILES} ${REF_SOURCE_FILES})
include_directories("ref/glad/include")
include_directories("ref")
include_directories("imgui_impl")
include_directories("ref/stb")
set(LIBRARIES "dl")

# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "~/git/capnproto-c++-0.6.1/cmake/")
# find_package(CapnProto REQUIRED)
# target_link_libraries(gl_play capnp)

if(WIN32)
    set(GLFW_PATH $ENV{GLFW_SDK})
    include_directories("${GLFW_PATH}/include")
    # include_directories($ENV{RANGES_INCLUDE})
    include_directories("$ENV{GLM_INCLUDE}")

    target_link_libraries(gl_play "${GLFW_PATH}/lib-vc2015/glfw3.lib")
    # TODO: Link to png
ELSEIF(APPLE)
    include_directories("../glm")
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    link_directories("/opt/local/lib")
  #  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -framework Cocoa -framework OpenGL -framework IOKit")

  find_library(GLFW_LIBRARY glfw HINTS "/opt/local/lib")
  target_link_libraries(gl_play  ${GLFW_LIBRARY})
    target_link_libraries(gl_play ${LIBRARIES})

    find_library(LIBPNG png)
    target_link_libraries(gl_play ${LIBPNG})
ELSE()
    find_package(glfw3)
    target_link_libraries(gl_play glfw)
    get_cmake_property(_variableNames VARIABLES)
    foreach (_variableName ${_variableNames})
        message(STATUS "${_variableName}=${${_variableName}}")
    endforeach()


    find_library(LIBPNG png)
    target_link_libraries(gl_play ${LIBPNG})

ENDIF()



# yet another way to get shaders

set(SHADERS
        shaders/basic.vert
        shaders/basic.frag
        shaders/two_tri.vert
        shaders/two_tri.frag
        shaders/flame.geom
        tonemap.png
	)
foreach(shader ${SHADERS})
	configure_file( "${shader}" ${shader} COPYONLY)
endforeach()
