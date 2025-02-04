#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class SideMenu final : juce::Component
{
public:
    SideMenu() = default;
    ~SideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
};