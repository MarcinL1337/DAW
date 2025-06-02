#include "TrackGuiComponent.h"
#include "TrackGuiManager.h"

class TrackGuiManager;
TrackGuiComponent::TrackGuiComponent(const juce::ValueTree& parentTree) : tree{parentTree}
{
    tree.addListener(this);
    // setInterceptsMouseClicks(true, true);
}

void TrackGuiComponent::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::forestgreen);
    timeBarPosition = timeBarTime * currentTrackGuiBoxWidth;
    g.drawLine(timeBarPosition, 0, timeBarPosition, getHeight());
}

void TrackGuiComponent::resized() {}

void TrackGuiComponent::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::timeBarTime)
    {
        timeBarTime = tree[ValueTreeIDs::timeBarTime];
        repaint();
    }
}

void TrackGuiComponent::changeBoxWidth(const uint16_t newBoxWidth) { currentTrackGuiBoxWidth = newBoxWidth; }

// void TrackGuiComponent::mouseDown(const juce::MouseEvent& event)
// {
//     if(event.mods.isRightButtonDown())
//     {
//         std::cerr << "Mouse Right Down" << std::endl;
//         const auto trackPlayer = findParentComponentOfClass<TrackGuiManager>();
//
//         for(const auto& trackGui: trackPlayer->trackGuiVector)
//         {
//             if(trackGui->getBounds().contains(event.mouseDownPosition.toInt()))
//             {
//                 std::cerr << "TrackGui contains mouse right click" << std::endl;
//                 for(const auto& waveform: trackGui->waveforms)
//                 {
//                     if(waveform->getBounds().contains(event.mouseDownPosition.toInt()))
//                     {
//                         std::cerr << "waveform contains mouse right click" << std::endl;
//                     }
//                 }
//             }
//         }
//     }
// }
