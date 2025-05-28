#include "FadeHandle.h"

FadeHandle::FadeHandle(bool isFadeIn) : isInType(isFadeIn) {}

void FadeHandle::setLength(float lengthSeconds, float maxLength)
{
    data.lengthSeconds = juce::jlimit(0.0f, maxLength, lengthSeconds);
    data.enabled = data.lengthSeconds > 0.0f;
    pathNeedsRebuild = true;
}

void FadeHandle::setType(FadeType type)
{
    data.type = type;
    pathNeedsRebuild = true;
}

void FadeHandle::setEnabled(bool enabled)
{
    data.enabled = enabled;
    if(!enabled)
        data.lengthSeconds = 0.0f;
    pathNeedsRebuild = true;
}

void FadeHandle::setMouseOver(bool over)
{
    isMouseOver = over;
    pathNeedsRebuild = true;
}

void FadeHandle::setDragging(bool dragging) { isDragging = dragging; }

int FadeHandle::getHandleX(int componentWidth, uint16_t boxWidth) const
{
    return isInType ? static_cast<int>(data.lengthSeconds * boxWidth)
                    : componentWidth - static_cast<int>(data.lengthSeconds * boxWidth);
}

bool FadeHandle::isMouseOverHandle(const juce::Point<int>& mousePos, int componentWidth, uint16_t boxWidth,
                                   int componentHeight) const
{
    const int handleX = getHandleX(componentWidth, boxWidth);
    const int centerY = componentHeight / 2;
    return mousePos.getDistanceFrom(juce::Point<int>(handleX, centerY)) < 10;
}

void FadeHandle::rebuildPath(int componentWidth, int componentHeight, uint16_t boxWidth)
{
    if(!pathNeedsRebuild)
        return;

    rebuildPathInternal(componentWidth, componentHeight, boxWidth);
    pathNeedsRebuild = false;
}

void FadeHandle::updateForNewAudioLength(float audioLengthSeconds)
{
    if(data.lengthSeconds > audioLengthSeconds)
    {
        data.lengthSeconds = audioLengthSeconds;
        pathNeedsRebuild = true;
    }
}

void FadeHandle::updateForNewBoxWidth(uint16_t newBoxWidth) { pathNeedsRebuild = true; }

void FadeHandle::rebuildPathInternal(int componentWidth, int componentHeight, uint16_t boxWidth)
{
    path = Fade::buildFadePath(data, isInType, componentWidth, componentHeight, boxWidth);
}
