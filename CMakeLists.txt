cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(game)

# Clang deez nuts
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include debug information
set(CMAKE_BUILD_TYPE Debug)

IF (WIN32)
    # Windows libraries:
    find_package(OpenGL REQUIRED)
    set(LIBS opengl32)

    file(GLOB_RECURSE SOURCE_FILES 
        ${CMAKE_SOURCE_DIR}/win32.cpp
    )
ELSE()
    # Linux libraries:
    set(LIBS X11)

    file(GLOB_RECURSE SOURCE_FILES 
        # ${CMAKE_SOURCE_DIR}/code/*.cpp
    )
ENDIF()

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} 
    PUBLIC ./code
)

target_link_libraries(${PROJECT_NAME} ${LIBS})
