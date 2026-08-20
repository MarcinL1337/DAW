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
    Data() = default;
    Data(const float lengthSeconds, const Function function) : function{function} { setLengthSeconds(lengthSeconds); }
    static constexpr float minLengthSeconds{0.01};
    Function function{Function::Linear};
    juce::Path Path;

    void setLengthSeconds(const float newLength) { lengthSeconds = juce::jmax(minLengthSeconds, newLength); }
    auto getLengthSeconds() const { return lengthSeconds; }

private:
    float lengthSeconds{minLengthSeconds};
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
    if(timePositionSeconds < fadeInData.getLengthSeconds())
    {
        const float position = static_cast<float>(timePositionSeconds) / fadeInData.getLengthSeconds();
        return getFadeValue(position, fadeInData.function, true);
    }
    if(timePositionSeconds > totalLengthSeconds - fadeOutData.getLengthSeconds())
    {
        const float position =
            static_cast<float>(timePositionSeconds - (totalLengthSeconds - fadeOutData.getLengthSeconds())) /
            fadeOutData.getLengthSeconds();
        return getFadeValue(position, fadeOutData.function, false);
    }
    return 1.0f;
}

inline nlohmann::json fadeDataToJson(const Data& fadeData)
{
    nlohmann::json j;
    j["lengthSeconds"] = fadeData.getLengthSeconds();
    j["function"] = fadeData.function;
    return j;
}

inline Data fadeDataFromJson(const nlohmann::json& j)
{
    Data fadeData;
    fadeData.setLengthSeconds(j.value("lengthSeconds", Data::minLengthSeconds));
    fadeData.function = static_cast<Function>(j.value("function", static_cast<int>(Function::Linear)));
    return fadeData;
}

}  // namespace Fade