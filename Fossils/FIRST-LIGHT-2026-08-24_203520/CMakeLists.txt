cmake_minimum_required(VERSION 3.22)

project(
    JAD
    VERSION 0.0.1
    LANGUAGES C CXX
)

add_subdirectory(External/JUCE)

juce_add_gui_app(
    JAD
    PRODUCT_NAME "JAD"
)

target_sources(
    JAD
    PRIVATE
        Source/Main.cpp
)

target_compile_features(
    JAD
    PRIVATE
        cxx_std_17
)

target_compile_definitions(
    JAD
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
)

target_link_libraries(
    JAD
    PRIVATE
        juce::juce_gui_extra
)

