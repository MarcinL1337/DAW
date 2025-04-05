#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class ClipsBoxesComponent : public juce::Component,
                            public juce::ValueTree::Listener
{
public:
    explicit ClipsBoxesComponent(const juce::ValueTree& parentTree);
    ~ClipsBoxesComponent() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

private:
    juce::ValueTree tree;

    float timeBarTime{};
    float timeBarPosition{};
};