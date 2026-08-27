#include "PluginLibraryPanel.h"

PluginLibraryPanel::PluginLibraryPanel (
    JADLookAndFeel& lookAndFeelToUse)
    : jadLookAndFeel (lookAndFeelToUse)
{
    titleLabel.setText (
        "PLUGIN LIBRARY",
        juce::dontSendNotification
    );

    titleLabel.setFont (
        juce::Font (16.0f, juce::Font::bold)
    );

    titleLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white
    );

    countLabel.setText (
        "0 DEVICES",
        juce::dontSendNotification
    );

    countLabel.setJustificationType (
        juce::Justification::centredRight
    );

    countLabel.setColour (
        juce::Label::textColourId,
        jadLookAndFeel.colourC()
    );

    cabinetLabel.setText (
        "PLUGIN CABINET",
        juce::dontSendNotification
    );

    cabinetLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white.withAlpha (0.72f)
    );

    detailLabel.setText (
        "SELECTED DEVICE",
        juce::dontSendNotification
    );

    detailLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white.withAlpha (0.72f)
    );

    searchBox.setTextToShowWhenEmpty (
        "Search plugins...",
        juce::Colours::white.withAlpha (0.38f)
    );

    searchBox.setColour (
        juce::TextEditor::backgroundColourId,
        juce::Colours::black.withAlpha (0.32f)
    );

    searchBox.setColour (
        juce::TextEditor::outlineColourId,
        jadLookAndFeel.colourC().withAlpha (0.55f)
    );

    searchBox.setColour (
        juce::TextEditor::focusedOutlineColourId,
        jadLookAndFeel.colourA()
    );

    searchBox.setColour (
        juce::TextEditor::textColourId,
        juce::Colours::white
    );

    for (auto* component : {
            static_cast<juce::Component*> (&titleLabel),
            static_cast<juce::Component*> (&countLabel),
            static_cast<juce::Component*> (&cabinetLabel),
            static_cast<juce::Component*> (&detailLabel),
            static_cast<juce::Component*> (&searchBox),
            static_cast<juce::Component*> (&scanButton),
            static_cast<juce::Component*> (&allButton),
            static_cast<juce::Component*> (&instrumentsButton),
            static_cast<juce::Component*> (&effectsButton),
            static_cast<juce::Component*> (&favouritesButton),
            static_cast<juce::Component*> (&loadButton)
        })
    {
        addAndMakeVisible (*component);
    }

    allButton.setClickingTogglesState (true);
    instrumentsButton.setClickingTogglesState (true);
    effectsButton.setClickingTogglesState (true);
    favouritesButton.setClickingTogglesState (true);

    allButton.setToggleState (
        true,
        juce::dontSendNotification
    );

    loadButton.setEnabled (false);
}

void PluginLibraryPanel::paint (
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds().toFloat();

    g.setColour (
        juce::Colours::black.withAlpha (0.16f)
    );

    g.fillRoundedRectangle (
        bounds,
        10.0f
    );

    auto body =
        getLocalBounds().reduced (10);

    body.removeFromTop (78);

    auto cabinet =
        body.removeFromLeft (
            juce::jmax (
                220,
                body.getWidth() * 55 / 100
            )
        );

    body.removeFromLeft (10);

    auto detail =
        body;

    g.setColour (
        jadLookAndFeel.colourC().withAlpha (0.16f)
    );

    g.fillRoundedRectangle (
        cabinet.toFloat(),
        8.0f
    );

    g.setColour (
        jadLookAndFeel.colourC().withAlpha (0.52f)
    );

    g.drawRoundedRectangle (
        cabinet.toFloat(),
        8.0f,
        1.0f
    );

    g.setColour (
        jadLookAndFeel.colourB().withAlpha (0.13f)
    );

    g.fillRoundedRectangle (
        detail.toFloat(),
        8.0f
    );

    g.setColour (
        jadLookAndFeel.colourB().withAlpha (0.50f)
    );

    g.drawRoundedRectangle (
        detail.toFloat(),
        8.0f,
        1.0f
    );

    auto empty =
        cabinet.reduced (16);

    empty.removeFromTop (30);

    g.setColour (
        juce::Colours::white.withAlpha (0.34f)
    );

    g.setFont (13.0f);

    g.drawFittedText (
        "NO PLUGINS SCANNED YET\n"
        "The cabinet is waiting for EXP-050C.",
        empty,
        juce::Justification::centred,
        2
    );

    auto device =
        detail.reduced (16);

    device.removeFromTop (34);

    auto slot =
        device.removeFromTop (58);

    g.setColour (
        juce::Colours::black.withAlpha (0.28f)
    );

    g.fillRoundedRectangle (
        slot.toFloat(),
        7.0f
    );

    g.setColour (
        jadLookAndFeel.colourA().withAlpha (0.48f)
    );

    g.drawRoundedRectangle (
        slot.toFloat(),
        7.0f,
        1.0f
    );

    g.setColour (
        juce::Colours::white.withAlpha (0.46f)
    );

    g.setFont (
        juce::Font (13.0f, juce::Font::bold)
    );

    g.drawText (
        "EMPTY DEVICE SLOT",
        slot,
        juce::Justification::centred
    );

    device.removeFromTop (10);

    g.setFont (12.0f);

    const juce::String metadata =
        "NAME        —\n"
        "MAKER       —\n"
        "CATEGORY    —\n"
        "FORMAT      VST3\n"
        "TYPE        —";

    g.setColour (
        juce::Colours::white.withAlpha (0.62f)
    );

    g.drawFittedText (
        metadata,
        device,
        juce::Justification::topLeft,
        5
    );
}

void PluginLibraryPanel::resized()
{
    auto area =
        getLocalBounds().reduced (10);

    auto header =
        area.removeFromTop (30);

    titleLabel.setBounds (
        header.removeFromLeft (180)
    );

    countLabel.setBounds (
        header.removeFromRight (120)
    );

    area.removeFromTop (4);

    auto tools =
        area.removeFromTop (34);

    searchBox.setBounds (
        tools.removeFromLeft (
            juce::jmax (
                180,
                tools.getWidth() / 3
            )
        ).reduced (2)
    );

    scanButton.setBounds (
        tools.removeFromLeft (110)
            .reduced (2)
    );

    tools.removeFromLeft (8);

    allButton.setBounds (
        tools.removeFromLeft (62)
            .reduced (2)
    );

    instrumentsButton.setBounds (
        tools.removeFromLeft (110)
            .reduced (2)
    );

    effectsButton.setBounds (
        tools.removeFromLeft (82)
            .reduced (2)
    );

    favouritesButton.setBounds (
        tools.removeFromLeft (96)
            .reduced (2)
    );

    area.removeFromTop (10);

    auto cabinet =
        area.removeFromLeft (
            juce::jmax (
                220,
                area.getWidth() * 55 / 100
            )
        );

    area.removeFromLeft (10);

    auto detail =
        area;

    cabinetLabel.setBounds (
        cabinet.removeFromTop (30)
            .reduced (10, 0)
    );

    detailLabel.setBounds (
        detail.removeFromTop (30)
            .reduced (10, 0)
    );

    loadButton.setBounds (
        detail.removeFromBottom (34)
            .reduced (10, 2)
    );
}
