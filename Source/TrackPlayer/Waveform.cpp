#include "Waveform.h"

juce::AudioFormatManager Waveform::formatManager;
std::unique_ptr<juce::AudioThumbnailCache> Waveform::audioThumbnailCache;

Waveform::Waveform(const uint16_t boxWidth, juce::ValueTree& parentTree, const NodeID newAudioClipID) :
    tree{parentTree}, currentTrackGuiBoxWidth{boxWidth}, audioClipID{newAudioClipID}
{
    initStaticData();
    audioThumbnail = std::make_unique<juce::AudioThumbnail>(64, formatManager, *audioThumbnailCache);
    audioThumbnail->addChangeListener(this);
    setInterceptsMouseClicks(true, false);
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
    fadeController->updateForNewBoxWidth(currentTrackGuiBoxWidth);
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

    constexpr int margin{TrackPlayerConstants::startBoxHeight * 3};
    const auto visibleBounds{g.getClipBounds().expanded(margin, margin).getIntersection(getLocalBounds())};
    if(not visibleBounds.isEmpty())
        drawWaveformWithFade(g, visibleBounds);
}

void Waveform::drawWaveformWithFade(juce::Graphics& g, const juce::Rectangle<int>& bounds) const
{
    const auto totalLength{audioThumbnail->getTotalLength()};

    g.setColour(juce::Colour(10, 190, 150).withAlpha(0.9f));

    if(!fadeController->hasFade())
    {
        const auto timeStart{static_cast<double>(bounds.getX()) / getWidth() * totalLength};
        const auto timeEnd{static_cast<double>(bounds.getRight()) / getWidth() * totalLength};
        audioThumbnail->drawChannel(g, bounds, timeStart, timeEnd, 0, 1.0f);
        return;
    }

    constexpr int segmentSize{16};
    for(int x{bounds.getX()}; x < bounds.getRight(); x += segmentSize)
    {
        const int segmentWidth{juce::jmin(segmentSize, bounds.getRight() - x)};
        const auto timeStart{static_cast<double>(x) / getWidth() * totalLength};
        const auto timeEnd{static_cast<double>(x + segmentWidth) / getWidth() * totalLength};
        const float fadeMultiplier{fadeController->getFadeMultiplier((timeStart + timeEnd) * 0.5, totalLength)};
        const auto segmentBounds{juce::Rectangle(x, bounds.getY(), segmentWidth, bounds.getHeight())};
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

void Waveform::mouseDrag(const juce::MouseEvent& event)
{
    if(event.mouseWasDraggedSinceMouseDown())
    {
        if(auto* dragContainer = juce::DragAndDropContainer::findParentDragContainerFor(this))
        {
            juce::var dragDescription;
            dragDescription.append(static_cast<int>(audioClipID.uid));
            dragDescription.append("waveform");
            dragDescription.append(dragClickOffsetX);
            setAlpha(0.5f);
            const juce::Image transparent(juce::Image::ARGB, 1, 1, true);
            dragContainer->startDragging(dragDescription, this, juce::ScaledImage{transparent});
        }
    }
}

void Waveform::mouseDown(const juce::MouseEvent& event)
{
    dragClickOffsetX = event.x;

    if(auto* parent = getParentComponent())
        parent->mouseDown(event.getEventRelativeTo(parent));
}

void Waveform::mouseMove(const juce::MouseEvent& event)
{
    if(tree.hasProperty(ValueTreeIDs::toggleSplitAudioClipMode) and tree[ValueTreeIDs::toggleSplitAudioClipMode])
        if(auto* parent = getParentComponent())
            parent->mouseMove(event.getEventRelativeTo(parent));
}

void Waveform::mouseEnter(const juce::MouseEvent& event)
{
    if(tree.hasProperty(ValueTreeIDs::toggleSplitAudioClipMode) && tree[ValueTreeIDs::toggleSplitAudioClipMode])
        if(auto* parent = getParentComponent())
            parent->mouseEnter(event.getEventRelativeTo(parent));
}

void Waveform::mouseExit(const juce::MouseEvent& event)
{
    if(tree.hasProperty(ValueTreeIDs::toggleSplitAudioClipMode) && tree[ValueTreeIDs::toggleSplitAudioClipMode])
        if(auto* parent = getParentComponent())
            parent->mouseExit(event.getEventRelativeTo(parent));
}

void Waveform::mouseUp(const juce::MouseEvent&) { setAlpha(1.0f); }