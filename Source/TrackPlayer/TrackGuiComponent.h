#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackGuiComponent final : public juce::Component,
                                public juce::ValueTree::Listener
{
public:
    explicit TrackGuiComponent(const juce::ValueTree& parentTree);
    ~TrackGuiComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void changeBoxWidth(uint16_t newBoxWidth);

private:
    juce::ValueTree tree;

    float timeBarTime{};
    float timeBarPosition{};
    uint16_t currentTrackGuiBoxWidth{TrackPlayerConstants::startBoxWidth};
};