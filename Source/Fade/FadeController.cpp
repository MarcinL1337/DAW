#include "FadeController.h"

FadeController::FadeController(juce::ValueTree& parentTree, NodeID audioClipID) :
    tree(parentTree), audioClipID(audioClipID)
{
    setInterceptsMouseClicks(true, false);
}

void FadeController::setFadeType(const bool isFadeIn, const FadeType type)
{
    getHandle(isFadeIn).data.type = type;
    repaint();
    notifyAudioProcessor();
}

void FadeController::updateForNewAudioLength(const float audioLengthSeconds)
{
    currentAudioLength = audioLengthSeconds;
    rebuildPathsIfNeeded();
}

void FadeController::updateForNewBoxWidth(const uint16_t newBoxWidth)
{
    currentBoxWidth = newBoxWidth;
    rebuildPathsIfNeeded();
}

FadeType FadeController::getFadeType(const bool isFadeIn) const { return getHandle(isFadeIn).data.type; }

void FadeController::paint(juce::Graphics& g)
{
    rebuildPathsIfNeeded();
    fadeIn.drawPath(g);
    fadeOut.drawPath(g);

    const ComponentDimensions dims{getWidth(), getHeight(), currentBoxWidth};
    fadeIn.drawHandle(g, dims);
    fadeOut.drawHandle(g, dims);
}

void FadeController::resized() { rebuildPathsIfNeeded(); }

void FadeController::mouseDown(const juce::MouseEvent& event)
{
    const ComponentDimensions dims{getWidth(), getHeight(), currentBoxWidth};
    const auto pos = event.getPosition();

    auto checkHandle = [&](FadeHandle& handle, const bool isFadeIn) -> bool
    {
        if(!handle.isMouseOverHandle(pos, dims))
            return false;

        if(event.mods.isRightButtonDown())
        {
            showTypeMenu(isFadeIn);
        }
        else if(event.mods.isLeftButtonDown())
        {
            handle.isDragging = true;
        }
        return true;
    };

    checkHandle(fadeIn, true) || checkHandle(fadeOut, false);
}

void FadeController::mouseDrag(const juce::MouseEvent& event)
{
    auto* handle = getActiveHandle();
    if(!handle || event.x < 0 || event.x > getWidth())
        return;

    const float newLength = handle->isInType ? static_cast<float>(event.x) / currentBoxWidth
                                             : static_cast<float>(event.getEventRelativeTo(this).x - getWidth()) /
                                                   (-static_cast<float>(currentBoxWidth));

    handle->setLength(newLength, currentAudioLength);
    rebuildPathsIfNeeded();
    repaint();
}

void FadeController::mouseUp(const juce::MouseEvent& event)
{
    if(getActiveHandle())
    {
        fadeIn.isDragging = false;
        fadeOut.isDragging = false;
        notifyAudioProcessor();
    }
}

void FadeController::mouseMove(const juce::MouseEvent& event)
{
    if(updateMouseOverStates(event.getPosition()))
    {
        rebuildPathsIfNeeded();
        repaint();
    }
    updateMouseCursor();
}

void FadeController::showTypeMenu(bool isFadeIn)
{
    juce::PopupMenu menu;
    const auto typeNames = Fade::getFadeTypeNames();
    const auto currentType = getHandle(isFadeIn).data.type;

    for(int i = 0; i < typeNames.size(); ++i)
        menu.addItem(i + 1, typeNames[i], true, Fade::getFadeTypeFromIndex(i) == currentType);

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, isFadeIn](const int result)
                       {
                           if(result > 0)
                               setFadeType(isFadeIn, Fade::getFadeTypeFromIndex(result - 1));
                       });
}

void FadeController::notifyAudioProcessor()
{
    auto* fadeDataObj = new juce::DynamicObject();

    fadeDataObj->setProperty("audioClipID", static_cast<int>(audioClipID.uid));
    fadeDataObj->setProperty("fadeInEnabled", fadeIn.data.enabled);
    fadeDataObj->setProperty("fadeInLength", fadeIn.data.lengthSeconds);
    fadeDataObj->setProperty("fadeInType", static_cast<int>(fadeIn.data.type));
    fadeDataObj->setProperty("fadeOutEnabled", fadeOut.data.enabled);
    fadeDataObj->setProperty("fadeOutLength", fadeOut.data.lengthSeconds);
    fadeDataObj->setProperty("fadeOutType", static_cast<int>(fadeOut.data.type));

    tree.setProperty("updateFadeData", juce::var(fadeDataObj), nullptr);
}

void FadeController::updateMouseCursor()
{
    setMouseCursor((fadeIn.isMouseOver || fadeOut.isMouseOver) ? juce::MouseCursor::LeftRightResizeCursor
                                                               : juce::MouseCursor::NormalCursor);
}

void FadeController::rebuildPathsIfNeeded()
{
    const ComponentDimensions dims{
        static_cast<int>(currentAudioLength * currentBoxWidth), getHeight(), currentBoxWidth};
    fadeIn.rebuildPath(dims);
    fadeOut.rebuildPath(dims);
}

bool FadeController::hitTest(const int x, const int y)
{
    const ComponentDimensions dims{getWidth(), getHeight(), currentBoxWidth};
    const juce::Point point(x, y);
    return fadeIn.isMouseOverHandle(point, dims) || fadeOut.isMouseOverHandle(point, dims);
}

FadeHandle* FadeController::getActiveHandle()
{
    return fadeIn.isDragging ? &fadeIn : (fadeOut.isDragging ? &fadeOut : nullptr);
}

bool FadeController::updateMouseOverStates(const juce::Point<int>& mousePos)
{
    const ComponentDimensions dims{getWidth(), getHeight(), currentBoxWidth};
    const bool wasOverAny = fadeIn.isMouseOver || fadeOut.isMouseOver;
    fadeIn.isMouseOver = fadeIn.isMouseOverHandle(mousePos, dims);
    fadeOut.isMouseOver = fadeOut.isMouseOverHandle(mousePos, dims);
    return wasOverAny != (fadeIn.isMouseOver || fadeOut.isMouseOver);
}