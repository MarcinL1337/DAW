#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "Menu.h"
#include "Toolbar/MainToolbar.h"

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    void paint(juce::Graphics&) override;
    void resized() override;
    void flexBoxInit();

private:
    Menu topLevelMenu;
    MainToolbar mainToolbar;
    juce::FlexBox mainFlexBox;
    juce::FlexBox mainContentFlexBox;
    juce::FlexBox mainWindowFlexBox;
    const float mainToolbarHeight{45.0};
    const float topLevelMenuHeight{25.0};
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
