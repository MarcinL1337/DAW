#include "AudioTrackManager.h"
#include <algorithm>

AudioTrackManager::AudioTrackManager(MainAudio& mainAudioRef) : mainAudio{mainAudioRef} {}

size_t AudioTrackManager::addAudioTrack()
{
    tracks.push_back(std::make_unique<AudioTrack>(mainAudio));
    return tracks.size() - 1;
}

void AudioTrackManager::removeAudioTrack(const int trackIndex)
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks.erase(tracks.begin() + trackIndex);
}

nlohmann::json AudioTrackManager::getAudioTrackJson(const int trackIndex) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    return tracks[trackIndex]->toJson();
}

void AudioTrackManager::moveTrack(const int fromIndex, const int toIndex)
{
    assert(fromIndex >= 0 && fromIndex < static_cast<int>(tracks.size()));
    assert(toIndex >= 0 && toIndex < static_cast<int>(tracks.size()));

    auto trackToMove = std::move(tracks[fromIndex]);
    tracks.erase(tracks.begin() + fromIndex);
    tracks.insert(tracks.begin() + toIndex, std::move(trackToMove));
}

NodeID AudioTrackManager::addClipToTrack(const int trackIndex, const juce::File& file) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    return tracks[trackIndex]->addAudioClip(file);
}

bool AudioTrackManager::removeClipFromTrack(const int trackIndex, const NodeID clipId) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    return tracks[trackIndex]->removeAudioClip(clipId);
}

void AudioTrackManager::setClipOffset(const NodeID clipId, const double offsetSeconds) const
{
    for(const auto& track: tracks)
    {
        const auto& clips = track->getAudioClips();
        if(std::ranges::find(clips, clipId) != clips.end())
        {
            track->setOffsetOfAudioClipInSeconds(clipId, offsetSeconds);
            return;
        }
    }
}

void AudioTrackManager::setTrackProperty(const int trackIndex, const AudioClipProperty property,
                                         const bool boolValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setProperty(property, boolValue);
}

void AudioTrackManager::setTrackProperty(const int trackIndex, const AudioClipProperty property,
                                         const float floatValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setProperty(property, floatValue);
}

void AudioTrackManager::setTrackProperty(const int trackIndex, const ReverbClipProperty property,
                                         const float floatValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setProperty(property, floatValue);
}

void AudioTrackManager::setTrackName(const int trackIndex, const juce::String& stringValue) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    tracks[trackIndex]->setTrackName(stringValue);
}

TrackProperties AudioTrackManager::getTrackProperties(const int trackIndex) const
{
    assert(trackIndex >= 0 && trackIndex < static_cast<int>(tracks.size()));
    return tracks[trackIndex]->getProperties();
}

SplitClipResult AudioTrackManager::splitClip(const int trackIndex, const NodeID clipId, const float ratio,
                                             const juce::File& audioDir) const
{
    const auto splitFile = getClipPath(clipId);
    const auto splitClipOffset = mainAudio.getAudioClipOffsetInSeconds(clipId);
    const auto splitResult = AudioClipSplitter::splitClipFile(splitFile, ratio, splitClipOffset, audioDir);
    return splitResult;
}

juce::File AudioTrackManager::getClipPath(const NodeID clipId) const { return mainAudio.getAudioClipPath(clipId); }

double AudioTrackManager::getClipOffset(const NodeID clipId) const
{
    return mainAudio.getAudioClipOffsetInSeconds(clipId);
}

std::pair<Fade::Data, Fade::Data> AudioTrackManager::getClipFadeData(const NodeID clipId) const
{
    return mainAudio.getAudioClipFadeData(clipId);
}

nlohmann::json AudioTrackManager::exportToJson() const
{
    nlohmann::json projectJson;
    projectJson["tracks"] = nlohmann::json::array();

    for(const auto& track: tracks) { projectJson["tracks"].push_back(track->toJson()); }

    return projectJson;
}

void AudioTrackManager::clearAllTracks() { tracks.clear(); }