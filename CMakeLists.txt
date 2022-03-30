cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_WIN32_EXECUTABLE true)

project(FileSearchTool)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS src/*.h)
file(GLOB IMGUI_SOURCES CONFIGURE_DEPENDS libs/imgui/*.cpp libs/imgui/backends/imgui_impl_glfw.cpp libs/imgui/backends/imgui_impl_opengl3.cpp libs/TextEditor/TextEditor.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${IMGUI_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src libs libs/imgui libs/imgui/backends)
target_include_directories(${PROJECT_NAME} PUBLIC libs/glfw/include libs/glew/include)
target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)

add_subdirectory(libs/glew/build/cmake)
add_subdirectory(libs/glfw)

target_link_libraries(${PROJECT_NAME} glfw)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} opengl32 glew_s imm32)
else()
    target_link_libraries(${PROJECT_NAME} GL glew pthread stdc++fs)
endif()
