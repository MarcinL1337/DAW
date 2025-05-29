#pragma once
#include "Fade.h"

struct ComponentDimensions
{
    int width, height;
    uint16_t boxWidth;

    juce::Point<int> getHandlePosition(const bool isFadeIn, const float lengthSeconds) const
    {
        const int handleX =
            isFadeIn ? static_cast<int>(lengthSeconds * boxWidth) : width - static_cast<int>(lengthSeconds * boxWidth);
        return {handleX, height / 2};
    }
};

class FadeHandle
{
public:
    explicit FadeHandle(bool isFadeIn);

    void setLength(float lengthSeconds, float maxLength);
    void setEnabled(bool enabled);

    juce::Point<int> getHandlePosition(const ComponentDimensions& dims) const;
    bool isMouseOverHandle(const juce::Point<int>& mousePos, const ComponentDimensions& dims) const;
    void rebuildPath(const ComponentDimensions& dims);
    void drawHandle(juce::Graphics& g, const ComponentDimensions& dims) const;
    void drawPath(juce::Graphics& g) const;

    FadeData data;
    juce::Path path;
    bool isMouseOver{false};
    bool isDragging{false};
    const bool isInType;
};