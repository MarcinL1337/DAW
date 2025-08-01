#include "MainToolbar.h"

MainToolbar::MainToolbar(juce::ValueTree& parentTree) : tree{parentTree}, toolbarFactory{tree}
{
    addAndMakeVisible(toolbar);
    initTimeBarValueLabel();
    initSplitSecondsLabel();
    toolbar.addDefaultItems(toolbarFactory);
    tree.addListener(this);
}

void MainToolbar::resized()
{
    toolbar.setBounds(getLocalBounds());

    timeBarValueArea = getLocalBounds().reduced(0.47f * getWidth(), 2).withX(0.94f * getWidth());
    timeBarValueLabel.setBounds(timeBarValueArea);

    splitSecondsArea = getLocalBounds().reduced(0.47f * getWidth(), 2).withX(0.85f * getWidth());
    splitSecondsLabel.setBounds(splitSecondsArea);
}

void MainToolbar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());

    paintTimeBarValue(g);

    if(isClipSplitActive)
    {
        paintSplitSecondsValue(g);
    }
}

void MainToolbar::paintTimeBarValue(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(timeBarValueArea, 2);
    oss.str("");
    oss.clear();
    oss << std::fixed << std::setprecision(2) << timeBarValue;
    timeBarValueLabel.setText(oss.str() + "s", juce::dontSendNotification);
}

void MainToolbar::paintSplitSecondsValue(juce::Graphics& g)
{
    g.setColour(juce::Colours::whitesmoke);
    g.drawRect(splitSecondsArea, 2);
    oss.str("");
    oss.clear();
    oss << std::fixed << std::setprecision(2) << splitSeconds;
    splitSecondsLabel.setText(oss.str() + "s", juce::dontSendNotification);
}

void MainToolbar::initTimeBarValueLabel()
{
    addAndMakeVisible(timeBarValueLabel);
    const juce::FontOptions fontOptions{15.0f, juce::Font::bold};
    timeBarValueLabel.setFont(fontOptions);
    timeBarValueLabel.setJustificationType(juce::Justification::centred);
}

void MainToolbar::initSplitSecondsLabel()
{
    const juce::FontOptions fontOptions{15.0f, juce::Font::bold};
    splitSecondsLabel.setFont(fontOptions);
    splitSecondsLabel.setJustificationType(juce::Justification::centred);
}

void MainToolbar::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property]) == ValueTreeConstants::doNothing)
        return;
    if(property == ValueTreeIDs::timeBarTime)
    {
        timeBarValue = tree[ValueTreeIDs::timeBarTime];
        repaint();
    }
    else if(property == ValueTreeIDs::toggleSplitAudioClipMode)
    {
        isClipSplitActive = tree[ValueTreeIDs::toggleSplitAudioClipMode];
        if(isClipSplitActive)
        {
            addAndMakeVisible(&splitSecondsLabel);
        }
        else
        {
            removeChildComponent(&splitSecondsLabel);
            splitSeconds = 0.0f;
        }
    }
    else if(property == ValueTreeIDs::splitSecondsChanged)
    {
        splitSeconds = tree[ValueTreeIDs::splitSecondsChanged];
        repaint();
    }
}