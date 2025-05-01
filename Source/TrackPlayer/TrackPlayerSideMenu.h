#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackPlayerSideMenu final : public juce::Component
{
public:
    explicit TrackPlayerSideMenu(const juce::ValueTree& parentTree);
    ~TrackPlayerSideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    void removeTrackControls(int trackIndex);
    void addTrackControls();

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }
    void decrementCurrentNumberOfTracks() { currentNumberOfTracks--; }

    void changeCurrentTrackGuiBoxHeight(const int newBoxHeight);

private:
    juce::ValueTree tree;
    void drawTrackText(juce::Graphics& g) const;

    void initRecordButton(const std::unique_ptr<juce::TextButton>& recordButton, juce::Rectangle<int>& buttonArea,
                          const uint16_t currentRow);
    void initSoloButton(const std::unique_ptr<juce::TextButton>& soloButton, juce::Rectangle<int>& buttonArea,
                        const uint16_t currentRow);
    void initMuteButton(const std::unique_ptr<juce::TextButton>& muteButton, juce::Rectangle<int>& buttonArea,
                        const uint16_t currentRow);

    using trackButtons = std::array<std::unique_ptr<juce::TextButton>, 3>;
    std::vector<trackButtons> trackButtonsVector{};

    uint16_t currentNumberOfTracks{0u};
    uint16_t trackButtonsSize{};
    uint16_t buttonMargin{};
    uint16_t currentTrackGuiBoxHeight{TrackPlayerConstants::startBoxHeight};
};