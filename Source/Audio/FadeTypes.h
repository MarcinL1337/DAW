#pragma once

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

class FadeCalculator
{
public:
    static float getFadeValue(float position, FadeType type, bool isFadeIn);
};
