cmake_minimum_required(VERSION 3.22)
project(ZeroIsInfectious VERSION 1.3 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ZERO_BUILD_APP "Build the JUCE desktop application" ON)
set(JUCE_DIR "" CACHE PATH "Optional path to a JUCE 8 source checkout")

if(ZERO_BUILD_APP)
    if(JUCE_DIR)
        if(NOT EXISTS "${JUCE_DIR}/CMakeLists.txt")
            message(FATAL_ERROR "JUCE_DIR must point to the JUCE source root")
        endif()
        add_subdirectory("${JUCE_DIR}" "${CMAKE_BINARY_DIR}/juce")
    else()
        include(FetchContent)
        # JUCE 8.0.12, pinned to the verified upstream commit.
        FetchContent_Declare(JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 29396c22c93392d6738e021b83196283d6e4d850
        )
        FetchContent_MakeAvailable(JUCE)
    endif()

    juce_add_gui_app(ZeroIsInfectious PRODUCT_NAME "Zero Is Infectious")
    target_sources(ZeroIsInfectious PRIVATE Main.cpp Engine.h)
    target_link_libraries(ZeroIsInfectious PRIVATE juce::juce_gui_basics)
    target_compile_definitions(ZeroIsInfectious PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
    )
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(engine_tests Tests.cpp)
    add_test(NAME engine COMMAND engine_tests)
endif()
