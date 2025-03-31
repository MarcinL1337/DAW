#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ToolbarFactory.h"

class MainToolbar final : public juce::Component,
                          public juce::ValueTree::Listener
{
public:
    explicit MainToolbar(MainAudio& mainAudioRef, const juce::ValueTree& parentTree);
    ~MainToolbar() override = default;

private:
    void resized() override;
    void paint(juce::Graphics& g) override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    juce::ValueTree tree;

    juce::Toolbar toolbar;
    ToolbarFactory toolbarFactory;

    double timeBarPosition{0.0};
};
