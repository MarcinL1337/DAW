#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "Constants.h"
#include "SideMenu/SideMenu.h"
#include "Toolbar/MainToolbar.h"
#include "TopMenu/Menu.h"
#include "TrackPlayer/Timeline.h"
#include "TrackPlayer/TrackPlayer.h"

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    void paint(juce::Graphics&) override;
    void resized() override;
    void flexBoxInit();

private:
    Menu topLevelMenu{};
    MainToolbar mainToolbar{};
    TrackPlayer trackPlayer{};
    SideMenu sideMenu{};
    Timeline timeline{};

    juce::FlexBox topLevelFlexBox{};
    juce::FlexBox mainContentFlexBox{};
    juce::FlexBox trackPlayerFlexBox{};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
