#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "Menu.h"
#include "Toolbar/MainToolbar.h"

class MainComponent final : public juce::Component
{
public:
    MainComponent();

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    std::unique_ptr<Menu> topLevelMenu;
    // MainToolbar mainToolbar;
    std::unique_ptr<MainToolbar> mainToolbar;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
