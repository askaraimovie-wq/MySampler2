cmake_minimum_required(VERSION 3.22)
project(MySampler VERSION 0.1.0)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 7.0.12
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(MySampler
    COMPANY_NAME "MyStudio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Myst
    PLUGIN_CODE Msmp
    FORMATS VST3 Standalone
    PRODUCT_NAME "My Sampler"
    COPY_PLUGIN_AFTER_BUILD FALSE)

target_sources(MySampler PRIVATE
    PluginProcessor.cpp
    PluginEditor.cpp)

target_compile_definitions(MySampler PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

target_link_libraries(MySampler
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_formats
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)
