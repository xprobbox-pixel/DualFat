cmake_minimum_required(VERSION 3.22)
project(CobaltDualFilter VERSION 1.2.1 LANGUAGES C CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(COBALT_DSP_ONLY "Build dependency-free DSP tests only" OFF)
enable_testing()
add_executable(DspTests Tests/DspTests.cpp)
add_test(NAME DSP COMMAND DspTests)
if(COBALT_DSP_ONLY)
 return()
endif()
if(DEFINED JUCE_PATH)
 add_subdirectory("${JUCE_PATH}" JUCE)
else()
 include(FetchContent)
 FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 29396c22c93392d6738e021b83196283d6e4d850 GIT_SHALLOW FALSE)
 FetchContent_MakeAvailable(JUCE)
endif()
set(formats VST3 Standalone)
if(APPLE)
 list(APPEND formats AU)
endif()
juce_add_plugin(CobaltDualFilter COMPANY_NAME "Cobalt Audio" BUNDLE_ID audio.cobalt.dualfilter PLUGIN_MANUFACTURER_CODE Cblt PLUGIN_CODE Cdfl FORMATS ${formats} PRODUCT_NAME "DUALFAT" IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE COPY_PLUGIN_AFTER_BUILD FALSE
    MICROPHONE_PERMISSION_ENABLED TRUE
    MICROPHONE_PERMISSION_TEXT "DUALFAT uses your selected audio input for real-time filtering.")
juce_add_binary_data(DualfatFonts HEADER_NAME DualfatFonts.h NAMESPACE DualfatFonts
    SOURCES Assets/Fonts/Roboto-Regular.ttf Assets/Fonts/Roboto-Bold.ttf Assets/Branding/DUALFAT-logo.png)
juce_generate_juce_header(CobaltDualFilter)
target_sources(CobaltDualFilter PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_definitions(CobaltDualFilter PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(CobaltDualFilter PRIVATE DualfatFonts juce::juce_audio_utils PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)

# Seal the finished bundles after JUCE's VST3 manifest generation.
if(APPLE)
 foreach(format VST3 AU Standalone)
  add_custom_command(TARGET CobaltDualFilter_${format} POST_BUILD
   COMMAND /usr/bin/codesign --force --sign - --timestamp=none
           "$<TARGET_BUNDLE_DIR:CobaltDualFilter_${format}>"
   VERBATIM)
 endforeach()
endif()
