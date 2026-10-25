cmake_minimum_required(VERSION 3.22)
project(GaelRackFx VERSION 1.0.0 LANGUAGES CXX)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(GaelRackFx
    COMPANY_NAME "GaelRackFx"
    BUNDLE_ID "com.gaelrackfx.plugin"
    PLUGIN_MANUFACTURER_CODE Gael
    PLUGIN_CODE GrFx
    PRODUCT_NAME "GaelRack Fx"
    DESCRIPTION "Malagasy vocal effects processor"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3 Standalone
    VST3_CATEGORIES Fx Dynamics EQ
)

juce_generate_juce_header(GaelRackFx)

target_sources(GaelRackFx PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_features(GaelRackFx PRIVATE cxx_std_17)
target_compile_definitions(GaelRackFx PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)
target_link_libraries(GaelRackFx PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags
           juce::juce_recommended_lto_flags
           juce::juce_recommended_warning_flags
)
