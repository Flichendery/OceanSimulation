cmake_minimum_required(VERSION 3.14)
project(Ocean)

set(CMAKE_CXX_STANDARD 17)

set(FTXUI_ENABLE_INSTALL OFF CACHE INTERNAL "")

include(FetchContent)
FetchContent_Declare(
  ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v5.0.0
)

FetchContent_MakeAvailable(ftxui)

add_executable(OceanSim
    Ocean.cpp
)

target_link_libraries(OceanSim PRIVATE ftxui::screen ftxui::dom ftxui::component)
