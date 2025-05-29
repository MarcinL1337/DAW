#include "Fade.h"

juce::Path Fade::buildFadePath(const FadeData& fadeData, const bool isFadeIn, const int width, const int height,
                               const uint16_t boxWidth)
{
    juce::Path path;

    if(!fadeData.enabled)
        return path;

    const int fadeWidth = static_cast<int>(fadeData.lengthSeconds * boxWidth);
    if(fadeWidth <= 0)
        return path;

    const int startX = isFadeIn ? 0 : width - fadeWidth;

    path.startNewSubPath(startX, height);

    for(int x = 0; x <= fadeWidth; ++x)
    {
        const float position = static_cast<float>(x) / fadeWidth;
        const float fadeValue = getFadeValue(position, fadeData.type, isFadeIn);
        path.lineTo(startX + x, height - fadeValue * height);
    }

    path.lineTo(startX + fadeWidth, height);
    path.closeSubPath();

    return path;
}
