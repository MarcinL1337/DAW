#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Constants.h"
#include "ClipsBoxes.h"
#include "ClipsBoxesComponent.h"
#include "Timeline.h"
#include "TrackPlayerSideMenu.h"

class TrackPlayer final : public juce::Component,
                          public juce::KeyListener
{
public:
    explicit TrackPlayer(const juce::ValueTree& parentTree);
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }

private:
    void drawBoxes();
    void viewportsInit();
    void addTrack();

    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    juce::Viewport trackPlayerSideMenuViewport{};

    juce::ValueTree tree;

    Timeline timeline;
    TrackPlayerSideMenu trackPlayerSideMenu{};
    ClipsBoxesComponent clipsBoxesComponent;

    std::vector<std::unique_ptr<ClipsBox>> clipsBoxesVector{};

    const int trackButtonsSize{30};
    uint16_t currentNumberOfTracks{1};
};