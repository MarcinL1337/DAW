#pragma once
#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "../Constants.h"

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
inline const juce::StringArray& getFadeTypeNames() { return FadeConstants::fadeTypeNames; }

inline FadeType getFadeTypeFromIndex(const int index) { return static_cast<FadeType>(index); }
inline int getIndexFromFadeType(FadeType type) { return static_cast<int>(type); }

float getFadeValue(float position, FadeType type, bool isFadeIn);

juce::Path buildFadePath(const FadeData& fadeData, bool isFadeIn, int width, int height, uint16_t boxWidth);
}  // namespace Fade
