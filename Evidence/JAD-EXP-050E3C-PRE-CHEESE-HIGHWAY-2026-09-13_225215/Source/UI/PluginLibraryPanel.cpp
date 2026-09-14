#include "PluginLibraryPanel.h"

PluginLibraryPanel::PluginLibraryPanel (
    PluginCatalog& catalogToUse,
    JADLookAndFeel& lookAndFeelToUse)
    : catalog (catalogToUse),
      jadLookAndFeel (lookAndFeelToUse)
{
    titleLabel.setText (
        "PLUGIN LIBRARY",
        juce::dontSendNotification
    );

    titleLabel.setFont (
        juce::FontOptions (16.0f)
            .withStyle ("Bold")
    );

    titleLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white
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

    scanStatusLabel.setText (
        catalog.getLastScanSummary(),
        juce::dontSendNotification
    );

    scanStatusLabel.setFont (
        juce::FontOptions (10.0f)
    );

    scanStatusLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white.withAlpha (0.48f)
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

    pluginList.setRowHeight (28);

    pluginList.setColour (
        juce::ListBox::backgroundColourId,
        juce::Colours::transparentBlack
    );

    pluginList.setColour (
        juce::ListBox::outlineColourId,
        juce::Colours::transparentBlack
    );

    for (auto* component : {
            static_cast<juce::Component*> (&titleLabel),
            static_cast<juce::Component*> (&countLabel),
            static_cast<juce::Component*> (&cabinetLabel),
            static_cast<juce::Component*> (&detailLabel),
            static_cast<juce::Component*> (&scanStatusLabel),
            static_cast<juce::Component*> (&searchBox),
            static_cast<juce::Component*> (&scanButton),
            static_cast<juce::Component*> (&allButton),
            static_cast<juce::Component*> (&instrumentsButton),
            static_cast<juce::Component*> (&effectsButton),
            static_cast<juce::Component*> (&favouritesButton),
            static_cast<juce::Component*> (&loadButton),
            static_cast<juce::Component*> (&pluginList)
        })
    {
        addAndMakeVisible (*component);
    }

    allButton.setClickingTogglesState (true);
    instrumentsButton.setClickingTogglesState (true);
    effectsButton.setClickingTogglesState (true);

    allButton.setRadioGroupId (4101);
    instrumentsButton.setRadioGroupId (4101);
    effectsButton.setRadioGroupId (4101);

    allButton.setToggleState (
        true,
        juce::dontSendNotification
    );

    favouritesButton.setEnabled (false);
    loadButton.setEnabled (false);

    scanButton.onClick =
        [this]
        {
            scanButton.setEnabled (false);

            scanStatusLabel.setText (
                "Scanning VST3 locations...",
                juce::dontSendNotification
            );

            repaint();

            catalog.scanVST3();

            scanStatusLabel.setText (
                catalog.getLastScanSummary(),
                juce::dontSendNotification
            );

            scanButton.setEnabled (true);

            refreshPlugins();
        };

    searchBox.onTextChange =
        [this]
        {
            refreshPlugins();
        };

    loadButton.onClick =
        [this]
        {
            if (
                selectedPluginIndex < 0
                || selectedPluginIndex >= visiblePlugins.size()
            )
            {
                detailLabel.setText (
                    "SELECT A PLUGIN FIRST",
                    juce::dontSendNotification
                );

                return;
            }

            const auto description =
                visiblePlugins[
                    selectedPluginIndex
                ];

            juce::String errorMessage;

            constexpr double bootstrapSampleRate =
                44100.0;

            constexpr int bootstrapBlockSize =
                512;

            auto instance =
                catalog.createInstance (
                    description,
                    bootstrapSampleRate,
                    bootstrapBlockSize,
                    errorMessage
                );

            if (instance == nullptr)
            {
                detailLabel.setText (
                    "INSTANCE FAILED | "
                        + (
                            errorMessage.isNotEmpty()
                                ? errorMessage
                                : juce::String (
                                    "UNKNOWN PLUGIN ERROR"
                                )
                        ),
                    juce::dontSendNotification
                );

                return;
            }

            instance->prepareToPlay (
                bootstrapSampleRate,
                bootstrapBlockSize
            );

            const auto loadedName =
                instance->getName();

            loadedPluginInstance =
                std::move (
                    instance
                );

            detailLabel.setText (
                "CHEESE INSTANCE ONLINE | "
                    + loadedName,
                juce::dontSendNotification
            );

            loadButton.setButtonText (
                "INSTANCE ONLINE"
            );

            repaint();
        };

    allButton.onClick =
        [this]
        {
            setFilter (
                Filter::all
            );
        };

    instrumentsButton.onClick =
        [this]
        {
            setFilter (
                Filter::instruments
            );
        };

    effectsButton.onClick =
        [this]
        {
            setFilter (
                Filter::effects
            );
        };

    refreshPlugins();
}

