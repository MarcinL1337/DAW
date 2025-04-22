#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Constants.h"
#include "Timeline.h"
#include "TrackGui.h"
#include "TrackGuiComponent.h"
#include "TrackPlayerSideMenu.h"

class TrackPlayer final : public juce::Component,
                          public juce::KeyListener,
                          public juce::ValueTree::Listener
{
public:
    explicit TrackPlayer(const juce::ValueTree& parentTree, TrackManager& trackManagerRef);
    TrackPlayer(const TrackPlayer&) = delete;
    TrackPlayer& operator=(const TrackPlayer&) = delete;
    ~TrackPlayer() override = default;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }

private:
    friend class TrackManager;
    void paint(juce::Graphics& g) override;
    void resized() override;

    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }
    void decrementCurrentNumberOfTracks() { currentNumberOfTracks--; }

    void makeNewTrackGui(const juce::String& newAudioFilePath = "");
    void viewportsInit();
    void addTrack(const juce::String& newAudioFilePath = "");
    void removeTrack();
    void handleNewAudioFileOpened(const juce::String& newAudioFilePath);
    TrackGui* findFirstEmptyTrackGui() const;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void changeTrackGuiBoxWidthAndPropagate(uint16_t newBoxWidth);

    juce::Viewport trackPlayerViewport{};
    juce::Viewport timelineViewport{};
    juce::Viewport trackPlayerSideMenuViewport{};

    juce::ValueTree tree;

    int currentNumOfSeconds{TrackPlayerConstants::startNumOfBoxes};
    Timeline timeline;
    TrackPlayerSideMenu trackPlayerSideMenu;
    TrackGuiComponent trackGuiComponent;

    std::vector<std::unique_ptr<TrackGui>> trackGuiVector{};

    const int trackButtonsSize{30};
    uint16_t currentNumberOfTracks{0u};
    uint16_t currentTrackGuiBoxHeight{TrackPlayerConstants::startBoxHeight};
    uint16_t currentTrackGuiBoxWidth{TrackPlayerConstants::startBoxWidth};
};