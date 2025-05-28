#include "FadeRenderer.h"

void FadeRenderer::drawFadeHandle(juce::Graphics& g, const FadeHandle& handle, int componentWidth, uint16_t boxWidth,
                                  int componentHeight)
{
    if(!handle.isEnabled())
        return;

    drawSingleHandle(g, handle, componentWidth, boxWidth, componentHeight);
}

void FadeRenderer::drawFadePath(juce::Graphics& g, const FadeHandle& handle)
{
    if(!handle.isEnabled() || handle.getPath().isEmpty())
        return;

    juce::Colour colour = handle.isFadeIn() ? getFadeInColour() : getFadeOutColour();
    drawSingleFadePath(g, handle, colour);
}

void FadeRenderer::drawAllFades(juce::Graphics& g, const FadeHandle& fadeIn, const FadeHandle& fadeOut,
                                int componentWidth, uint16_t boxWidth, int componentHeight)
{
    drawFadePath(g, fadeIn);
    drawFadePath(g, fadeOut);
    drawFadeHandle(g, fadeIn, componentWidth, boxWidth, componentHeight);
    drawFadeHandle(g, fadeOut, componentWidth, boxWidth, componentHeight);
}

juce::Colour FadeRenderer::getHandleColour(bool isMouseOver)
{
    return isMouseOver ? juce::Colours::white : juce::Colours::lightblue;
}

void FadeRenderer::drawSingleFadePath(juce::Graphics& g, const FadeHandle& handle, juce::Colour colour)
{
    g.setColour(colour);
    g.fillPath(handle.getPath());
}

void FadeRenderer::drawSingleHandle(juce::Graphics& g, const FadeHandle& handle, int componentWidth, uint16_t boxWidth,
                                    int componentHeight)
{
    const int handleX = handle.getHandleX(componentWidth, boxWidth);
    const int centerY = componentHeight / 2;
    const auto bounds =
        juce::Rectangle<int>(handleX - handleSize / 2, centerY - handleSize / 2, handleSize, handleSize);

    g.setColour(getHandleColour(handle.getMouseOver()));
    g.fillRect(bounds);
    g.setColour(getHandleBorderColour());
    g.drawRect(bounds, handleBorderThickness);
}
