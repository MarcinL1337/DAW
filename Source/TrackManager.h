#pragma once

#include <map>
#include <vector>
#include "Audio/MainAudio.h"
#include "TrackPlayer/TrackPlayer.h"

using NodeID = juce::AudioProcessorGraph::NodeID;

class TrackManager
{
public:
    TrackManager(MainAudio& mainAudioRef, TrackPlayer& trackPlayerRef);
    ~TrackManager() = default;

    void addEmptyTrack() const;
    NodeID addAudioToTrack(const juce::File& file, int trackIndex);
    NodeID addAudioToTrack(const juce::File& file, int trackIndex) const;
    NodeID addAudio(const juce::File& file) const;
    NodeID addTrack(const juce::File& file);
    void removeAudio(NodeID nodeID);

    // Usuwa audio track o podanym ID
    void removeAudioTrack(NodeID nodeID);

    // Przypisuje audio track do konkretnego visual tracka
    void assignAudioToTrack(NodeID audioNodeID, int trackIndex);

    // Ustawia parametry dla tracka audio
    void setPanOfTrack(NodeID nodeID, float pan) const;
    void setGainOfTrack(NodeID nodeID, float gain) const;
    void setOffsetOfTrackInSeconds(NodeID nodeID, double offsetSeconds) const;
    void setSoloOfTrack(NodeID nodeID, bool solo) const;
    void setMuteOfTrack(NodeID nodeID, bool mute) const;

    // Główne operacje transportu
    void play() const;
    void pause() const;
    void stop() const;
    void seek(int64_t positionSamples) const;

    // Pobiera czas dla wizualizacji timebara
    int64_t getCurrentPositionInSamples() const;
    double getCurrentPositionInSeconds() const;

    // Pobiera długość audio tracka w sekundach
    double getAudioTrackLengthInSeconds(NodeID nodeID) const;

    // Zwraca wszystkie audio tracki przypisane do danego visual tracka
    std::vector<NodeID> getAudioTracksForVisualTrack(int trackIndex) const;

private:
    MainAudio& mainAudio;
    TrackPlayer& trackPlayer;

    // Klucz: indeks visual tracka, Wartość: lista ID audio tracków
    std::map<int, std::vector<NodeID>> trackAssignments;
};