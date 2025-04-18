#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackManager;

class TrackPlayerSideMenu final : public juce::Component
{
public:
    explicit TrackPlayerSideMenu(TrackManager& trackManagerRef);
    ~TrackPlayerSideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }
    void decrementCurrentNumberOfTracks() { currentNumberOfTracks--; }

private:
    TrackManager& trackManager;
    void drawTrackText(juce::Graphics& g) const;
    void drawTrackButtons();

    using trackButtons = std::array<std::unique_ptr<juce::TextButton>, 3>;
    std::vector<trackButtons> trackButtonsVector{};

    uint16_t currentNumberOfTracks{0u};
    const int trackButtonsSize{30};
};