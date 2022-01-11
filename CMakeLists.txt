cmake_minimum_required(VERSION 3.10.2)
set(CMAKE_CXX_STANDARD 17)
project(cg-trabalho-3)

# include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

# find_package(GLEW REQUIRED)
# find_package(glfw REQUIRED)

# add_executable(exe
#     src/main.cpp
#     src/shader.cpp
# )

# target_include_directories(exe
#     PUBLIC
#         ${CMAKE_CURRENT_SOURCE_DIR}/src
#     PRIVATE
#         ${GLEW_INCLUDE_DIRS}
#         ${glfw_INCLUDE_DIRS}
# )

# target_link_libraries(exe
#     GLEW::GLEW
#     glfw::glfw
# )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(exe
    src/camera.cpp
    src/csv-model.cpp
    src/main.cpp
    src/settings.cpp
    src/shader.cpp
    src/texture.cpp
)

target_include_directories(exe
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(exe ${CONAN_LIBS})

file(
    COPY
        ${CMAKE_CURRENT_SOURCE_DIR}/src/settings.json
    DESTINATION
        ${CMAKE_CURRENT_BINARY_DIR}/bin
)
