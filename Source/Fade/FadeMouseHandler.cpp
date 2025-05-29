#include "FadeMouseHandler.h"

FadeMouseHandler::FadeMouseHandler(FadeHandle& fadeIn, FadeHandle& fadeOut) : fadeIn(fadeIn), fadeOut(fadeOut) {}

bool FadeMouseHandler::handleMouseDown(const juce::MouseEvent& event, const int componentWidth, const uint16_t boxWidth,
                                       const int componentHeight) const
{
    if(event.mods.isLeftButtonDown())
    {
        const auto pos = event.getPosition();
        if(fadeIn.isMouseOverHandle(event.getPosition(), componentWidth, boxWidth, componentHeight))
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

bool FadeMouseHandler::handleMouseDrag(const juce::MouseEvent& event, const uint16_t boxWidth,
                                       const float currentAudioLength)
{
    auto* handle = getActiveHandle();
    if(!handle)
        return false;

    if(event.x < 0 || event.x > event.originalComponent->getWidth())
        return false;

    const float newLength = handle->isFadeIn()
                                ? static_cast<float>(event.x) / boxWidth
                                : static_cast<float>(event.getEventRelativeTo(event.originalComponent).x -
                                                     event.originalComponent->getWidth()) /
                                      (-static_cast<float>(boxWidth));

    handle->setLength(newLength, currentAudioLength);
    return true;
}

bool FadeMouseHandler::handleMouseUp(const juce::MouseEvent& event) const
{
    if(!getActiveHandle())
        return false;
    fadeIn.setDragging(false);
    fadeOut.setDragging(false);
    return true;
}

bool FadeMouseHandler::handleMouseMove(const juce::MouseEvent& event, const int componentWidth, const uint16_t boxWidth,
                                       const int componentHeight) const
{
    return updateMouseOverStates(event.getPosition(), componentWidth, boxWidth, componentHeight);
}

juce::MouseCursor FadeMouseHandler::getAppropriateMouseCursor() const
{
    return (fadeIn.getMouseOver() || fadeOut.getMouseOver()) ? juce::MouseCursor::LeftRightResizeCursor
                                                             : juce::MouseCursor::NormalCursor;
}

void FadeMouseHandler::reset() const
{
    fadeIn.setDragging(false);
    fadeOut.setDragging(false);
    fadeIn.setMouseOver(false);
    fadeOut.setMouseOver(false);
}

bool FadeMouseHandler::updateMouseOverStates(const juce::Point<int>& mousePos, const int componentWidth,
                                             const uint16_t boxWidth, const int componentHeight) const
{
    const bool wasOverAny = fadeIn.getMouseOver() || fadeOut.getMouseOver();
    fadeIn.setMouseOver(fadeIn.isMouseOverHandle(mousePos, componentWidth, boxWidth, componentHeight));
    fadeOut.setMouseOver(fadeOut.isMouseOverHandle(mousePos, componentWidth, boxWidth, componentHeight));
    return wasOverAny != (fadeIn.getMouseOver() || fadeOut.getMouseOver());
}
