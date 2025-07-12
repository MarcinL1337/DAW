#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "TimeBar.h"

class Timeline final : public juce::Component,
                       public juce::ValueTree::Listener
{
public:
    explicit Timeline(int numOfSeconds, const juce::ValueTree& parentTree);
    ~Timeline() override = default;

    void changeBoxWidth(const uint16_t newBoxWidth) { currentTrackGuiBoxWidth = newBoxWidth; }
    void changeNumOfSeconds(const int newNumOfSeconds) { currentNumOfSeconds = newNumOfSeconds; }

protected:
    bool hitTest(int x, int y) override;

private:
    void paint(juce::Graphics& g) override;
    void resized() override;
    void drawLineOnTimeline(juce::Graphics& g, uint32_t lineNumber) const;

    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;
    void mouseUp(const juce::MouseEvent& event) override;

    juce::ValueTree tree;
    juce::Identifier timeBarTime{"timeBarTime"};

    TimeBar timeBar{};

    int timeBarXOffset{0};
    int currentNumOfSeconds;
    bool isCurrentlyDraggingTimeBar{false};
    float timeBarTimeInSeconds{0.0f};
    uint16_t currentTrackGuiBoxWidth{TrackPlayerConstants::startBoxWidth};
};