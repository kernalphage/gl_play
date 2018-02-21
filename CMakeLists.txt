cmake_minimum_required(VERSION 3.4)
project(gl_play)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-g -Wl,--no-as-needed -ldl -lGL" )
set(SOURCE_FILES main.cpp Material.h ref/glad/src/glad.c)


add_executable(gl_play ${SOURCE_FILES})
include_directories("ref/glad/include")

if(WIN32)
    set(GLFW_PATH $ENV{GLFW_SDK})
    include_directories("${GLFW_PATH}/include")
    include_directories($ENV{RANGES_INCLUDE})

    target_link_libraries(gl_play "${GLFW_PATH}/lib-vc2015/glfw3.lib")
ELSE()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(GLFW REQUIRED glfw3)
    include_directories(${GLFW_INCLUDE_DIRS})
    target_link_libraries(gl_play glfw ${GLFW_LIBRARIES})
    target_link_libraries(gl_play ${LIBRARIES})
ENDIF()

# yet another way to get shaders

set(SHADERS basic.vert basic.frag)
foreach(shader ${SHADERS})
	configure_file( ${shader} ${shader} COPYONLY)
endforeach()