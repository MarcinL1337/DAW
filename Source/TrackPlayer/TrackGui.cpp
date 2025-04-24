#include "TrackGui.h"

TrackGui::TrackGui(const uint16_t boxWidth, const int numOfSeconds, const juce::ValueTree& parentTree) :
    tree{parentTree}, currentNumOfSeconds{numOfSeconds}, currentBoxWidth{boxWidth}
{}

TrackGui::TrackGui(const uint16_t boxWidth, const int numOfBoxes, const juce::ValueTree& parentTree,
                   const juce::String& newAudioFilePath) :
    TrackGui(boxWidth, numOfBoxes, parentTree)
{
    makeNewWaveformFromAudioFilePath(newAudioFilePath);
}

void TrackGui::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::lightgrey);

    for(auto i{0u}; i < currentNumOfSeconds; i++)
    {
        // TODO: take this into consideration: https://forum.juce.com/t/best-practice-for-2d-graph-scrolling/13359/5
        g.drawRect(i * getBoxWidthToFloat(), 0.0, getBoxWidthToFloat(), getBoxHeightToFloat(), 0.75);
    }
}

void TrackGui::resized()
{
    for(const auto& waveform: waveforms) { waveform->setBounds(getLocalBounds()); }
}

void TrackGui::makeNewWaveformFromAudioFilePath(const juce::String& newAudioFilePath)
{
    auto waveform = std::make_unique<Waveform>(newAudioFilePath, currentBoxWidth, tree);
    waveforms.emplace_back(std::move(waveform));
    addAndMakeVisible(waveforms.back().get());
    resized();
}

void TrackGui::addNewAudioFile(const juce::String& newAudioFilePath)
{
    makeNewWaveformFromAudioFilePath(newAudioFilePath);
}

void TrackGui::changeBoxWidth(const uint16_t newBoxWidth)
{
    currentBoxWidth = newBoxWidth;
    for(const auto& waveform: waveforms) { waveform->changeBoxWidth(newBoxWidth); }
}

void TrackGui::changeBoxHeight(const uint16_t newBoxHeight)
{
    currentBoxHeight = newBoxHeight;
    for(const auto& waveform: waveforms) { waveform->changeBoxHeight(newBoxHeight); }
}
