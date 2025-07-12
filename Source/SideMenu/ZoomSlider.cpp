#include "ZoomSlider.h"

ZoomSlider::ZoomSlider()
{
    setSliderStyle(LinearHorizontal);
    setTextBoxStyle(TextBoxBelow, true, 100, 20);
    setTextValueSuffix("%");
    setRange(25, 500);
    setNumDecimalPlacesToDisplay(0);
    setValue(100);

    addAndMakeVisible(zoomSliderLabel);
    zoomSliderLabel.setText("Zoom level", juce::dontSendNotification);
    zoomSliderLabel.attachToComponent(this, false);
    zoomSliderLabel.setJustificationType(juce::Justification::centred);
}

double ZoomSlider::snapValue(const double attemptedValue, DragMode dragMode)
{
    return juce::roundToIntAccurate(attemptedValue / 25.0f) * 25.0f;
}
