#include "Waveform.h"

juce::AudioFormatManager Waveform::formatManager;
std::unique_ptr<juce::AudioThumbnailCache> Waveform::audioThumbnailCache;

Waveform::Waveform(const uint16_t boxWidth, juce::ValueTree& parentTree, const NodeID newAudioClipID) :
    tree{parentTree}, currentTrackGuiBoxWidth{boxWidth}, audioClipID{newAudioClipID}
{
    initStaticData();
    audioThumbnail = std::make_unique<juce::AudioThumbnail>(64, formatManager, *audioThumbnailCache);
    audioThumbnail->addChangeListener(this);
    setInterceptsMouseClicks(false, true);
    fadeController = std::make_unique<FadeController>(tree, audioClipID);
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
    fadeController->updateForNewAudioLength(static_cast<float>(audioThumbnail->getTotalLength()));
    addAndMakeVisible(fadeController.get());
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

    drawWaveformWithFade(g, getLocalBounds());
}

void Waveform::drawWaveformWithFade(juce::Graphics& g, const juce::Rectangle<int>& bounds)
{
    const auto totalLength = audioThumbnail->getTotalLength();

    g.setColour(juce::Colour(10, 190, 150).withAlpha(0.9f));

    if(!fadeController->hasFade())
    {
        audioThumbnail->drawChannel(g, bounds, 0.0, totalLength, 0, 1.0f);
        return;
    }

    constexpr int segmentSize = 16;
    for(int x = 0; x < bounds.getWidth(); x += segmentSize)
    {
        const int segmentWidth = juce::jmin(segmentSize, bounds.getWidth() - x);
        const auto timeStart = (static_cast<double>(x) / bounds.getWidth()) * totalLength;
        const auto timeEnd = (static_cast<double>(x + segmentWidth) / bounds.getWidth()) * totalLength;

        const float fadeMultiplier = fadeController->getFadeMultiplier((timeStart + timeEnd) * 0.5, totalLength);
        const auto segmentBounds = juce::Rectangle(bounds.getX() + x, bounds.getY(), segmentWidth, bounds.getHeight());

        audioThumbnail->drawChannel(g, segmentBounds, timeStart, timeEnd, 0, fadeMultiplier);
    }
}

void Waveform::resized()
{
    const float offsetPixels = offsetSeconds * currentTrackGuiBoxWidth;
    const auto waveformLengthInPixels{audioThumbnail->getTotalLength() * currentTrackGuiBoxWidth};
    setBounds(offsetPixels, 0, std::ceil(waveformLengthInPixels), getHeight());
    fadeController->setBounds(getLocalBounds());
}

void Waveform::changeBoxWidth(const uint16_t newBoxWidth)
{
    currentTrackGuiBoxWidth = newBoxWidth;
    fadeController->updateForNewBoxWidth(newBoxWidth);
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
