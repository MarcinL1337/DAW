#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Constants.h"
#include "ClipsBoxes.h"
#include "TimeBar.h"
#include "Timeline.h"
#include "TrackPlayerSideMenu.h"

class TrackPlayer final : public juce::Component,
                          public juce::KeyListener
{
public:
    TrackPlayer();
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDrag(const juce::MouseEvent& event) override;
    void mouseUp(const juce::MouseEvent& event) override;
    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void drawBoxes();
    void drawTrackButtons(juce::Graphics& g);
    void viewportsInit();
    void addTrack();

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }

private:
    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    juce::Viewport trackPlayerSideMenuViewport{};

    Timeline timeline{TrackPlayerConstants::startNumOfBoxes};
    TrackPlayerSideMenu trackPlayerSideMenu{};
    Component clipsBoxesComponent{};
    TimeBar timeBar{};

    // TODO (maybe): change all vectors to static_vector
    using trackButtons = std::array<std::unique_ptr<juce::TextButton>, 3>;
    std::vector<std::unique_ptr<ClipsBox>> clipsBoxesVector{};
    std::vector<trackButtons> trackButtonsVector{};
    std::vector<std::unique_ptr<juce::Label>> trackLabelsVector{};

    const int trackButtonsSize{30};
    uint16_t currentNumberOfTracks{1};
    int timeBarXOffset{0};
    juce::Point<int> lastMousePosition{};
    bool isCurrentlyDraggingTimeBar{false};
};