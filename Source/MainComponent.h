#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "Audio/MainAudio.h"
#include "Constants.h"
#include "ProjectFilesManager.h"
#include "SideMenu/SideMenu.h"
#include "Toolbar/MainToolbar.h"
#include "TopMenu/Menu.h"
#include "TrackManager.h"
#include "TrackPlayer/Timeline.h"
#include "TrackPlayer/TrackGuiManager.h"

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    void paint(juce::Graphics&) override;
    void resized() override;
    void flexBoxInit();

private:
    juce::Identifier treeType{"mainComponentValueTree"};
    juce::ValueTree tree{treeType};

    MainAudio mainAudio;
    Menu topLevelMenu;
    MainToolbar mainToolbar;
    TrackGuiManager trackPlayer;
    SideMenu sideMenu;
    TrackManager trackManager;
    ProjectFilesManager projectFilesManager;

    juce::FlexBox topLevelFlexBox{};
    juce::FlexBox mainContentFlexBox{};
    juce::FlexBox trackPlayerFlexBox{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

    friend class MainWindow;
};
