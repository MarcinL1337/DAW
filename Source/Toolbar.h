#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class Toolbar : public juce::ToolbarItemFactory
{
public:
    Toolbar();
    ~Toolbar() override;

private:
    void getAllToolbarItemIds (juce::Array <int>& ids) override;
    void getDefaultItemSet (juce::Array <int>& ids) override;
    juce::ToolbarItemComponent* createItem (int itemId) override;

	enum SpecialItemIds
    {
        separatorBarId      = -1,   /**< The item ID for a vertical (or horizontal) separator bar that
                                         can be placed between sets of items to break them into groups. */
        spacerId            = -2,   /**< The item ID for a fixed-width space that can be placed between
                                         items.*/
        flexibleSpacerId    = -3    /**< The item ID for a gap that pushes outwards against the things on
                                         either side of it, filling any available space. */
    };

	enum ToolbarElementsIds
	{
		previous = 1,
		next,
		restart,
		playPause,
		stopRecording,
		startRecording
	};
};