int PluginLibraryPanel::getNumRows()
{
    return visiblePlugins.size();
}

juce::String PluginLibraryPanel::getPluginTypeText (
    const juce::PluginDescription& plugin
) const
{
    if (plugin.isInstrument)
        return "INSTRUMENT";

    return "EFFECT";
}

bool PluginLibraryPanel::passesFilter (
    const juce::PluginDescription& plugin
) const
{
    if (filter == Filter::instruments)
        return plugin.isInstrument;

    if (filter == Filter::effects)
        return ! plugin.isInstrument;

    return true;
}

void PluginLibraryPanel::setFilter (
    Filter newFilter)
{
    filter = newFilter;

    allButton.setToggleState (
        filter == Filter::all,
        juce::dontSendNotification
    );

    instrumentsButton.setToggleState (
        filter == Filter::instruments,
        juce::dontSendNotification
    );

    effectsButton.setToggleState (
        filter == Filter::effects,
        juce::dontSendNotification
    );

    refreshPlugins();
}

void PluginLibraryPanel::refreshPlugins()
{
    selectedPluginIndex = -1;

    loadButton.setEnabled (false);

    const auto query =
        searchBox.getText();

    const auto source =
        query.trim().isEmpty()
            ? catalog.getPlugins()
            : catalog.search (query);

    visiblePlugins.clearQuick();

    for (const auto& plugin : source)
    {
        if (passesFilter (plugin))
            visiblePlugins.add (plugin);
    }

    countLabel.setText (
        juce::String (visiblePlugins.size())
            + " DEVICES",
        juce::dontSendNotification
    );

    pluginList.deselectAllRows();
    pluginList.updateContent();
    pluginList.repaint();

    repaint();
}

void PluginLibraryPanel::paintListBoxItem (
    int rowNumber,
    juce::Graphics& g,
    int width,
    int height,
    bool rowIsSelected)
{
    if (! juce::isPositiveAndBelow (
            rowNumber,
            visiblePlugins.size()
        ))
    {
        return;
    }

    const auto& plugin =
        visiblePlugins.getReference (
            rowNumber
        );

    auto bounds =
        juce::Rectangle<int> (
            0,
            0,
            width,
            height
        )
        .reduced (3, 2);

    if (rowIsSelected)
    {
        g.setColour (
            jadLookAndFeel.colourA()
                .withAlpha (0.20f)
        );

        g.fillRoundedRectangle (
            bounds.toFloat(),
            5.0f
        );
    }

    auto typeBounds =
        bounds.removeFromRight (92);

    auto makerBounds =
        bounds.removeFromRight (
            juce::jmin (
                150,
                width / 3
            )
        );

    g.setColour (
        juce::Colours::white.withAlpha (
            rowIsSelected ? 0.96f : 0.76f
        )
    );

    g.setFont (
        juce::FontOptions (12.0f)
            .withStyle (
                rowIsSelected
                    ? "Bold"
                    : "Regular"
            )
    );

    g.drawFittedText (
        plugin.name,
        bounds.reduced (7, 0),
        juce::Justification::centredLeft,
        1
    );

    g.setColour (
        juce::Colours::white.withAlpha (0.40f)
    );

    g.setFont (
        juce::FontOptions (10.0f)
    );

    g.drawFittedText (
        plugin.manufacturerName,
        makerBounds.reduced (5, 0),
        juce::Justification::centredLeft,
        1
    );

    g.setColour (
        plugin.isInstrument
            ? jadLookAndFeel.colourB()
            : jadLookAndFeel.colourC()
    );

    g.drawFittedText (
        getPluginTypeText (plugin),
        typeBounds.reduced (5, 0),
        juce::Justification::centredRight,
        1
    );
}

