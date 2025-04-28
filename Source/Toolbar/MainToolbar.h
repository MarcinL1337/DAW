#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "ToolbarFactory.h"

class MainToolbar final : public juce::Component,
                          public juce::ValueTree::Listener
{
public:
    explicit MainToolbar(const juce::ValueTree& parentTree);
    ~MainToolbar() override = default;

private:
    void resized() override;
    void paint(juce::Graphics& g) override;

    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;

    void paintTimeBarValue(juce::Graphics& g);
    void initTimeBarValueLabel();

    juce::Label timeBarValueLabel;
    juce::ValueTree tree;
    juce::Toolbar toolbar;
    ToolbarFactory toolbarFactory;

    std::ostringstream oss;
    float timeBarValue{0.0f};
    juce::Rectangle<int> timeBarValueArea{};
};
