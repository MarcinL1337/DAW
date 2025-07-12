#pragma once
#include <nlohmann/json.hpp>

namespace Fade
{
enum class Function
{
    Linear = 1,
    Logarithmic,
    Exponential,
    SCurve
};

struct Data
{
    float lengthSeconds = 0.0f;
    Function function = Function::Linear;
    juce::Path Path;
};

inline float getFadeValue(float position, const Function function, const bool isFadeIn)
{
    position = isFadeIn ? position : 1.0f - position;
    switch(function)
    {
        case Function::Linear:
            return position;
        case Function::Logarithmic:
            return std::log10(1.0f + position * 9.0f);
        case Function::Exponential:
            return position * position;
        case Function::SCurve:
            return (std::sin((position - 0.5f) * juce::MathConstants<float>::pi) + 1.0f) * 0.5f;
        default:
            std::unreachable();
    }
}

inline float getFadeMultiplier(const double timePositionSeconds, const double totalLengthSeconds,
                               const Data& fadeInData, const Data& fadeOutData)
{
    if(timePositionSeconds < fadeInData.lengthSeconds)
    {
        const float position = static_cast<float>(timePositionSeconds) / fadeInData.lengthSeconds;
        return getFadeValue(position, fadeInData.function, true);
    }
    if(timePositionSeconds > totalLengthSeconds - fadeOutData.lengthSeconds)
    {
        const float position =
            static_cast<float>(timePositionSeconds - (totalLengthSeconds - fadeOutData.lengthSeconds)) /
            fadeOutData.lengthSeconds;
        return getFadeValue(position, fadeOutData.function, false);
    }
    return 1.0f;
}

inline nlohmann::json fadeDataToJson(const Data& fadeData)
{
    nlohmann::json j;
    j["lengthSeconds"] = fadeData.lengthSeconds;
    j["function"] = fadeData.function;
    return j;
}

inline Data fadeDataFromJson(const nlohmann::json& j)
{
    Data fadeData;
    fadeData.lengthSeconds = j.value("lengthSeconds", 0.0f);
    fadeData.function = static_cast<Function>(j.value("function", static_cast<int>(Function::Linear)));
    return fadeData;
}

}  // namespace Fade