#pragma once

#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

enum class FadeType
{
    Linear,
    Logarithmic,
    Exponential,
    SCurve
};

struct FadeData
{
    bool enabled = false;
    float lengthSeconds = 0.0f;
    FadeType type = FadeType::Linear;
};

namespace Fade
{
constexpr std::array<const char*, 4> FADE_TYPE_NAMES = {"Linear", "Logarithmic", "Exponential", "S-Curve"};

inline juce::StringArray getFadeTypeNames()
{
    return juce::StringArray(FADE_TYPE_NAMES.data(), FADE_TYPE_NAMES.size());
}

inline FadeType getFadeTypeFromIndex(int index) { return static_cast<FadeType>(juce::jlimit(0, 3, index)); }

inline int getIndexFromFadeType(FadeType type) { return static_cast<int>(type); }

inline float getFadeValue(float position, FadeType type, bool isFadeIn)
{
    position = juce::jlimit(0.0f, 1.0f, isFadeIn ? position : 1.0f - position);

    switch(type)
    {
        case FadeType::Logarithmic:
            return std::log10(1.0f + position * 9.0f);
        case FadeType::Exponential:
            return position * position;
        case FadeType::SCurve:
            return (std::sin((position - 0.5f) * juce::MathConstants<float>::pi) + 1.0f) * 0.5f;
        default:
            return position;  // Linear
    }
}
juce::Path buildFadePath(const FadeData& fadeData, bool isFadeIn, int width, int height, uint16_t boxWidth);
}  // namespace Fade
