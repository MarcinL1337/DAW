#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

class TrackPlayerSideMenu final : public juce::Component
{
public:
    TrackPlayerSideMenu() = default;
    ~TrackPlayerSideMenu() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;

    uint16_t getCurrentNumberOfTracks() const { return currentNumberOfTracks; }
    void incrementCurrentNumberOfTracks() { currentNumberOfTracks++; }
    void decrementCurrentNumberOfTracks() { currentNumberOfTracks--; }

private:
    void drawTrackText(juce::Graphics& g) const;
    void drawTrackButtons();

    using trackButtons = std::array<std::unique_ptr<juce::TextButton>, 3>;
    std::vector<trackButtons> trackButtonsVector{};

    uint16_t currentNumberOfTracks{0u};
    const uint16_t trackButtonsSize{36u};
    const uint16_t buttonMargin{3u};
    uint16_t currentTrackGuiBoxHeight{TrackPlayerConstants::startBoxHeight};
};