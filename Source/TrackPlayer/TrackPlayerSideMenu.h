#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

struct trackControls
{
    std::unique_ptr<juce::TextButton> recordButton;
    std::unique_ptr<juce::TextButton> soloButton;
    std::unique_ptr<juce::TextButton> muteButton;
    std::unique_ptr<juce::Label> trackNameLabel;
};

class TrackPlayerSideMenu final : public juce::Component,
                                  public juce::ValueTree::Listener
{
public:
    explicit TrackPlayerSideMenu(juce::ValueTree& parentTree);
    ~TrackPlayerSideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void removeTrackControls(int trackIndex);
    void addTrackControls();
    void updateTrackButtonStates(int trackIndex, bool muted, bool soloed) const;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }
    void decrementCurrentNumberOfTracks() { currentNumberOfTracks--; }

private:
    void valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property) override;
    void mouseDown(const juce::MouseEvent& event) override;

    void setupRecordButton(const std::unique_ptr<juce::TextButton>& recordButton, juce::Rectangle<int>& buttonArea,
                           const uint16_t currentRow);
    void setupSoloButton(const std::unique_ptr<juce::TextButton>& soloButton, juce::Rectangle<int>& buttonArea,
                         const uint16_t currentRow);
    void setupMuteButton(const std::unique_ptr<juce::TextButton>& muteButton, juce::Rectangle<int>& buttonArea,
                         const uint16_t currentRow);
    void setupTrackNameLabel(const std::unique_ptr<juce::Label>& trackNameLabel, juce::Rectangle<int>& trackNameArea);

    juce::Rectangle<int> getCurrentTrackButtonsArea(const uint16_t currentRow) const;
    juce::Rectangle<int> getCurrentTrackNameArea(const uint16_t currentRow) const;

    void resizeAllTrackButtons(const int newBoxHeight);

    juce::ValueTree& tree;

    std::vector<trackControls> trackControlsVector{};

    int currentSelectedTrack{0};
    uint16_t currentNumberOfTracks{0u};
    uint16_t trackButtonsSize{};
    uint16_t buttonMargin{};
    uint16_t currentTrackGuiBoxHeight{TrackPlayerConstants::startBoxHeight};
};