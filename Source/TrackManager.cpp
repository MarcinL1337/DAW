#include "TrackManager.h"
#include <algorithm>

TrackManager::TrackManager(TrackPlayer& trackPlayerRef, MainAudio& mainAudioRef) :
    trackPlayer(trackPlayerRef), mainAudio(mainAudioRef)
{
    juce::Timer::callAfterDelay(50, [&] { addTrack(); });
}

int TrackManager::addTrack()
{
    trackPlayer.addTrack();
    tracks.push_back({nextTrackId, std::vector<NodeID>{}, TrackSettings{}});
    return nextTrackId++;
}

bool TrackManager::removeTrack(const int trackId)
{
    const int trackIndex = getTrackIndexById(trackId);
    if(trackIndex == -1)
        return false;

    for(const auto& clipId: tracks[trackIndex].audioClips) { mainAudio.removeAudioClip(clipId); }

    tracks.erase(tracks.begin() + trackIndex);
    trackPlayer.removeTrack();

    return true;
}

bool TrackManager::changeTrackOrder(const int trackId, const int newPosition)
{
    const int trackIndex = getTrackIndexById(trackId);
    if(trackIndex == -1 || newPosition < 0 || newPosition >= static_cast<int>(tracks.size()))
        return false;

    const auto trackToMove = tracks[trackIndex];
    tracks.erase(tracks.begin() + trackIndex);
    tracks.insert(tracks.begin() + newPosition, trackToMove);

    return true;
}

NodeID TrackManager::addAudioClipToTrack(const int trackId, const juce::File& file)
{
    const int trackIndex = getTrackIndexById(trackId);
    if(trackIndex == -1)
        return {};

    const NodeID clipId = mainAudio.addAudioClip(file);
    tracks[trackIndex].audioClips.push_back(clipId);

    const auto& [gain, pan, mute, solo] = tracks[trackIndex].settings;
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
    const int trackIndex = getTrackIndexById(trackId);
    if(trackIndex == -1)
        return false;

    auto& clips = tracks[trackIndex].audioClips;
    const auto it = std::ranges::find(clips, clipId);

    if(it == clips.end())
        return false;

    mainAudio.removeAudioClip(clipId);
    clips.erase(it);

    return true;
}

bool TrackManager::moveAudioClipBetweenTracks(const int sourceTrackId, const int destTrackId, const NodeID clipId)
{
    const int sourceIndex = getTrackIndexById(sourceTrackId);
    const int destIndex = getTrackIndexById(destTrackId);

    if(sourceIndex == -1 || destIndex == -1)
        return false;

    auto& sourceClips = tracks[sourceIndex].audioClips;
    const auto it = std::ranges::find(sourceClips, clipId);

    if(it == sourceClips.end())
        return false;

    sourceClips.erase(it);
    tracks[destIndex].audioClips.push_back(clipId);

    return true;
}

int TrackManager::getTrackIndexById(const int trackId) const
{
    for(size_t i = 0; i < tracks.size(); ++i)
        if(tracks[i].id == trackId)
            return static_cast<int>(i);

    return -1;
}

bool TrackManager::keyPressed(const juce::KeyPress& key)
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
    int const trackIndex = getTrackIndexById(trackId);
    if(trackIndex == -1)
        return;

    auto& settings = tracks[trackIndex].settings;
    switch(property)
    {
        case AudioClipProperty::MUTE:
            settings.mute = boolValue;
            break;
        case AudioClipProperty::SOLO:
            settings.solo = boolValue;
            break;
        default:
            break;
    }

    for(const auto& clipId: tracks[trackIndex].audioClips) switch(property)
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
    const int trackIndex = getTrackIndexById(trackId);
    if(trackIndex == -1)
        return;

    auto& settings = tracks[trackIndex].settings;
    switch(property)
    {
        case AudioClipProperty::GAIN:
            settings.gain = floatValue;
            break;
        case AudioClipProperty::PAN:
            settings.pan = floatValue;
            break;
        default:
            break;
    }

    for(const auto& clipId: tracks[trackIndex].audioClips) switch(property)
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