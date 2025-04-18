#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Constants.h"
#include "Timeline.h"
#include "TrackGui.h"
#include "TrackGuiComponent.h"
#include "TrackPlayerSideMenu.h"

class TrackPlayer final : public juce::Component
{
public:
    explicit TrackPlayer(const juce::ValueTree& parentTree, TrackManager& trackManagerRef);
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }
    void decrementCurrentNumberOfTracks() { currentNumberOfTracks--; }

private:
    friend class TrackManager;
    void addTrack();
    void removeTrack();

    void makeNewTrackGui();
    void viewportsInit();

    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    juce::Viewport trackPlayerSideMenuViewport{};

    juce::ValueTree tree;

    Timeline timeline;
    TrackPlayerSideMenu trackPlayerSideMenu;
    TrackGuiComponent trackGuiComponent;

    std::vector<std::unique_ptr<TrackGui>> trackGuiVector{};

    const int trackButtonsSize{30};
    uint16_t currentNumberOfTracks{0u};
};