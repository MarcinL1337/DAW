#include "FadeController.h"

FadeController::FadeController(juce::ValueTree& parentTree, const NodeID audioClipID) :
    tree{parentTree}, audioClipID{audioClipID}
{
    setInterceptsMouseClicks(true, false);
}

void FadeController::updateForNewAudioLength(const float audioLengthSeconds)
{
    this->audioLengthSeconds = audioLengthSeconds;
    rebuildPaths();
}

void FadeController::updateForNewBoxWidth(const uint16_t newBoxWidth)
{
    currentBoxWidth = newBoxWidth;
    rebuildPaths();
}

void FadeController::paint(juce::Graphics& g)
{
    g.setColour(juce::Colour(0xff4A90E2).withAlpha(0.4f));
    g.fillPath(fadeIn.Path);
    g.setColour(juce::Colour(0xff7B68EE).withAlpha(0.4f));
    g.fillPath(fadeOut.Path);

    drawHandle(g, true);
    drawHandle(g, false);
}

void FadeController::resized() { rebuildPaths(); }

void FadeController::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isRightButtonDown())
        showFunctionMenu(isMouseOverHandle(event.getPosition(), true));
    else if(event.mods.isLeftButtonDown())
        draggingHandle = isMouseOverHandle(event.getPosition(), true);
}

void FadeController::mouseDrag(const juce::MouseEvent& event)
{
    if(!draggingHandle.hasValue() || event.x < 0 || event.x > getWidth())
        return;

    const bool isFadeIn = *draggingHandle;
    const float newLengthSeconds = static_cast<float>(isFadeIn ? event.x : getWidth() - event.x) / currentBoxWidth;

    const float otherFadeLength = getFadeData(!isFadeIn).lengthSeconds;
    const float maxAllowedLength = audioLengthSeconds - otherFadeLength - 0.1f;  // min 0.1 sec gap between fades

    getFadeData(isFadeIn).lengthSeconds = juce::jlimit(0.0f, juce::jmax(0.0f, maxAllowedLength), newLengthSeconds);

    rebuildPaths();
    repaint();
}

void FadeController::mouseUp(const juce::MouseEvent&)
{
    if(draggingHandle.hasValue())
    {
        draggingHandle.reset();
        notifyAudioProcessor();
    }
}

void FadeController::mouseMove(const juce::MouseEvent&) { setMouseCursor(juce::MouseCursor::LeftRightResizeCursor); }

void FadeController::showFunctionMenu(bool isFadeIn)
{
    juce::PopupMenu menu;
    const auto currentFunction = getFadeData(isFadeIn).function;

    const juce::StringArray functionNames = {"", "Linear", "Logarithmic", "Exponential", "S-Curve"};
    for(int i = 1; i < functionNames.size(); i++)
        menu.addItem(i, functionNames[i], true, static_cast<Fade::Function>(i) == currentFunction);

    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this, isFadeIn](const int result)
                       {
                           if(result > 0)
                           {
                               getFadeData(isFadeIn).function = static_cast<Fade::Function>(result);
                               rebuildPaths();
                               repaint();
                               notifyAudioProcessor();
                           }
                       });
}

void FadeController::notifyAudioProcessor()
{
    juce::Array<juce::var> fadeInfo;
    fadeInfo.add(static_cast<int>(audioClipID.uid));
    fadeInfo.add(fadeIn.lengthSeconds);
    fadeInfo.add(static_cast<int>(fadeIn.function));
    fadeInfo.add(fadeOut.lengthSeconds);
    fadeInfo.add(static_cast<int>(fadeOut.function));

    tree.setProperty(ValueTreeIDs::audioClipFadeChanged, fadeInfo, nullptr);
    tree.setProperty(ValueTreeIDs::audioClipFadeChanged, ValueTreeConstants::doNothing, nullptr);
}

void FadeController::rebuildPaths()
{
    const int w = getWidth(), h = getHeight();
    constexpr bool isFadeIn = true;
    fadeIn.Path = buildFadePath(isFadeIn == true, w, h);
    fadeOut.Path = buildFadePath(isFadeIn == false, w, h);
}

bool FadeController::hitTest(const int x, const int y)
{
    const juce::Point point(x, y);
    return isMouseOverHandle(point, true) || isMouseOverHandle(point, false);
}

juce::Point<int> FadeController::getHandlePosition(const bool isFadeIn) const
{
    const auto& fadeLengthSeconds = getFadeData(isFadeIn).lengthSeconds;
    const int handleX = isFadeIn ? static_cast<int>(fadeLengthSeconds * currentBoxWidth)
                                 : getWidth() - static_cast<int>(fadeLengthSeconds * currentBoxWidth);
    return {handleX, handleSize / 2};
}

void FadeController::drawHandle(juce::Graphics& g, const bool isFadeIn) const
{
    const auto [x, y] = getHandlePosition(isFadeIn);
    const auto bounds = juce::Rectangle(x - handleSize / 2, y - handleSize / 2, handleSize, handleSize);

    g.setColour(juce::Colours::white);
    g.fillRect(bounds);
    g.setColour(juce::Colours::black);
    g.drawRect(bounds, 1);
}

bool FadeController::isMouseOverHandle(const juce::Point<int>& mousePos, const bool isFadeIn) const
{
    return mousePos.getDistanceFrom(getHandlePosition(isFadeIn)) < mouseInteractionDistance;
}

juce::Path FadeController::buildFadePath(const bool isFadeIn, const int width, const int height)
{
    const Fade::Data& fadeData = getFadeData(isFadeIn);
    juce::Path path;
    const int fadeWidth = juce::jlimit(0, width, static_cast<int>(fadeData.lengthSeconds * currentBoxWidth));

    const int startX = isFadeIn ? 0 : width - fadeWidth;
    path.startNewSubPath(startX, height);

    for(int x = 0; x <= fadeWidth; ++x)
    {
        const float position = static_cast<float>(x) / fadeWidth;
        const float fadeValue = Fade::getFadeValue(position, fadeData.function, isFadeIn);
        path.lineTo(startX + x, height - fadeValue * height);
    }

    path.lineTo(startX + fadeWidth, height);
    path.closeSubPath();
    return path;
}

float FadeController::getFadeMultiplier(const double timePositionSeconds, const double totalLengthSeconds) const
{
    return Fade::getFadeMultiplier(timePositionSeconds, totalLengthSeconds, fadeIn, fadeOut);
}

bool FadeController::hasFade() const { return fadeIn.lengthSeconds > 0.0f || fadeOut.lengthSeconds > 0.0f; }
