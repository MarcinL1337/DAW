#include "Waveform.h"
#include "../Audio/MainAudio.h"

Waveform::Waveform(const uint16_t boxWidth, juce::ValueTree& parentTree, const NodeID newAudioClipID) :
    audioThumbnailCache(5),
    audioThumbnail(512, formatManager, audioThumbnailCache),
    tree{parentTree},
    currentTrackGuiBoxWidth{boxWidth},
    audioClipID{newAudioClipID}
{
    audioThumbnail.addChangeListener(this);
    setInterceptsMouseClicks(true, false);
}

Waveform::Waveform(const juce::String& newAudioFilePath, const uint16_t boxWidth, juce::ValueTree& parentTree,
                   const NodeID newAudioClipID) :
    Waveform(boxWidth, parentTree, newAudioClipID)
{
    const juce::File newAudioFile(newAudioFilePath);
    formatManager.registerBasicFormats();
    formatReader = formatManager.createReaderFor(newAudioFile);
    audioThumbnail.setSource(new juce::FileInputSource(newAudioFile));
    setOffsetSeconds(0);
}

void Waveform::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    if(source == &audioThumbnail)
        repaint();
}

void Waveform::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(30, 30, 30).withAlpha(0.75f));
    g.fillRect(getLocalBounds());

    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.drawRect(getLocalBounds());

    g.setColour(juce::Colour(10, 190, 150).withAlpha(0.9f));
    audioThumbnail.drawChannel(g, getLocalBounds(), 0.0, audioThumbnail.getTotalLength(), 0, 1.0f);

    drawFadeShape(g);
    drawFadeHandles(g);
}

void Waveform::resized()
{
    const float offsetPixels = offsetSeconds * currentTrackGuiBoxWidth;
    const auto waveformLengthInPixels{audioThumbnail.getTotalLength() * currentTrackGuiBoxWidth};
    setBounds(offsetPixels, 0, std::ceil(waveformLengthInPixels), getHeight());

    updateFadeInAudioProcessor();
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
    const auto waveformLengthInPixels{audioThumbnail.getTotalLength() * currentTrackGuiBoxWidth};
    if(waveformLengthInPixels + offsetPixels > getWidth())
    {
        std::cerr << "Dupa = " << std::ceil(audioThumbnail.getTotalLength()) + offsetSeconds << std::endl;
        tree.setProperty(
            ValueTreeIDs::numOfSecondsChanged, std::ceil(audioThumbnail.getTotalLength()) + offsetSeconds, nullptr);
    }

    updateFadeInAudioProcessor();
}

void Waveform::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isLeftButtonDown())
    {
        if(isMouseOverFadeInHandle(event.getPosition()))
        {
            draggingFadeIn = true;
        }
        else if(isMouseOverFadeOutHandle(event.getPosition()))
        {
            draggingFadeOut = true;
        }
    }
    else if(event.mods.isRightButtonDown())
    {
        if(isMouseOverFadeInHandle(event.getPosition()))
        {
            showFadeTypeMenu(true);
        }
        else if(isMouseOverFadeOutHandle(event.getPosition()))
        {
            showFadeTypeMenu(false);
        }
    }
}

void Waveform::mouseDrag(const juce::MouseEvent& event)
{
    if(draggingFadeIn)
    {
        float newLength = static_cast<float>(event.x) / currentTrackGuiBoxWidth;
        updateFadeAfterDrag(true, newLength);
    }
    else if(draggingFadeOut)
    {
        float newLength = static_cast<float>(getWidth() - event.x) / currentTrackGuiBoxWidth;
        updateFadeAfterDrag(false, newLength);
    }
}

void Waveform::mouseUp(const juce::MouseEvent& event)
{
    draggingFadeIn = false;
    draggingFadeOut = false;
    updateFadeInAudioProcessor();
}

