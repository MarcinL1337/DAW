#include "TrackGui.h"
#include "TrackGuiManager.h"

TrackGui::TrackGui(const uint16_t boxWidth, const int numOfSeconds, juce::ValueTree& parentTree) :
    tree{parentTree}, currentBoxWidth{boxWidth}, currentNumOfSeconds{numOfSeconds}
{
    initPopUpMenu();
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

void TrackGui::makeNewWaveformFromAudioFilePath(const juce::String& newAudioFilePath, const NodeID newAudioClipID)
{
    auto waveform = std::make_unique<Waveform>(newAudioFilePath, currentBoxWidth, tree, newAudioClipID);
    waveforms.emplace_back(std::move(waveform));
    addAndMakeVisible(waveforms.back().get());
    resized();
}

void TrackGui::addNewAudioFile(const juce::String& newAudioFilePath, const NodeID newAudioClipID)
{
    makeNewWaveformFromAudioFilePath(newAudioFilePath, newAudioClipID);
}

void TrackGui::mouseDown(const juce::MouseEvent& event)
{
    if(event.mods.isRightButtonDown() and reallyContains(event.mouseDownPosition, true))
    {
        for(auto& waveform: waveforms)
        {
            if(waveform->getBoundsInParent().contains(event.mouseDownPosition.x, event.mouseDownPosition.y))
            {
                // handle specific audio clip removal
            }
        }
        showPopUpMenu();
    }
}

void TrackGui::initPopUpMenu()
{
    menu.addItem(deleteTrack, "Delete track");
    menu.addItem(deleteAudioClip, "Delete audio clip");
    menu.addItem(duplicateTrack, "Duplicate the track");
    menu.addItem(3, "Do an epic dab");
}

void TrackGui::showPopUpMenu()
{
    menu.showMenuAsync(juce::PopupMenu::Options(),
                       [this](const int option)
                       {
                           switch(option)
                           {
                               case noOptionChosen:
                                   break;
                               case deleteTrack:
                                   deleteTrackFromGui();
                                   break;
                               case deleteAudioClip:
                                   break;
                               case 4:
                                   std::cerr << "epic dab done" << std::endl;
                                   break;
                               default:
                                   std::unreachable();
                           }
                       });
}

void TrackGui::setOffsetOfWaveformInSeconds(const NodeID audioClipID, const double offsetSeconds) const
{
    for(const auto& waveform: waveforms)
        if(waveform->getAudioClipID() == audioClipID)
        {
            waveform->setOffsetSeconds(offsetSeconds);
            break;
        }
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

void TrackGui::deleteTrackFromGui() const
{
    const auto trackPlayer = findParentComponentOfClass<TrackGuiManager>();
    auto index{0};
    for(auto& trackGui: trackPlayer->trackGuiVector)
    {
        if(this == trackGui.get())
        {
            tree.setProperty(deleteTrackGui, index, nullptr);
            tree.setProperty(deleteTrackGui, ValueTreeConstants::doNothing, nullptr);
        }
        index++;
    }
}
