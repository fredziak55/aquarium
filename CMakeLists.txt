cmake_minimum_required(VERSION 3.10)
project(AquariumProject)

add_library(glad src/glad.c)
target_include_directories(glad PUBLIC include)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)

# Include directories
include_directories(
    include
    ${OPENGL_INCLUDE_DIRS}
    ${GLFW3_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIRS}
    ${ASSIMP_INCLUDE_DIRS}
    include_directories(${CMAKE_SOURCE_DIR}/third_party)
)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Executable
add_executable(AquariumProject ${SOURCES})

# Link libraries
target_link_libraries(AquariumProject 
    glad
    ${OPENGL_LIBRARIES} 
    glfw
    ${ASSIMP_LIBRARIES}
)

add_custom_command(TARGET AquariumProject POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:AquariumProject>/resources
)