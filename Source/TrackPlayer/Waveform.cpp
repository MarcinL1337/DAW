#include "Waveform.h"

juce::AudioFormatManager Waveform::formatManager;
std::unique_ptr<juce::AudioThumbnailCache> Waveform::audioThumbnailCache;

Waveform::Waveform(const uint16_t boxWidth, juce::ValueTree& parentTree, const NodeID newAudioClipID) :
    tree{parentTree}, currentTrackGuiBoxWidth{boxWidth}, audioClipID{newAudioClipID}
{
    initStaticData();
    audioThumbnail = std::make_unique<juce::AudioThumbnail>(128, formatManager, *audioThumbnailCache);
    audioThumbnail->addChangeListener(this);
    setInterceptsMouseClicks(false, false);
}

Waveform::Waveform(const juce::String& newAudioFilePath, const uint16_t boxWidth, juce::ValueTree& parentTree,
                   const NodeID newAudioClipID) :
    Waveform(boxWidth, parentTree, newAudioClipID)
{
    const juce::File newAudioFile(newAudioFilePath);
    formatManager.registerBasicFormats();
    formatReader = formatManager.createReaderFor(newAudioFile);
    audioThumbnail->setSource(new juce::FileInputSource(newAudioFile));
    setOffsetSeconds(0);
}

void Waveform::initStaticData()
{
    if(not isStaticDataInitialized)
    {
        formatManager.registerBasicFormats();
        audioThumbnailCache = std::make_unique<juce::AudioThumbnailCache>(10);
        isStaticDataInitialized = true;
    }
}

void Waveform::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == audioThumbnail.get())
        repaint();
}

void Waveform::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(30, 30, 30).withAlpha(0.75f));
    g.fillRect(getLocalBounds());

    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRect(getLocalBounds());

    g.setColour(juce::Colour(10, 190, 150).withAlpha(0.9f));
    audioThumbnail->drawChannel(g, getLocalBounds(), 0.0, audioThumbnail->getTotalLength(), 0, 1.0f);
}

void Waveform::resized()
{
    const float offsetPixels = offsetSeconds * currentTrackGuiBoxWidth;
    const auto waveformLengthInPixels{audioThumbnail->getTotalLength() * currentTrackGuiBoxWidth};
    setBounds(offsetPixels, 0, std::ceil(waveformLengthInPixels), getHeight());
}

void Waveform::changeBoxWidth(const uint16_t newBoxWidth)
{
    currentTrackGuiBoxWidth = newBoxWidth;
    resized();
}

void Waveform::changeBoxHeight(const uint16_t newBoxHeight) { currentTrackGuiBoxHeight = newBoxHeight; }

void Waveform::setOffsetSeconds(const double newOffsetSeconds)
{
    offsetSeconds = newOffsetSeconds;
    const float offsetPixels = offsetSeconds * currentTrackGuiBoxWidth;
    const auto waveformLengthInPixels{audioThumbnail->getTotalLength() * currentTrackGuiBoxWidth};
    if(waveformLengthInPixels + offsetPixels > getWidth())
    {
        tree.setProperty(
            ValueTreeIDs::numOfSecondsChanged, std::ceil(audioThumbnail->getTotalLength()) + offsetSeconds, nullptr);
    }
    resized();
}

