#pragma once

#include <utility>
#include <vector>

#include "Audio/AudioTrack.h"
#include "Audio/MainAudio.h"
#include "Constants.h"
#include "SideMenu/SideMenu.h"
#include "TrackPlayer/TrackGuiManager.h"

class TrackManager final : public juce::Component,
                           public juce::KeyListener,
                           public juce::ValueTree::Listener
{
public:
    TrackManager(TrackGuiManager& trackGuiManagerRef, MainAudio& mainAudioRef, SideMenu& sideMenuRef);
    ~TrackManager() override;

    int addTrack();
    void removeTrack(int trackIndex);
    int duplicateTrack(int trackIndex);
    int createTrackFromJson(const nlohmann::json& trackJson);
    bool changeTrackOrder(int trackIndex, int newPosition);  // to be implemented

    NodeID addAudioClipToTrack(int trackIndex, const juce::File& file) const;
    void setOffsetOfAudioClipInSeconds(NodeID nodeID, double offsetSeconds) const;
    bool removeAudioClipFromTrack(const int trackIndex, const NodeID clipId) const;
    void splitAudioClip(const int trackIndex, const NodeID clipId, const float waveformSplitRatio) const;
    void addNewAudioClipsBySplit(const int trackIndex, const juce::File& fileToBeSplit, const float waveformSplitRatio,
                                 const double splitClipOffset) const;
    static void handleWriteToFile(juce::AudioFormatReader& reader, const juce::AudioFormatManager& formatManager,
                                  const juce::File& destFile, const int numOfSamplesToWrite,
                                  const int readerOffsetInSamples);
    bool moveAudioClipBetweenTracks(int sourceTrackIndex, int destTrackIndex, NodeID clipId);  // to be implemented

    bool keyPressed(const juce::KeyPress& key, Component* originatingComponent) override;

    void setTrackProperty(int trackIndex, AudioClipProperty property, bool boolValue) const;
    void setTrackProperty(int trackIndex, AudioClipProperty property, float floatValue) const;
    TrackProperties getTrackProperties(int trackIndex) const;

    void valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                  const juce::Identifier& property) override;

private:
    void chooseNewNamesForSplitFiles(juce::String& firstFile, juce::String& secondFile,
                                     const juce::String& extension) const;
    void handleSplitClipsDirCreation() const;

    TrackGuiManager& trackGuiManager;
    MainAudio& mainAudio;
    SideMenu& sideMenu;
    juce::ValueTree& tree;
    std::vector<std::unique_ptr<AudioTrack>> tracks;
    std::optional<juce::File> currentlyCopiedClipFilePath{std::nullopt};
    const juce::File tempClipsFolder{"../../../Assets/Audio/TemporarySplitClips"};
};
