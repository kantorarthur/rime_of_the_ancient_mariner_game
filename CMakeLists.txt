cmake_minimum_required(VERSION 3.16)
project(RimeOfTheAncientMariner CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/*.cpp
    ${CMAKE_SOURCE_DIR}/external/*.cpp)

add_executable(rime_of_the_ancient_mariner ${SOURCES})
target_include_directories(rime_of_the_ancient_mariner PRIVATE ${CMAKE_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/external)
target_link_libraries(rime_of_the_ancient_mariner OpenGL::GL GLEW::GLEW glfw)

set_target_properties(rime_of_the_ancient_mariner PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
