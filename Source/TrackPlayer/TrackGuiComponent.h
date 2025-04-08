#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackGuiComponent : public juce::Component,
                          public juce::ValueTree::Listener
{
public:
    explicit TrackGuiComponent(const juce::ValueTree& parentTree);
    ~TrackGuiComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

private:
    juce::ValueTree tree;

    float timeBarTime{};
    float timeBarPosition{};
};