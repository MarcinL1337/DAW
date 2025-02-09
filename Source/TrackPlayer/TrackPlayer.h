#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "ClipsBoxes.h"
#include "Timeline.h"

class TrackPlayer final : public juce::Component
{
public:
    TrackPlayer();
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void flexBoxInit();
    void drawBoxes();
    void viewportsInit();

    int getClipsBoxesComponentWidth() const { return clipsBoxesComponent.getWidth(); }

private:
    const int tempNumOfBoxes{51};
    juce::FlexBox trackPlayerFlexBox{};
    juce::FlexBox clipsBoxesFlexBox{};
    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    Timeline timeline{tempNumOfBoxes};
    Component clipsBoxesComponent{};
    std::vector<std::unique_ptr<ClipsBox>> clipsBoxesVector{};
};