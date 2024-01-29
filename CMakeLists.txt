cmake_minimum_required(VERSION 3.0.0)
project(model VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

add_executable(model project.cpp shader/shader.cpp model/objload.cpp stb/stb_image.h glad.c glad.h )



target_link_libraries(model glfw3)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
