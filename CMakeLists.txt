cmake_minimum_required(VERSION 3.20)
project(cre VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(cre_WARNINGS_AS_ERRORS "Treat warnings as errors" OFF)
option(cre_ENABLE_SANITIZERS "Enable address and undefined sanitizers" OFF)
option(cre_ENABLE_GUI "Build SDL2 + Dear ImGui user interface" ON)

find_program(CLANG_FORMAT_EXE NAMES clang-format)
find_program(CLANG_TIDY_EXE NAMES clang-tidy)

function(configure_project_target target_name)
    if(MSVC)
        target_compile_options(${target_name} PRIVATE /W4)
        return()
    endif()

    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Wpedantic)
    if(cre_WARNINGS_AS_ERRORS)
        target_compile_options(${target_name} PRIVATE -Werror)
    endif()
    if(cre_ENABLE_SANITIZERS)
        target_compile_options(${target_name} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
        target_link_options(${target_name} PRIVATE -fsanitize=address,undefined)
    endif()
endfunction()

if(cre_ENABLE_GUI)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)

    set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui)

    add_library(imgui STATIC
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
        ${IMGUI_DIR}/backends/imgui_impl_sdl2.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
    )

    target_include_directories(imgui PUBLIC
        ${IMGUI_DIR}
        ${IMGUI_DIR}/backends
    )

    target_link_libraries(imgui PUBLIC SDL2::SDL2 OpenGL::GL)
endif()

add_library(cre_core
    src/App.cpp
)
target_include_directories(cre_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
configure_project_target(cre_core)

if(cre_ENABLE_GUI)
    add_library(cre_gui STATIC
        src/GuiApp.cpp
    )
    target_include_directories(cre_gui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(cre_gui PUBLIC cre_core imgui)
    configure_project_target(cre_gui)
endif()

add_executable(cre
    src/main.cpp
)
target_link_libraries(cre PRIVATE cre_core)
if(cre_ENABLE_GUI)
    target_link_libraries(cre PRIVATE cre_gui)
endif()
target_include_directories(cre PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
configure_project_target(cre)

add_executable(cre_tests
    tests/SmokeTests.cpp
)
target_link_libraries(cre_tests PRIVATE cre_core)
target_include_directories(cre_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
configure_project_target(cre_tests)

if(CLANG_FORMAT_EXE)
    file(GLOB_RECURSE FORMAT_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp
    )
    add_custom_target(format-check
        COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${FORMAT_SOURCES}
        COMMENT "Checking code format"
        VERBATIM
    )
endif()

include(CTest)
if(BUILD_TESTING)
    add_test(NAME cre_tests COMMAND cre_tests)
endif()
