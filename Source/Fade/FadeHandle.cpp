#include "FadeHandle.h"

FadeHandle::FadeHandle(const bool isFadeIn) : isInType(isFadeIn) {}

void FadeHandle::setLength(const float lengthSeconds, const float maxLength)
{
    data.lengthSeconds = juce::jlimit(0.0f, maxLength, lengthSeconds);
    data.enabled = data.lengthSeconds > 0.0f;
}

void FadeHandle::setEnabled(const bool enabled)
{
    data.enabled = enabled;
    if(!enabled)
        data.lengthSeconds = 0.0f;
}

juce::Point<int> FadeHandle::getHandlePosition(const ComponentDimensions& dims) const
{
    return dims.getHandlePosition(isInType, data.lengthSeconds);
}

bool FadeHandle::isMouseOverHandle(const juce::Point<int>& mousePos, const ComponentDimensions& dims) const
{
    const auto handlePosition = getHandlePosition(dims);
    return mousePos.getDistanceFrom(handlePosition) < FadeConstants::mouseInteractionDistance;
}

void FadeHandle::rebuildPath(const ComponentDimensions& dims)
{
    path = Fade::buildFadePath(data, isInType, dims.width, dims.height, dims.boxWidth);
}

void FadeHandle::drawHandle(juce::Graphics& g, const ComponentDimensions& dims) const
{
    if(!data.enabled)
        return;

    const auto handlePos = getHandlePosition(dims);
    const auto bounds = juce::Rectangle<int>(handlePos.x - FadeConstants::handleSize / 2,
                                             handlePos.y - FadeConstants::handleSize / 2,
                                             FadeConstants::handleSize,
                                             FadeConstants::handleSize);

    g.setColour(isMouseOver ? FadeConstants::handleMouseOverColour : FadeConstants::handleNormalColour);
    g.fillRect(bounds);
    g.setColour(juce::Colours::black);
    g.drawRect(bounds, FadeConstants::handleBorderThickness);
}

void FadeHandle::drawPath(juce::Graphics& g) const
{
    if(!data.enabled || path.isEmpty())
        return;

    const juce::Colour colour = (isInType ? FadeConstants::fadeInBaseColour : FadeConstants::fadeOutBaseColour)
                                    .withAlpha(FadeConstants::fadePathAlpha);
    g.setColour(colour);
    g.fillPath(path);
}