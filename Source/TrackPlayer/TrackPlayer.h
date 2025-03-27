#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Constants.h"
#include "ClipsBoxes.h"
#include "Timeline.h"
#include "TrackPlayerSideMenu.h"

class TrackPlayer final : public juce::Component,
                          public juce::KeyListener,
                          public juce::ValueTree::Listener
{
public:
    TrackPlayer(const juce::ValueTree& parentTree);
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent& event) override;
    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void drawBoxes();
    void drawTrackButtons();
    void drawTrackText(juce::Graphics& g) const;
    void viewportsInit();
    void addTrack();

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }

private:
    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    juce::Viewport trackPlayerSideMenuViewport{};

    juce::ValueTree tree;

    Timeline timeline;
    TrackPlayerSideMenu trackPlayerSideMenu{};
    Component clipsBoxesComponent{};

    // TODO (maybe): change all vectors to static_vector
    using trackButtons = std::array<std::unique_ptr<juce::TextButton>, 3>;
    std::vector<std::unique_ptr<ClipsBox>> clipsBoxesVector{};
    std::vector<trackButtons> trackButtonsVector{};
    std::vector<std::unique_ptr<juce::Label>> trackLabelsVector{};

    const int trackButtonsSize{30};
    uint16_t currentNumberOfTracks{1};
    float timeBarTime{};
};