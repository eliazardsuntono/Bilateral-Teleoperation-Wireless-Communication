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

find_package(nlohmann_json REQUIRED)

add_executable(client client.cpp)
target_link_libraries(client PRIVATE moteus::cpp)
target_link_libraries(client PRIVATE nlohmann_json::nlohmann_json)
