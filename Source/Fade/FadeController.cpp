#include "FadeController.h"

FadeController::FadeController(juce::ValueTree& parentTree, NodeID audioClipID) :
    tree(parentTree), audioClipID(audioClipID), mouseHandler(fadeIn, fadeOut)
{
    setInterceptsMouseClicks(true, false);
}

void FadeController::setFadeType(const bool isFadeIn, const FadeType type)
{
    auto& handle = isFadeIn ? fadeIn : fadeOut;
    handle.setType(type);
    repaint();
    notifyAudioProcessor();
}

void FadeController::updateForNewAudioLength(const float audioLengthSeconds)
{
    currentAudioLength = audioLengthSeconds;

    fadeIn.updateForNewAudioLength(audioLengthSeconds);
    fadeOut.updateForNewAudioLength(audioLengthSeconds);

    rebuildPathsIfNeeded();
}

void FadeController::updateForNewBoxWidth(const uint16_t newBoxWidth)
{
    currentBoxWidth = newBoxWidth;
    fadeIn.updateForNewBoxWidth(newBoxWidth);
    fadeOut.updateForNewBoxWidth(newBoxWidth);
    rebuildPathsIfNeeded();
}

FadeType FadeController::getFadeType(const bool isFadeIn) const { return (isFadeIn ? fadeIn : fadeOut).getType(); }

void FadeController::paint(juce::Graphics& g)
{
    rebuildPathsIfNeeded();

    fadeIn.drawPath(g);
    fadeOut.drawPath(g);
    fadeIn.drawHandle(g, getWidth(), currentBoxWidth, getHeight());
    fadeOut.drawHandle(g, getWidth(), currentBoxWidth, getHeight());
}

void FadeController::resized() { rebuildPathsIfNeeded(); }

void FadeController::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isRightButtonDown())
    {
        if(fadeIn.isMouseOverHandle(event.getPosition(), getWidth(), currentBoxWidth, getHeight()))
            showTypeMenu(true);
        else if(fadeOut.isMouseOverHandle(event.getPosition(), getWidth(), currentBoxWidth, getHeight()))
            showTypeMenu(false);
    }
    else
    {
        mouseHandler.handleMouseDown(event, getWidth(), currentBoxWidth, getHeight());
    }
}

void FadeController::mouseDrag(const juce::MouseEvent& event)
{
    if(mouseHandler.handleMouseDrag(event, currentBoxWidth, currentAudioLength))
    {
        rebuildPathsIfNeeded();
        repaint();
    }
}

void FadeController::mouseUp(const juce::MouseEvent& event)
{
    if(mouseHandler.handleMouseUp(event))
        notifyAudioProcessor();
}

void FadeController::mouseMove(const juce::MouseEvent& event)
{
    if(mouseHandler.handleMouseMove(event, getWidth(), currentBoxWidth, getHeight()))
    {
        rebuildPathsIfNeeded();
        repaint();
    }
    updateMouseCursor();
}

void FadeController::showTypeMenu(bool isFadeIn)
{
    juce::PopupMenu menu = createTypeMenu(isFadeIn ? fadeIn.getType() : fadeOut.getType());

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, isFadeIn](const int result) { handleTypeMenuResult(result, isFadeIn, this); });
}

void FadeController::notifyAudioProcessor()
{
    juce::var fadeDataVar;
    auto* fadeDataObj = new juce::DynamicObject();

    fadeDataObj->setProperty("audioClipID", static_cast<int>(audioClipID.uid));
    fadeDataObj->setProperty("fadeInEnabled", fadeIn.isEnabled());
    fadeDataObj->setProperty("fadeInLength", fadeIn.getLength());
    fadeDataObj->setProperty("fadeInType", static_cast<int>(fadeIn.getType()));
    fadeDataObj->setProperty("fadeOutEnabled", fadeOut.isEnabled());
    fadeDataObj->setProperty("fadeOutLength", fadeOut.getLength());
    fadeDataObj->setProperty("fadeOutType", static_cast<int>(fadeOut.getType()));

    tree.setProperty("updateFadeData", juce::var(fadeDataObj), nullptr);
}

void FadeController::updateMouseCursor() { setMouseCursor(mouseHandler.getAppropriateMouseCursor()); }

void FadeController::rebuildPathsIfNeeded()
{
    fadeIn.rebuildPath(getWidth(), getHeight(), currentBoxWidth);
    fadeOut.rebuildPath(getWidth(), getHeight(), currentBoxWidth);
}

juce::PopupMenu FadeController::createTypeMenu(FadeType currentType)
{
    juce::PopupMenu menu;
    const auto typeNames = Fade::getFadeTypeNames();

    for(int i = 0; i < typeNames.size(); ++i)
        menu.addItem(i + 1, typeNames[i], true, Fade::getFadeTypeFromIndex(i) == currentType);

    return menu;
}

void FadeController::handleTypeMenuResult(const int result, const bool isFadeIn, FadeController* controller)
{
    if(result > 0 && controller)
        controller->setFadeType(isFadeIn, Fade::getFadeTypeFromIndex(result - 1));
}

bool FadeController::hitTest(const int x, const int y)
{
    if(const juce::Point point(x, y); fadeIn.isMouseOverHandle(point, getWidth(), currentBoxWidth, getHeight()) ||
                                      fadeOut.isMouseOverHandle(point, getWidth(), currentBoxWidth, getHeight()))
    {
        return true;
    }
    return false;
}