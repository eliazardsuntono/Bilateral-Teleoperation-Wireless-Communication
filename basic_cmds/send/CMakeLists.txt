cmake_minimum_required(VERSION 3.20)

project(MoteusBilateralTeleoperation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  moteus
  GIT_REPOSITORY https://github.com/mjbots/moteus.git 
  GIT_TAG origin/main
)

FetchContent_MakeAvailable(moteus)

add_executable(send send.cpp)
target_link_libraries(send PRIVATE moteus::cpp)
