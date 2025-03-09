cmake_minimum_required(VERSION 3.8)
project(MC_APP_BOILERPLATE VERSION 0.1.0 LANGUAGES C CXX)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Platform 
option(PLATFORM_BUILD_DESKTOP "Build for platform desktop" ON)

if(PLATFORM_BUILD_DESKTOP)
    include(platforms/desktop/CMakeLists.txt)
endif()
