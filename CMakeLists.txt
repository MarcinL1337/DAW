cmake_minimum_required(VERSION 3.24)

# Change to your project name
project(GuiAppTemplate VERSION 0.0.1)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_XCODE_GENERATE_SCHEME OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# We're going to use CPM as our package manager to bring in JUCE
# Check to see if we have CPM installed already.  Bring it in if we don't.
set(CPM_DOWNLOAD_VERSION 0.40.5)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if (NOT EXISTS ${CPM_DOWNLOAD_LOCATION})
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif ()

include(${CPM_DOWNLOAD_LOCATION})

# Bring in JUCE locally
CPMAddPackage(
        NAME juce
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG origin/master
)

CPMAddPackage(
        NAME nlohmann_json
        VERSION 3.11.3
        GITHUB_REPOSITORY nlohmann/json
        OPTIONS
        "JSON_BuildTests OFF"
)

# Make sure you include any new source files here
set(SourceFiles
        Source/Main.cpp
        Source/MainComponent.h
        Source/MainComponent.cpp
        Source/MainWindow.h
        Source/MainWindow.cpp

        Source/TopMenu/Menu.h
        Source/TopMenu/Menu.cpp

        Source/Toolbar/ToolbarFactory.h
        Source/Toolbar/ToolbarFactory.cpp
        Source/Toolbar/MainToolbar.h
        Source/Toolbar/MainToolbar.cpp

        Source/TrackPlayer/ClipSplitBar.h
        Source/TrackPlayer/ClipSplitBar.cpp
        Source/TrackPlayer/TrackGuiManager.h
        Source/TrackPlayer/TrackGuiManager.cpp
        Source/TrackPlayer/Timeline.h
        Source/TrackPlayer/Timeline.cpp
        Source/TrackPlayer/trackGui.h
        Source/TrackPlayer/trackGui.cpp
        Source/TrackPlayer/TrackPlayerSideMenu.h
        Source/TrackPlayer/TrackPlayerSideMenu.cpp
        Source/TrackPlayer/TimeBar.h
        Source/TrackPlayer/TimeBar.cpp
        Source/TrackPlayer/TrackGuiComponent.h
        Source/TrackPlayer/TrackGuiComponent.cpp
        Source/TrackPlayer/Waveform.h
        Source/TrackPlayer/Waveform.cpp

        Source/SideMenu/SideMenu.h
        Source/SideMenu/SideMenu.cpp
        Source/SideMenu/ZoomSlider.h
        Source/SideMenu/ZoomSlider.cpp

        Source/Audio/MainAudio.h
        Source/Audio/MainAudio.cpp
        Source/Audio/AudioClip.h
        Source/Audio/AudioClip.cpp
        Source/Audio/AudioTrack.h
        Source/Audio/AudioTrack.cpp

        Source/TrackManager.h
        Source/TrackManager.cpp
)

juce_add_gui_app(${PROJECT_NAME}
        # VERSION ...                       # Set this if the app version is different to the project version
        # ICON_BIG ...                      # ICON_* arguments specify a path to an image file to use as an icon
        # ICON_SMALL ...
        # DOCUMENT_EXTENSIONS ...           # Specify file extensions that should be associated with this app
        # COMPANY_NAME ...                  # Specify the name of the app's author
        PRODUCT_NAME "MyDAW")               # The name of the final executable, which can differ from the target name

# How we want our SourceFiles to appear in our IDE
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SourceFiles})

# Make the SourceFiles buildable
target_sources(${PROJECT_NAME} PRIVATE ${SourceFiles})

# These are some toggleable options from the JUCE CMake API
target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

juce_add_binary_data(BinaryData SOURCES
        Assets/ToolbarImages/nextButton.png
        Assets/ToolbarImages/pauseButton.png
        Assets/ToolbarImages/playButton.png
        Assets/ToolbarImages/playPauseButton.png
        Assets/ToolbarImages/playPauseButton2.png
        Assets/ToolbarImages/previousButton.png
        Assets/ToolbarImages/recordButton.png
        Assets/ToolbarImages/replayButton.png
        Assets/ToolbarImages/stopButton.png
        Assets/ToolbarImages/followModeButton.png
        Assets/ToolbarImages/split.png
)

# JUCE libraries to bring into our project
target_link_libraries(${PROJECT_NAME}
        PUBLIC
        BinaryData
        juce::juce_analytics
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_core
        juce::juce_data_structures
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        nlohmann_json::nlohmann_json
)
