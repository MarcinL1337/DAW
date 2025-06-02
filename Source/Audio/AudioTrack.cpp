#include "AudioTrack.h"
#include <algorithm>

AudioTrack::AudioTrack(MainAudio& mainAudioRef) : mainAudio(mainAudioRef) {}

AudioTrack::~AudioTrack()
{
    for(const auto& clipId: audioClips) { mainAudio.removeAudioClip(clipId); }
}

NodeID AudioTrack::addAudioClip(const juce::File& file)
{
    const NodeID clipId = mainAudio.addAudioClip(file);
    audioClips.push_back(clipId);

    mainAudio.setGainOfAudioClip(clipId, properties.gain);
    mainAudio.setPanOfAudioClip(clipId, properties.pan);
    mainAudio.setMuteOfAudioClip(clipId, properties.mute);
    mainAudio.setSoloOfAudioClip(clipId, properties.solo);

    return clipId;
}

bool AudioTrack::removeAudioClip(const NodeID clipId)
{
    const auto it = std::ranges::find(audioClips, clipId);

    if(it == audioClips.end())
        return false;

    mainAudio.removeAudioClip(clipId);
    audioClips.erase(it);

    return true;
}

void AudioTrack::setOffsetOfAudioClipInSeconds(const NodeID nodeID, const double offsetSeconds) const
{
    mainAudio.setOffsetOfAudioClipInSeconds(nodeID, offsetSeconds);
}

// TODO: use template from pull request #23 https://github.com/MarcinL1337/DAW/pull/23#discussion_r2058940747
void AudioTrack::setProperty(const AudioClipProperty property, const bool boolValue)
{
    switch(property)
    {
        case AudioClipProperty::MUTE:
            properties.mute = boolValue;
            for(const auto& clipId: audioClips) { mainAudio.setMuteOfAudioClip(clipId, boolValue); }
            break;
        case AudioClipProperty::SOLO:
            properties.solo = boolValue;
            for(const auto& clipId: audioClips) { mainAudio.setSoloOfAudioClip(clipId, boolValue); }
            break;
        default:
            std::unreachable();
    }
}

void AudioTrack::setProperty(const AudioClipProperty property, const float floatValue)
{
    switch(property)
    {
        case AudioClipProperty::GAIN:
            properties.gain = floatValue;
            for(const auto& clipId: audioClips) { mainAudio.setGainOfAudioClip(clipId, floatValue); }
            break;
        case AudioClipProperty::PAN:
            properties.pan = floatValue;
            for(const auto& clipId: audioClips) { mainAudio.setPanOfAudioClip(clipId, floatValue); }
            break;
        default:
            std::unreachable();
    }
}

TrackProperties AudioTrack::getProperties() const { return properties; }

nlohmann::json AudioTrack::toJson() const
{
    nlohmann::json j;
    j["properties"] = {
        {"gain", properties.gain}, {"pan", properties.pan}, {"mute", properties.mute}, {"solo", properties.solo}};

    j["audioClips"] = nlohmann::json::array();
    for(const auto& clipId: audioClips)
    {
        auto clipPath = mainAudio.getAudioClipPath(clipId);
        auto offset = mainAudio.getAudioClipOffsetInSeconds(clipId);

        j["audioClips"].push_back({{"path", clipPath.getFullPathName().toStdString()}, {"offsetSeconds", offset}});
    }
    return j;
}
