#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"
#include "ClipSplitBar.h"
#include "Waveform.h"

class TrackGuiManager;

class TrackGui final : public juce::Component,
                       public juce::ValueTree::Listener,
                       public juce::DragAndDropTarget
{
public:
    explicit TrackGui(uint16_t boxWidth, int numOfSeconds, juce::ValueTree& parentTree);

    TrackGui(const TrackGui&) = delete;
    TrackGui& operator=(const TrackGui&) = delete;
    ~TrackGui() override { tree.removeListener(this); }

    float getBoxWidthToFloat() const { return currentBoxWidth; }
    float getBoxHeightToFloat() const { return currentBoxHeight; }
    uint16_t getBoxWidth() const { return currentBoxWidth; }
    uint16_t getBoxHeight() const { return currentBoxHeight; }
    void changeBoxWidth(uint16_t newBoxWidth);
    void changeBoxHeight(uint16_t newBoxHeight);
    void changeNumOfSeconds(const int numOfSeconds) { currentNumOfSeconds = numOfSeconds; }

    bool hasNoAudioClips() const { return waveforms.empty(); }
    void addNewAudioFile(const juce::String& newAudioFilePath, NodeID newAudioClipID);
    void setOffsetOfWaveformInSeconds(NodeID audioClipID, double offsetSeconds) const;

private:
    friend class TrackGuiComponent;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseMove(const juce::MouseEvent& event) override;
    void mouseEnter(const juce::MouseEvent& event) override;
    void mouseExit(const juce::MouseEvent& event) override;
    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

    void makeNewWaveformFromAudioFilePath(const juce::String& newAudioFilePath, NodeID newAudioClipID);
    static juce::PopupMenu initPopUpMenuForTrack();
    static juce::PopupMenu initPopUpMenuForClip();
    void showPopUpMenuForTrack(const float clickOffset);
    void showPopUpMenuForClip(const Waveform& clipWaveform);
    void triggerTrackGuiAction(const juce::Identifier& actionId) const;
    void handleClipDelete(const Waveform& clipWaveform);
    void handleClipDelete(NodeID audioClipID);
    void handleClipCopy(const Waveform& clipWaveform);
    void handleClipCut(const Waveform& clipWaveform);
    void handleClipPaste(const float clickOffset);
    void handleClipSplit(const juce::MouseEvent& event);

    void handleLeftMouseClick(const juce::MouseEvent& event);
    void handleRightMouseClick(const juce::MouseEvent& event);

    bool isInterestedInDragSource(const SourceDetails& dragSourceDetails) override;
    void itemDropped(const SourceDetails& dragSourceDetails) override;
    void itemDragEnter(const SourceDetails& dragSourceDetails) override;
    void itemDragExit(const SourceDetails& dragSourceDetails) override;

    enum popUpMenuOptions
    {
        noOptionChosen = 0,
        deleteTrack,
        duplicateTrack,
        deleteAudioClip,
        copyAudioClip,
        cutAudioClip,
        pasteAudioClip
    };

    inline static bool isAnyWaveformCopied{false};
    inline static bool isClipSplitActive{false};
    static ClipSplitBar clipSplitBar;

    std::vector<std::unique_ptr<Waveform>> waveforms{};
    juce::ValueTree& tree;

    uint16_t currentBoxWidth;
    uint16_t currentBoxHeight{TrackPlayerConstants::startBoxHeight};
    int currentNumOfSeconds{};
};