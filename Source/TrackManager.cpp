#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef) :
    trackPlayer(trackPlayerRef), mainAudio(mainAudioRef)
{
    addKeyListener(this);
    juce::Timer::callAfterDelay(50, [&] { addTrack(); });
}

int TrackManager::addTrack()
{
    trackPlayer.addTrack();
    tracks.push_back({nextTrackId, std::vector<NodeID>{}, TrackProperties{}});
    return nextTrackId++;
}

bool TrackManager::removeTrack(const int trackId)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return false;

    for(const auto& clipId: tracks[trackPositionInVector].audioClips) { mainAudio.removeAudioClip(clipId); }

    tracks.erase(tracks.begin() + trackPositionInVector);
    trackPlayer.removeTrack();

    return true;
}

bool TrackManager::changeTrackOrder(const int trackId, const uint32_t newPosition)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector ||
       newPosition >= static_cast<int>(tracks.size()))
        return false;

    const auto trackToMove = tracks[trackPositionInVector];
    tracks.erase(tracks.begin() + trackPositionInVector);
    tracks.insert(tracks.begin() + newPosition, trackToMove);

    return true;
}

NodeID TrackManager::addAudioClipToTrack(const int trackId, const juce::File& file)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return TrackManagerConstants::InvalidNodeID;

    const NodeID clipId = mainAudio.addAudioClip(file);
    tracks[trackPositionInVector].audioClips.push_back(clipId);

    const auto& [gain, pan, mute, solo] = tracks[trackPositionInVector].properties;
    mainAudio.setGainOfAudioClip(clipId, gain);
    mainAudio.setPanOfAudioClip(clipId, pan);
    mainAudio.setMuteOfAudioClip(clipId, mute);
    mainAudio.setSoloOfAudioClip(clipId, solo);

    return clipId;
}

void TrackManager::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    mainAudio.setOffsetOfAudioClipInSeconds(nodeID, offsetSeconds);
}

bool TrackManager::removeAudioClipFromTrack(const int trackId, const NodeID clipId)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return false;

    auto& clips = tracks[trackPositionInVector].audioClips;
    const auto it = std::ranges::find(clips, clipId);

    if(it == clips.end())
        return false;

    mainAudio.removeAudioClip(clipId);
    clips.erase(it);

    return true;
}

bool TrackManager::moveAudioClipBetweenTracks(const int sourceTrackId, const int destTrackId, const NodeID clipId)
{
    const auto sourcePositionInVector = getTrackPositionInVectorById(sourceTrackId);
    const auto destPositionInVector = getTrackPositionInVectorById(destTrackId);

    if(sourcePositionInVector == TrackManagerConstants::InvalidTrackPositionInVector ||
       destPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return false;

    auto& sourceClips = tracks[sourcePositionInVector].audioClips;
    const auto it = std::ranges::find(sourceClips, clipId);

    if(it == sourceClips.end())
        return false;

    sourceClips.erase(it);
    tracks[destPositionInVector].audioClips.push_back(clipId);

    return true;
}

uint32_t TrackManager::getTrackPositionInVectorById(const int trackId) const
{
    for(size_t i = 0; i < tracks.size(); ++i)
        if(tracks[i].id == trackId)
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
            removeTrack(tracks.back().id);
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

    auto& properties = tracks[trackPositionInVector].properties;
    switch(property)
    {
        case AudioClipProperty::MUTE:
            properties.mute = boolValue;
            break;
        case AudioClipProperty::SOLO:
            properties.solo = boolValue;
            break;
        default:
            break;
    }

    for(const auto& clipId: tracks[trackPositionInVector].audioClips) switch(property)
        {
            case AudioClipProperty::MUTE:
                mainAudio.setMuteOfAudioClip(clipId, boolValue);
                break;
            case AudioClipProperty::SOLO:
                mainAudio.setSoloOfAudioClip(clipId, boolValue);
                break;
            default:
                break;
        }
}

void TrackManager::setPropertyForAllClipsInTrack(const int trackId, const AudioClipProperty property,
                                                 const float floatValue)
{
    const auto trackPositionInVector = getTrackPositionInVectorById(trackId);
    if(trackPositionInVector == TrackManagerConstants::InvalidTrackPositionInVector)
        return;

    auto& properties = tracks[trackPositionInVector].properties;
    switch(property)
    {
        case AudioClipProperty::GAIN:
            properties.gain = floatValue;
            break;
        case AudioClipProperty::PAN:
            properties.pan = floatValue;
            break;
        default:
            break;
    }

    for(const auto& clipId: tracks[trackPositionInVector].audioClips) switch(property)
        {
            case AudioClipProperty::GAIN:
                mainAudio.setGainOfAudioClip(clipId, floatValue);
                break;
            case AudioClipProperty::PAN:
                mainAudio.setPanOfAudioClip(clipId, floatValue);
                break;
            default:
                break;
        }
}