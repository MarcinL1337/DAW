#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "Fade.h"

class FadeHandle
{
public:
    explicit FadeHandle(bool isFadeIn);

    void setLength(float lengthSeconds, float maxLength);
    void setType(FadeType type);
    void setEnabled(bool enabled);

    float getLength() const { return data.lengthSeconds; }
    FadeType getType() const { return data.type; }
    bool isEnabled() const { return data.enabled; }
    bool isFadeIn() const { return isInType; }

    void setMouseOver(bool over);
    void setDragging(bool dragging);
    bool getMouseOver() const { return isMouseOver; }
    bool getDragging() const { return isDragging; }

    int getHandleX(int componentWidth, uint16_t boxWidth) const;
    bool isMouseOverHandle(const juce::Point<int>& mousePos, int componentWidth, uint16_t boxWidth,
                           int componentHeight) const;

    const juce::Path& getPath() const { return path; }
    void rebuildPath(int componentWidth, int componentHeight, uint16_t boxWidth);

    void updateForNewAudioLength(float audioLengthSeconds);
    void updateForNewBoxWidth(uint16_t newBoxWidth);

    const FadeData& getData() const { return data; }

private:
    FadeData data;
    juce::Path path;
    bool isMouseOver{false};
    bool isDragging{false};
    bool isInType;
    bool pathNeedsRebuild{true};

    void rebuildPathInternal(int componentWidth, int componentHeight, uint16_t boxWidth);
};