void PluginLibraryPanel::selectedRowsChanged (
    int lastRowSelected)
{
    if (! juce::isPositiveAndBelow (
            lastRowSelected,
            visiblePlugins.size()
        ))
    {
        selectedPluginIndex = -1;
        loadButton.setEnabled (false);
        repaint();
        return;
    }

    selectedPluginIndex =
        lastRowSelected;

    loadButton.setEnabled (true);

    repaint();
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

    body.removeFromTop (102);

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

    if (visiblePlugins.isEmpty())
    {
        auto empty =
            cabinet.reduced (16);

        empty.removeFromTop (30);

        g.setColour (
            juce::Colours::white.withAlpha (0.34f)
        );

        g.setFont (
            juce::FontOptions (13.0f)
        );

        g.drawFittedText (
            catalog.getNumPlugins() == 0
                ? "NO PLUGINS CATALOGUED YET\nPRESS SCAN VST3 TO FEED THE CABINET"
                : "NO PLUGINS MATCH THE CURRENT FILTER",
            empty,
            juce::Justification::centred,
            2
        );
    }

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

    g.setFont (
        juce::FontOptions (13.0f)
            .withStyle ("Bold")
    );

    if (juce::isPositiveAndBelow (
            selectedPluginIndex,
            visiblePlugins.size()
        ))
    {
        const auto& plugin =
            visiblePlugins.getReference (
                selectedPluginIndex
            );

        g.setColour (
            juce::Colours::white.withAlpha (0.90f)
        );

        g.drawFittedText (
            plugin.name,
            slot.reduced (8),
            juce::Justification::centred,
            2
        );

        device.removeFromTop (10);

        g.setFont (
            juce::FontOptions (12.0f)
        );

        const juce::String metadata =
            "NAME        "
            + plugin.name
            + "\nMAKER       "
            + plugin.manufacturerName
            + "\nCATEGORY    "
            + plugin.category
            + "\nFORMAT      "
            + plugin.pluginFormatName
            + "\nTYPE        "
            + getPluginTypeText (plugin);

        g.setColour (
            juce::Colours::white.withAlpha (0.66f)
        );

        g.drawFittedText (
            metadata,
            device,
            juce::Justification::topLeft,
            5
        );
    }
    else
    {
        g.setColour (
            juce::Colours::white.withAlpha (0.46f)
        );

        g.drawText (
            "EMPTY DEVICE SLOT",
            slot,
            juce::Justification::centred
        );

        device.removeFromTop (10);

        g.setFont (
            juce::FontOptions (12.0f)
        );

        g.setColour (
            juce::Colours::white.withAlpha (0.62f)
        );

        g.drawFittedText (
            "NAME        -\n"
            "MAKER       -\n"
            "CATEGORY    -\n"
            "FORMAT      VST3\n"
            "TYPE        -",
            device,
            juce::Justification::topLeft,
            5
        );
    }
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

    area.removeFromTop (2);

    scanStatusLabel.setBounds (
        area.removeFromTop (22)
            .reduced (4, 0)
    );

    area.removeFromTop (8);

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

    pluginList.setBounds (
        cabinet.reduced (8, 4)
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