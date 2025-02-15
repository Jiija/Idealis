cmake_minimum_required(VERSION 3.2)

project(Idealis)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_INSTALL_PREFIX ${PROJECT_SOURCE_DIR})

set(CMAKE_C_COMPILER gcc CACHE STRING "C compiler")
set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler")

include_directories(${PROJECT_SOURCE_DIR}/App/Database)

add_subdirectory(App)
add_subdirectory(Database)
add_subdirectory(external/llama)
