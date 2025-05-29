#include "FadeHandle.h"

FadeHandle::FadeHandle(const bool isFadeIn) : isInType(isFadeIn) {}

void FadeHandle::setLength(const float lengthSeconds, const float maxLength)
{
    data.lengthSeconds = juce::jlimit(0.0f, maxLength, lengthSeconds);
    data.enabled = data.lengthSeconds > 0.0f;
    pathNeedsRebuild = true;
}

void FadeHandle::setType(const FadeType type)
{
    data.type = type;
    pathNeedsRebuild = true;
}

void FadeHandle::setEnabled(const bool enabled)
{
    data.enabled = enabled;
    if(!enabled)
        data.lengthSeconds = 0.0f;
    pathNeedsRebuild = true;
}

void FadeHandle::setMouseOver(const bool over)
{
    isMouseOver = over;
    pathNeedsRebuild = true;
}

void FadeHandle::setDragging(const bool dragging) { isDragging = dragging; }

int FadeHandle::getHandleX(const int componentWidth, const uint16_t boxWidth) const
{
    return isInType ? static_cast<int>(data.lengthSeconds * boxWidth)
                    : componentWidth - static_cast<int>(data.lengthSeconds * boxWidth);
}

bool FadeHandle::isMouseOverHandle(const juce::Point<int>& mousePos, const int componentWidth, const uint16_t boxWidth,
                                   const int componentHeight) const
{
    const int handleX = getHandleX(componentWidth, boxWidth);
    const int centerY = componentHeight / 2;
    return mousePos.getDistanceFrom(juce::Point(handleX, centerY)) < 10;
}

void FadeHandle::rebuildPath(const int componentWidth, const int componentHeight, const uint16_t boxWidth)
{
    if(!pathNeedsRebuild)
        return;

    rebuildPathInternal(componentWidth, componentHeight, boxWidth);
    pathNeedsRebuild = false;
}

void FadeHandle::updateForNewAudioLength(const float audioLengthSeconds)
{
    if(data.lengthSeconds > audioLengthSeconds)
    {
        data.lengthSeconds = audioLengthSeconds;
        pathNeedsRebuild = true;
    }
}

void FadeHandle::updateForNewBoxWidth(uint16_t newBoxWidth) { pathNeedsRebuild = true; }

void FadeHandle::rebuildPathInternal(const int componentWidth, const int componentHeight, const uint16_t boxWidth)
{
    path = Fade::buildFadePath(data, isInType, componentWidth, componentHeight, boxWidth);
}

void FadeHandle::drawHandle(juce::Graphics& g, const int componentWidth, const uint16_t boxWidth,
                            const int componentHeight) const
{
    if(!data.enabled)
        return;

    const int handleX = getHandleX(componentWidth, boxWidth);
    const int centerY = componentHeight / 2;
    const auto bounds = juce::Rectangle(handleX - FadeConstants::handleSize / 2,
                                        centerY - FadeConstants::handleSize / 2,
                                        FadeConstants::handleSize,
                                        FadeConstants::handleSize);

    g.setColour(getHandleColour(isMouseOver));
    g.fillRect(bounds);
    g.setColour(juce::Colours::black);
    g.drawRect(bounds, FadeConstants::handleBorderThickness);
}

void FadeHandle::drawPath(juce::Graphics& g) const
{
    if(!data.enabled || path.isEmpty())
        return;

    const juce::Colour colour = isInType ? getFadeInColour() : getFadeOutColour();
    g.setColour(colour);
    g.fillPath(path);
}

juce::Colour FadeHandle::getHandleColour(const bool isMouseOver)
{
    return isMouseOver ? juce::Colours::white : juce::Colours::lightblue;
}