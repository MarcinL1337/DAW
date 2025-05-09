#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <cassert>
#include "../Audio/MainAudio.h"
#include "BinaryData.h"

class ToolbarFactory final : public juce::ToolbarItemFactory,
                             public juce::Button::Listener
{
public:
    ToolbarFactory() = delete;
    explicit ToolbarFactory(juce::ValueTree& valueTree);
    ~ToolbarFactory() override = default;

private:
    void initPlayheadFollowMode() const;

    void getAllToolbarItemIds(juce::Array<int>& ids) override;
    void getDefaultItemSet(juce::Array<int>& ids) override;
    juce::ToolbarItemComponent* createItem(int itemId) override;
    void buttonClicked(juce::Button* button) override;

    juce::ToolbarButton* createButtonFromImage(int itemId, const juce::String& shortDescription);
    void temporaryButtonsFunction(juce::String buttonName);
    void previousButtonClicked();
    void nextButtonClicked();
    void replayButtonClicked();
    void playPauseButtonClicked() const;
    void startRecordingButtonClicked();
    void stopButtonClicked() const;
    void followModeButtonClicked() const;

    enum SpecialItemIds
    {
        separatorBarId = -1,  /**< The item ID for a vertical (or horizontal) separator bar that
                                   can be placed between sets of items to break them into groups. */
        spacerId = -2,        /**< The item ID for a fixed-width space that can be placed between
                                   items.*/
        flexibleSpacerId = -3 /**< The item ID for a gap that pushes outwards against the things on
                                   either side of it, filling any available space. */
    };

    enum ToolbarElementsIds
    {
        previous = 1,
        next,
        replay,
        playPause,
        startRecording,
        stop,
        followMode
    };

    juce::ToolbarButton* previousButton{nullptr};
    juce::ToolbarButton* nextButton{nullptr};
    juce::ToolbarButton* playPauseButton{nullptr};
    juce::ToolbarButton* replayButton{nullptr};
    juce::ToolbarButton* startRecordingButton{nullptr};
    juce::ToolbarButton* stopButton{nullptr};
    juce::ToolbarButton* followModeButton{nullptr};

    juce::ValueTree& tree;
};