void Waveform::mouseMove(const juce::MouseEvent& event)
{
    bool wasOverFadeIn = mouseOverFadeInHandle;
    bool wasOverFadeOut = mouseOverFadeOutHandle;

    mouseOverFadeInHandle = isMouseOverFadeInHandle(event.getPosition());
    mouseOverFadeOutHandle = isMouseOverFadeOutHandle(event.getPosition());

    if(wasOverFadeIn != mouseOverFadeInHandle || wasOverFadeOut != mouseOverFadeOutHandle)
    {
        repaint();
    }

    if(mouseOverFadeInHandle || mouseOverFadeOutHandle)
    {
        setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    }
    else
    {
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
}

bool Waveform::isMouseOverFadeInHandle(const juce::Point<int>& position) const
{
    int handleX = static_cast<int>(fadeInData.lengthSeconds * currentTrackGuiBoxWidth);
    return position.getDistanceFrom(juce::Point<int>(handleX, getHeight() / 2)) < handleSize;
}

bool Waveform::isMouseOverFadeOutHandle(const juce::Point<int>& position) const
{
    int handleX = getWidth() - static_cast<int>(fadeOutData.lengthSeconds * currentTrackGuiBoxWidth);
    return position.getDistanceFrom(juce::Point<int>(handleX, getHeight() / 2)) < handleSize;
}

void Waveform::updateFadeAfterDrag(bool isFadeIn, float newLength)
{
    const float audioLength = static_cast<float>(audioThumbnail.getTotalLength());

    if(isFadeIn)
    {
        newLength = juce::jlimit(0.0f, audioLength / 2.0f, newLength);
        fadeInData.lengthSeconds = newLength;
        fadeInData.enabled = newLength > 0.0f;
    }
    else
    {
        newLength = juce::jlimit(0.0f, audioLength / 2.0f, newLength);
        fadeOutData.lengthSeconds = newLength;
        fadeOutData.enabled = newLength > 0.0f;
    }

    repaint();
}

void Waveform::drawFadeHandles(juce::Graphics& g)
{
    int fadeInHandleX = static_cast<int>(fadeInData.lengthSeconds * currentTrackGuiBoxWidth);
    int fadeOutHandleX = getWidth() - static_cast<int>(fadeOutData.lengthSeconds * currentTrackGuiBoxWidth);

    // Fade in handle
    if(fadeInData.enabled)
    {
        g.setColour(mouseOverFadeInHandle ? juce::Colours::white : juce::Colours::lightblue);
        g.fillRect(fadeInHandleX - handleSize / 2, getHeight() / 2 - handleSize / 2, handleSize, handleSize);
        g.setColour(juce::Colours::black);
        g.drawRect(fadeInHandleX - handleSize / 2, getHeight() / 2 - handleSize / 2, handleSize, handleSize, 1);
    }

    // Fade out handle
    if(fadeOutData.enabled)
    {
        g.setColour(mouseOverFadeOutHandle ? juce::Colours::white : juce::Colours::lightblue);
        g.fillRect(fadeOutHandleX - handleSize / 2, getHeight() / 2 - handleSize / 2, handleSize, handleSize);
        g.setColour(juce::Colours::black);
        g.drawRect(fadeOutHandleX - handleSize / 2, getHeight() / 2 - handleSize / 2, handleSize, handleSize, 1);
    }
}

void Waveform::drawFadeShape(juce::Graphics& g)
{
    const int height = getHeight();
    const float audioLength = static_cast<float>(audioThumbnail.getTotalLength());
    const int fadeInWidth = static_cast<int>(fadeInData.lengthSeconds * currentTrackGuiBoxWidth);
    const int fadeOutWidth = static_cast<int>(fadeOutData.lengthSeconds * currentTrackGuiBoxWidth);

    if(fadeInData.enabled && fadeInWidth > 0)
    {
        juce::Path fadeInPath;
        fadeInPath.startNewSubPath(0, height);

        for(int x = 0; x <= fadeInWidth; ++x)
        {
            float position = static_cast<float>(x) / fadeInWidth;
            float fadeValue = FadeCalculator::getFadeValue(position, fadeInData.type, true);
            fadeInPath.lineTo(x, height - fadeValue * height);
        }

        fadeInPath.lineTo(fadeInWidth, height);
        fadeInPath.closeSubPath();

        g.setColour(juce::Colours::red.withAlpha(0.4f));
        g.fillPath(fadeInPath);
    }

    if(fadeOutData.enabled && fadeOutWidth > 0)
    {
        const int startX = getWidth() - fadeOutWidth;

        juce::Path fadeOutPath;
        fadeOutPath.startNewSubPath(startX, height);

        for(int x = 0; x <= fadeOutWidth; ++x)
        {
            float position = static_cast<float>(x) / fadeOutWidth;
            float fadeValue = FadeCalculator::getFadeValue(position, fadeOutData.type, false);
            fadeOutPath.lineTo(startX + x, height - fadeValue * height);
        }

        fadeOutPath.lineTo(getWidth(), height);
        fadeOutPath.closeSubPath();

        g.setColour(juce::Colours::blue.withAlpha(0.4f));
        g.fillPath(fadeOutPath);
    }
}

void Waveform::showFadeTypeMenu(bool isFadeIn)
{
    juce::PopupMenu menu;
    FadeType currentType = isFadeIn ? fadeInData.type : fadeOutData.type;

    menu.addItem(1, "Linear", true, currentType == FadeType::Linear);
    menu.addItem(2, "Logarithmic", true, currentType == FadeType::Logarithmic);
    menu.addItem(3, "Exponential", true, currentType == FadeType::Exponential);
    menu.addItem(4, "S-Curve", true, currentType == FadeType::SCurve);

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, isFadeIn](int result)
                       {
                           if(result > 0)
                           {
                               FadeType selectedType = static_cast<FadeType>(result - 1);
                               if(isFadeIn)
                                   setFadeInType(selectedType);
                               else
                                   setFadeOutType(selectedType);

                               updateFadeInAudioProcessor();
                               repaint();
                           }
                       });
}

