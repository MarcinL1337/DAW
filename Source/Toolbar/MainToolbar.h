#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ToolbarFactory.h"

class MainToolbar final : public juce::Component
{
public:
    MainToolbar();
    ~MainToolbar() override = default;

private:
    void resized() override;
    void paint(juce::Graphics& g) override;

    juce::Toolbar toolbar;
    ToolbarFactory toolbarFactory;
};
