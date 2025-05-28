#include "FadeMouseHandler.h"

FadeMouseHandler::FadeMouseHandler(FadeHandle& fadeIn, FadeHandle& fadeOut) : fadeIn(fadeIn), fadeOut(fadeOut) {}

bool FadeMouseHandler::handleMouseDown(const juce::MouseEvent& event, int componentWidth, uint16_t boxWidth,
                                       int componentHeight)
{
    const auto pos = event.getPosition();

    if(event.mods.isLeftButtonDown())
    {
        if(fadeIn.isMouseOverHandle(pos, componentWidth, boxWidth, componentHeight))
        {
            fadeIn.setDragging(true);
            return true;
        }
        else if(fadeOut.isMouseOverHandle(pos, componentWidth, boxWidth, componentHeight))
        {
            fadeOut.setDragging(true);
            return true;
        }
    }

    return false;
}

bool FadeMouseHandler::handleMouseDrag(const juce::MouseEvent& event, uint16_t boxWidth, float currentAudioLength)
{
    if(!hasActiveHandle())
        return false;

    auto& handle = *getActiveHandle();

    if(event.x < 0 || event.x > event.originalComponent->getWidth())
        return false;

    const float newLength = handle.isFadeIn() ? static_cast<float>(event.x) / boxWidth
                                              : static_cast<float>(event.getEventRelativeTo(event.originalComponent).x -
                                                                   event.originalComponent->getWidth()) /
                                                    (-static_cast<float>(boxWidth));

    updateFadeLength(handle, newLength, currentAudioLength);
    return true;
}

bool FadeMouseHandler::handleMouseUp(const juce::MouseEvent& event)
{
    if(!hasActiveHandle())
        return false;

    fadeIn.setDragging(false);
    fadeOut.setDragging(false);
    return true;
}

bool FadeMouseHandler::handleMouseMove(const juce::MouseEvent& event, int componentWidth, uint16_t boxWidth,
                                       int componentHeight)
{
    return updateMouseOverStates(event.getPosition(), componentWidth, boxWidth, componentHeight);
}

FadeHandle* FadeMouseHandler::getActiveHandle()
{
    if(fadeIn.getDragging())
        return &fadeIn;
    if(fadeOut.getDragging())
        return &fadeOut;
    return nullptr;
}

const FadeHandle* FadeMouseHandler::getActiveHandle() const
{
    if(fadeIn.getDragging())
        return &fadeIn;
    if(fadeOut.getDragging())
        return &fadeOut;
    return nullptr;
}

bool FadeMouseHandler::hasActiveHandle() const { return fadeIn.getDragging() || fadeOut.getDragging(); }

juce::MouseCursor FadeMouseHandler::getAppropriateMouseCursor() const
{
    return (fadeIn.getMouseOver() || fadeOut.getMouseOver()) ? juce::MouseCursor::LeftRightResizeCursor
                                                             : juce::MouseCursor::NormalCursor;
}

void FadeMouseHandler::reset()
{
    fadeIn.setDragging(false);
    fadeOut.setDragging(false);
    fadeIn.setMouseOver(false);
    fadeOut.setMouseOver(false);
}

bool FadeMouseHandler::updateMouseOverStates(const juce::Point<int>& mousePos, int componentWidth, uint16_t boxWidth,
                                             int componentHeight)
{
    const bool wasOverAny = fadeIn.getMouseOver() || fadeOut.getMouseOver();

    fadeIn.setMouseOver(fadeIn.isMouseOverHandle(mousePos, componentWidth, boxWidth, componentHeight));
    fadeOut.setMouseOver(fadeOut.isMouseOverHandle(mousePos, componentWidth, boxWidth, componentHeight));

    return wasOverAny != (fadeIn.getMouseOver() || fadeOut.getMouseOver());
}

void FadeMouseHandler::updateFadeLength(FadeHandle& handle, float newLength, float maxLength)
{
    handle.setLength(newLength, maxLength);
}
