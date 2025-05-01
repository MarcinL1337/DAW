#include "Waveform.h"

Waveform::Waveform(const uint16_t boxWidth, juce::ValueTree& parentTree, const NodeID newAudioClipID) :
    audioThumbnailCache(5),
    audioThumbnail(512, formatManager, audioThumbnailCache),
    tree{parentTree},
    currentTrackGuiBoxWidth{boxWidth},
    audioClipID{newAudioClipID}
{
    audioThumbnail.addChangeListener(this);
}

Waveform::Waveform(const juce::String& newAudioFilePath, const uint16_t boxWidth, juce::ValueTree& parentTree,
                   const NodeID newAudioClipID) :
    Waveform(boxWidth, parentTree, newAudioClipID)
{
    const juce::File newAudioFile(newAudioFilePath);
    formatManager.registerBasicFormats();
    formatReader = formatManager.createReaderFor(newAudioFile);
    audioThumbnail.setSource(new juce::FileInputSource(newAudioFile));
}

void Waveform::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == &audioThumbnail)
        repaint();
}

void Waveform::paint(juce::Graphics& g)
{
    const auto waveformLengthInPixels{audioThumbnail.getTotalLength() * currentTrackGuiBoxWidth};
    if(waveformLengthInPixels > getWidth())
    {
        tree.setProperty(numOfSecondsChanged, std::ceil(audioThumbnail.getTotalLength()), nullptr);
    }
    setSize(std::ceil(waveformLengthInPixels), getHeight());
    g.setColour(juce::Colours::violet);

    const float offsetPixels = offsetSeconds * currentTrackGuiBoxWidth;
    g.addTransform(juce::AffineTransform::translation(offsetPixels, 0));

    audioThumbnail.drawChannel(g, getBounds(), 0.0, audioThumbnail.getTotalLength(), 0, 1.0f);
}

void Waveform::resized() {}

void Waveform::changeBoxWidth(const uint16_t newBoxWidth) { currentTrackGuiBoxWidth = newBoxWidth; }

void Waveform::changeBoxHeight(const uint16_t newBoxHeight) { currentTrackGuiBoxHeight = newBoxHeight; }

void Waveform::setOffsetSeconds(const double newOffsetSeconds)
{
    offsetSeconds = newOffsetSeconds;
    // repaint();
}

