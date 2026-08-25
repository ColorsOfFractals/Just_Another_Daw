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

        Source/App/JADApplication.cpp
        Source/App/JADApplication.h

        Source/Core/JADContext.cpp
        Source/Core/JADContext.h

        Source/Audio/AudioSystem.cpp
        Source/Audio/AudioSystem.h

        Source/Model/SessionState.cpp

        Source/Model/TransportState.cpp


        Source/Model/TrackModel.cpp

        Source/Model/TrackModel.h
        Source/Model/TransportState.h
        Source/Model/SessionState.h

        Source/UI/MainComponent.cpp

        Source/UI/SynthVoicePanel.cpp

        Source/UI/SynthVoicePanel.h
        Source/UI/MainComponent.h
)

target_include_directories(
    JAD
    PRIVATE
        Source
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

target_link_libraries(JAD
    PRIVATE
        juce::juce_gui_extra
        juce::juce_audio_devices
        juce::juce_audio_basics
)










