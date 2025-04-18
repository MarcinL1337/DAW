#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "Audio/MainAudio.h"
#include "Constants.h"
#include "SideMenu/SideMenu.h"
#include "Toolbar/MainToolbar.h"
#include "TopMenu/Menu.h"
#include "TrackManager.h"
#include "TrackPlayer/Timeline.h"
#include "TrackPlayer/TrackPlayer.h"

class MainComponent final : public juce::Component,
                            public juce::KeyListener
{
public:
    MainComponent();
    void paint(juce::Graphics&) override;
    void resized() override;
    void flexBoxInit();
    void addTestTracks();

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

private:
    juce::Identifier treeType{"mainComponentValueTree"};
    juce::ValueTree tree{treeType};

    MainAudio mainAudio{};
    Menu topLevelMenu{};
    MainToolbar mainToolbar;
    TrackPlayer trackPlayer;
    TrackManager trackManager;
    SideMenu sideMenu{};

    juce::FlexBox topLevelFlexBox{};
    juce::FlexBox mainContentFlexBox{};
    juce::FlexBox trackPlayerFlexBox{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
