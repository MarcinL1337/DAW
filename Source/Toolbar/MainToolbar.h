#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ToolbarFactory.h"

class MainToolbar final : public juce::Component,
                          public juce::ValueTree::Listener
{
public:
    explicit MainToolbar(juce::ValueTree& parentTree);
    ~MainToolbar() override = default;

private:
    void resized() override;
    void paint(juce::Graphics& g) override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void paintTimeBarValue(juce::Graphics& g);
    void paintSplitSecondsValue(juce::Graphics& g);
    void initTimeBarValueLabel();
    void initSplitSecondsLabel();

    juce::Label timeBarValueLabel;
    juce::Label splitSecondsLabel;
    juce::ValueTree& tree;
    juce::Toolbar toolbar;
    ToolbarFactory toolbarFactory;

    std::ostringstream oss;
    float timeBarValue{0.0f};
    float splitSeconds{0.0f};
    bool isClipSplitActive{false};
    juce::Rectangle<int> timeBarValueArea{};
    juce::Rectangle<int> splitSecondsArea{};
};
