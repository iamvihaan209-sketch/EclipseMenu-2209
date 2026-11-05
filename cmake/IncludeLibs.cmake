# Automatically download CPM.cmake if it doesn't exist yet
set(CPM_DOWNLOAD_VERSION 0.42.1)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake v${CPM_DOWNLOAD_VERSION}...")
    file(DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
        ${CPM_DOWNLOAD_LOCATION}
    )
endif()

include(${CPM_DOWNLOAD_LOCATION})

CPMAddPackage(
    NAME imgui
    GITHUB_REPOSITORY ocornut/imgui
    GIT_TAG docking
    GIT_SHALLOW TRUE
    DOWNLOAD_ONLY TRUE
)

if(imgui_ADDED AND NOT TARGET imgui)
    add_library(imgui STATIC
        ${imgui_SOURCE_DIR}/imgui.cpp
        ${imgui_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_SOURCE_DIR}/imgui_demo.cpp
    )
    target_include_directories(imgui PUBLIC ${imgui_SOURCE_DIR})
    target_compile_definitions(imgui PUBLIC IMGUI_DEFINE_MATH_OPERATORS)
endif()

if (NOT TARGET imgui)
    message(FATAL_ERROR "Failed to add ImGui dependency!")
endif()

# nlohmann/json: Geode v6 no longer re-exports it, so it is added directly
CPMAddPackage(
    NAME nlohmann_json
    GITHUB_REPOSITORY nlohmann/json
    VERSION 3.11.3
)

# rift: text/expression scripting library used by labels and Discord RPC
CPMAddPackage(
    NAME rift
    GITHUB_REPOSITORY EclipseMenu/rift
    GIT_TAG v2.0.2
)

add_library(third_party INTERFACE)
target_link_libraries(third_party INTERFACE imgui nlohmann_json::nlohmann_json rift)
