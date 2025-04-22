#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef) :
    trackPlayer(trackPlayerRef), mainAudio(mainAudioRef)
{
    juce::Timer::callAfterDelay(50, [&] { addTrack(); });

    juce::Timer::callAfterDelay(200,
                                [this]
                                {
                                    if(auto* parent = getParentComponent())
                                        parent->addKeyListener(this);
                                });
}

int TrackManager::addTrack()
{
    trackPlayer.addTrack();
    tracks.push_back(std::make_unique<AudioTrack>(nextTrackId, mainAudio));
    return nextTrackId++;
}

bool TrackManager::removeTrack(const int trackId)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return false;

    tracks.erase(tracks.begin() + trackPositionInVector);
    trackPlayer.removeTrack();

    return true;
}

NodeID TrackManager::addAudioClipToTrack(const int trackId, const juce::File& file) const
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return TrackManagerConstants::InvalidNodeID;

    // TODO: handle file opening in trackManager?
    trackPlayer.handleNewAudioFileOpened(file.getFullPathName());

    return tracks[trackPositionInVector]->addAudioClip(file);
}

void TrackManager::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    for(const auto& track: tracks)
    {
        const auto& clips = track->getAudioClips();
        if(std::ranges::find(clips, nodeID) != clips.end())
        {
            track->setOffsetOfAudioClipInSeconds(nodeID, offsetSeconds);
            return;
        }
    }
}

uint32_t TrackManager::getTrackPositionInVectorById(const int trackId) const
{
    for(size_t i = 0; i < tracks.size(); ++i)
        if(tracks[i]->getId() == trackId)
            return static_cast<int>(i);

    return TrackManagerConstants::InvalidTrackPositionInVector;
}

bool TrackManager::keyPressed(const juce::KeyPress& key, Component* originatingComponent)
{
    if(key.getModifiers().isShiftDown() && key.getTextCharacter() == '+')
    {
        addTrack();
        return true;
    }
    if(key.getModifiers().isShiftDown() && key.getTextCharacter() == '_')
    {
        if(!tracks.empty())
            removeTrack(tracks.back()->getId());
        return true;
    }
    return false;
}

void TrackManager::setPropertyForAllClipsInTrack(const int trackId, const AudioClipProperty property,
                                                 const bool boolValue)
{
    auto const trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return;
    tracks[trackPositionInVector]->setProperty(property, boolValue);
}

void TrackManager::setPropertyForAllClipsInTrack(const int trackId, const AudioClipProperty property,
                                                 const float floatValue)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return;
    tracks[trackPositionInVector]->setProperty(property, floatValue);
}

/*
bool TrackManager::removeAudioClipFromTrack(const int trackId, const NodeID clipId)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return false;

    return tracks[trackPositionInVector]->removeAudioClip(clipId);
}

bool TrackManager::moveAudioClipBetweenTracks(const int sourceTrackId, const int destTrackId, const NodeID clipId)
{
    const auto sourcePositionInVector = getTrackPositionInVectorById(sourceTrackId);
    const auto destPositionInVector = getTrackPositionInVectorById(destTrackId);

    if(sourcePositionInVector == TrackManagerConstants::InvalidTrackPositionInVector ||
       destPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return false;

    auto& sourceClips = tracks[sourcePositionInVector]->getAudioClips();
    const auto it = std::ranges::find(sourceClips, clipId);

    if(it == sourceClips.end())
        return false;

    sourceClips.erase(it);
    tracks[destPositionInVector]->getAudioClips().push_back(clipId);

    return true;
}

bool TrackManager::changeTrackOrder(const int trackId, const uint32_t newPosition)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector ||
       newPosition >= static_cast<int>(tracks.size()))
        return false;

    auto trackToMove = std::move(tracks[trackPositionInVector]);
    tracks.erase(tracks.begin() + trackPositionInVector);
    tracks.insert(tracks.begin() + newPosition, std::move(trackToMove));

    return true;
}
*/