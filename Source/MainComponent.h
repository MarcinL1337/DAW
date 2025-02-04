#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
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

    float getMainToolbarHeight() const { return mainToolbarHeight; }
    float getTopLevelMenuHeight() const { return topLevelMenuHeight; }

private:
    Menu topLevelMenu;
    MainToolbar mainToolbar;
    TrackPlayer trackPlayer;
    Timeline timeline;

    juce::FlexBox topLevelFlexBox;
    juce::FlexBox mainContentFlexBox;

    const float mainToolbarHeight{45.0};
    const float topLevelMenuHeight{25.0};
    const float sideMenuWidth{100.0};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