void Waveform::setFadeInLength(float lengthInSeconds)
{
    fadeInData.lengthSeconds =
        juce::jlimit(0.0f, static_cast<float>(audioThumbnail.getTotalLength()) / 2.0f, lengthInSeconds);
    fadeInData.enabled = fadeInData.lengthSeconds > 0.0f;
    updateFadeInAudioProcessor();
    repaint();
}

void Waveform::setFadeOutLength(float lengthInSeconds)
{
    fadeOutData.lengthSeconds =
        juce::jlimit(0.0f, static_cast<float>(audioThumbnail.getTotalLength()) / 2.0f, lengthInSeconds);
    fadeOutData.enabled = fadeOutData.lengthSeconds > 0.0f;
    updateFadeInAudioProcessor();
    repaint();
}

void Waveform::setFadeInType(FadeType type)
{
    fadeInData.type = type;
    updateFadeInAudioProcessor();
    repaint();
}

void Waveform::setFadeOutType(FadeType type)
{
    fadeOutData.type = type;
    updateFadeInAudioProcessor();
    repaint();
}

void Waveform::updateFadeInAudioProcessor()
{
    auto* mainComponent = findParentComponentOfClass<juce::Component>();
    if(!mainComponent)
        return;

    juce::var fadeDataVar;
    juce::DynamicObject* fadeDataObj = new juce::DynamicObject();

    fadeDataObj->setProperty("audioClipID", static_cast<int>(audioClipID.uid));
    fadeDataObj->setProperty("fadeInEnabled", fadeInData.enabled);
    fadeDataObj->setProperty("fadeInLength", fadeInData.lengthSeconds);
    fadeDataObj->setProperty("fadeInType", static_cast<int>(fadeInData.type));
    fadeDataObj->setProperty("fadeOutEnabled", fadeOutData.enabled);
    fadeDataObj->setProperty("fadeOutLength", fadeOutData.lengthSeconds);
    fadeDataObj->setProperty("fadeOutType", static_cast<int>(fadeOutData.type));

    fadeDataVar = fadeDataObj;
    tree.setProperty("updateFadeData", fadeDataVar, nullptr);
}