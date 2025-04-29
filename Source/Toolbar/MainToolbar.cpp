#include "MainToolbar.h"

MainToolbar::MainToolbar(const juce::ValueTree& parentTree) : tree{parentTree}, toolbarFactory(tree)
{
    addAndMakeVisible(toolbar);
    initTimeBarValueLabel();
    toolbar.addDefaultItems(toolbarFactory);
    tree.addListener(this);
}

void MainToolbar::resized()
{
    toolbar.setBounds(getLocalBounds());

    timeBarValueArea = getLocalBounds().reduced(0.47f * getWidth(), 2).withX(0.94f * getWidth());
    timeBarValueLabel.setBounds(timeBarValueArea);
}

void MainToolbar::paint(juce::Graphics& g)
{
    g.setColour(juce::Colours::black);
    g.drawRect(getLocalBounds());

    paintTimeBarValue(g);
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

void MainToolbar::initTimeBarValueLabel()
{
    addAndMakeVisible(timeBarValueLabel);
    const juce::FontOptions fontOptions{15.0f, juce::Font::bold};
    timeBarValueLabel.setFont(fontOptions);
    timeBarValueLabel.setJustificationType(juce::Justification::centred);
}

void MainToolbar::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
{
    if(static_cast<int>(tree[property.toString()]) == ValueTreeConstants::doNothing)
        return;
    if(property.toString() == "timeBarTime")
    {
        timeBarValue = tree["timeBarTime"];
        repaint();
    }
}