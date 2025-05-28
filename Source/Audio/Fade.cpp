#include "Fade.h"
#include <juce_core/juce_core.h>
#include <cmath>

float Fade::getFadeValue(float position, const FadeType type, const bool isFadeIn)
{
    position = juce::jlimit(0.0f, 1.0f, position);

    if(!isFadeIn)
        position = 1.0f - position;

    switch(type)
    {
        case FadeType::Linear:
            return position;

        case FadeType::Logarithmic:
            return std::log10(1.0f + position * 9.0f);

        case FadeType::Exponential:
            return std::pow(position, 2.0f);

        case FadeType::SCurve:
            return (std::sin((position - 0.5f) * juce::MathConstants<float>::pi) + 1.0f) * 0.5f;

        default:
            return position;
    }
}
