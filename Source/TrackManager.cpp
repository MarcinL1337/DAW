#include "TrackManager.h"

TrackManager::TrackManager(MainAudio& mainAudioRef, TrackPlayer& trackPlayerRef) :
    mainAudio(mainAudioRef), trackPlayer(trackPlayerRef)
{}
// maska bitowa, która określa które tracki są zajętę
void TrackManager::addEmptyTrack() const { trackPlayer.addTrack(); }

NodeID TrackManager::addAudioToTrack(const juce::File& file, const int trackIndex)
{
    const auto audioNodeID = mainAudio.addTrack(file);
    trackAssignments[trackIndex].push_back(audioNodeID);
    return audioNodeID;
}

NodeID TrackManager::addTrack(const juce::File& file)
{
    const auto audioNodeID = mainAudio.addTrack(file);

    trackPlayer.addTrack();
    assignAudioToTrack(audioNodeID, trackPlayer.getCurrentNumberOfTracks() - 1);
    return audioNodeID;
}

void TrackManager::removeAudio(const NodeID nodeID)
{
    mainAudio.removeTrack(nodeID);

    // Usuń przypisania tego track z mapy
    for(auto& [trackIndex, audioTracks]: trackAssignments)
    {
        audioTracks.erase(std::remove(audioTracks.begin(), audioTracks.end(), nodeID), audioTracks.end());
    }
}

void TrackManager::assignAudioToTrack(const NodeID audioNodeID, const int trackIndex)
{
    // Sprawdź czy visual track istnieje
    if(trackIndex >= 0 && trackIndex < trackPlayer.getCurrentNumberOfTracks())
    {
        // Dodaj audio track do listy powiązanej z visual trackiem
        trackAssignments[trackIndex].push_back(audioNodeID);
    }
}

void TrackManager::setPanOfTrack(const NodeID nodeID, const float pan) const { mainAudio.setPanOfTrack(nodeID, pan); }

void TrackManager::setGainOfTrack(const NodeID nodeID, const float gain) const
{
    mainAudio.setGainOfTrack(nodeID, gain);
}

void TrackManager::setOffsetOfTrackInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    mainAudio.setOffsetOfTrackInSeconds(nodeID, offsetSeconds);
}

void TrackManager::setSoloOfTrack(const NodeID nodeID, const bool solo) const
{
    mainAudio.setSoloOfTrack(nodeID, solo);
}

void TrackManager::setMuteOfTrack(const NodeID nodeID, const bool mute) const
{
    mainAudio.setMuteOfTrack(nodeID, mute);
}

void TrackManager::play() const { mainAudio.play(); }

void TrackManager::pause() const { mainAudio.pause(); }

void TrackManager::stop() const { mainAudio.stop(); }

void TrackManager::seek(const int64_t positionSamples) const { mainAudio.seek(positionSamples); }

int64_t TrackManager::getCurrentPositionInSamples() const
{
    if(auto posInfo = mainAudio.getPosition())
        if(auto samplePos = posInfo->getTimeInSamples())
            return *samplePos;
    return 0;
}

double TrackManager::getCurrentPositionInSeconds() const
{
    return static_cast<double>(getCurrentPositionInSamples()) / mainAudio.getSampleRate();
}

std::vector<NodeID> TrackManager::getAudioTracksForVisualTrack(const int trackIndex) const
{
    auto it = trackAssignments.find(trackIndex);
    if(it != trackAssignments.end())
    {
        return it->second;
    }
    return {};  // Pusty wektor, jeśli nie ma przypisanych tracków
}