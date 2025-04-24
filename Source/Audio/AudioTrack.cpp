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

void AudioTrack::setProperty(const AudioClipProperty property, const bool boolValue)
{
    switch(property)
    {
        case AudioClipProperty::MUTE:
            properties.mute = boolValue;
            break;
        case AudioClipProperty::SOLO:
            properties.solo = boolValue;
            break;
        default:
            return;
    }

    for(const auto& clipId: audioClips)
    {
        switch(property)
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
}

void AudioTrack::setProperty(const AudioClipProperty property, const float floatValue)
{
    switch(property)
    {
        case AudioClipProperty::GAIN:
            properties.gain = floatValue;
            break;
        case AudioClipProperty::PAN:
            properties.pan = floatValue;
            break;
        default:
            return;
    }

    for(const auto& clipId: audioClips)
    {
        switch(property)
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
}