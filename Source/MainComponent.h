#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "Audio/MainAudio.h"
#include "Constants.h"
#include "ProjectFilesManager.h"
#include "SideMenu/SideMenu.h"
#include "Toolbar/MainToolbar.h"
#include "TopMenu/Menu.h"
#include "TrackManager.h"
#include "TrackPlayer/TrackGuiManager.h"

class MainComponent final : public juce::Component
{
public:
    MainComponent();
    ~MainComponent() override;

private:
    void paint(juce::Graphics&) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;
    void flexBoxInit();

    juce::Identifier treeType{"mainComponentValueTree"};
    juce::ValueTree tree{treeType};

    MainAudio mainAudio;
    Menu topLevelMenu;
    MainToolbar mainToolbar;
    TrackGuiManager trackPlayer;
    SideMenu sideMenu;
    TrackManager trackManager;
    ProjectFilesManager projectFilesManager;

    juce::FlexBox topLevelFlexBox{};
    juce::FlexBox mainContentFlexBox{};
    juce::FlexBox trackPlayerFlexBox{};

    struct TooltipLF final : juce::LookAndFeel_V4
    {
        void drawTooltip(juce::Graphics& g, const juce::String& text, const int width, const int height) override
        {
            g.fillAll(juce::Colour{0xff2e2e2e});
            g.setColour(juce::Colour{0xff2e2e2e});
            g.fillRoundedRectangle(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 4.0f);

            g.setColour(juce::Colour{0xff0abe96}.withAlpha(0.7f));
            g.drawRoundedRectangle(
                0.5f, 0.5f, static_cast<float>(width) - 1.0f, static_cast<float>(height) - 1.0f, 4.0f, 1.0f);

            g.setColour(juce::Colours::whitesmoke);
            g.setFont(juce::FontOptions{13.0f});
            g.drawFittedText(text, 0, 0, width, height, juce::Justification::centred, 1);
        }
    } tooltipLF;
    juce::TooltipWindow tooltipWindow{this, 700};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)

    friend class MainWindow;
};
