#include "AudioSettingsPanel.h"

AudioSettingsPanel::AudioSettingsPanel (
    AudioSystem& audioSystemToUse,
    MidiSystem& midiSystemToUse,
    JADLookAndFeel& lookAndFeelToUse)
    : audioSystem (audioSystemToUse),
      midiSystem (midiSystemToUse),
      jadLookAndFeel (lookAndFeelToUse),
      deviceSelector (
          audioSystem.getDeviceManager(),
          0,
          2,
          0,
          2,
          true,
          false,
          true,
          false
      )
{
    titleLabel.setText (
        "JAD PREFERENCES",
        juce::dontSendNotification
    );

    titleLabel.setFont (
        juce::FontOptions (18.0f)
            .withStyle ("Bold")
    );

    statusLabel.setFont (
        juce::FontOptions (13.0f)
            .withStyle ("Bold")
    );

    detailLabel.setFont (
        juce::FontOptions (11.0f)
    );

    detailLabel.setJustificationType (
        juce::Justification::centredRight
    );

    closeButton.onClick =
        [this]
        {
            if (onClose)
                onClose();
        };

    for (auto* button : {
            &audioTabButton,
            &midiTabButton,
            &recordingTabButton,
            &appearanceTabButton,
            &updatesTabButton
        })
    {
        button->setClickingTogglesState (true);
        button->setRadioGroupId (7301);
        addAndMakeVisible (*button);
    }

    audioTabButton.onClick =
        [this]
        {
            setPreferencesPage (0);
        };

    midiTabButton.onClick =
        [this]
        {
            setPreferencesPage (1);
        };

    recordingTabButton.onClick =
        [this]
        {
            setPreferencesPage (2);
        };

    appearanceTabButton.onClick =
        [this]
        {
            setPreferencesPage (3);
        };

    updatesTabButton.onClick =
        [this]
        {
            setPreferencesPage (4);
        };

    placeholderLabel.setJustificationType (
        juce::Justification::centred
    );

    placeholderLabel.setFont (
        juce::FontOptions (16.0f)
            .withStyle ("Bold")
    );

    placeholderLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white.withAlpha (0.64f)
    );

    midiInputTitleLabel.setText (
        "LIVE MIDI INPUT",
        juce::dontSendNotification
    );

    midiInputTitleLabel.setFont (
        juce::FontOptions (16.0f)
            .withStyle ("Bold")
    );

    midiConnectionLabel.setFont (
        juce::FontOptions (14.0f)
            .withStyle ("Bold")
    );

    midiActivityLabel.setFont (
        juce::FontOptions (13.0f)
    );

    midiHelpLabel.setText (
        "PC keyboard and the onscreen piano always remain active.\n"
        "Selecting a USB controller adds it to the same armed-track input.",
        juce::dontSendNotification
    );

    midiHelpLabel.setColour (
        juce::Label::textColourId,
        juce::Colours::white.withAlpha (0.62f)
    );

    midiHelpLabel.setJustificationType (
        juce::Justification::topLeft
    );

    midiInputBox.onChange =
        [this]
        {
            const int selectedIndex =
                midiInputBox.getSelectedItemIndex();

            if (selectedIndex <= 0)
            {
                midiSystem.selectComputerKeyboardOnly();
            }
            else
            {
                midiSystem.openDevice (
                    selectedIndex - 1
                );
            }

            refreshMidiPage();
        };

    autoReconnectButton.setToggleState (
        midiSystem.isAutoReconnectEnabled(),
        juce::dontSendNotification
    );

    autoReconnectButton.onClick =
        [this]
        {
            midiSystem.setAutoReconnectEnabled (
                autoReconnectButton.getToggleState()
            );
        };

    refreshMidiButton.onClick =
        [this]
        {
            midiSystem.refreshDevices();
            rebuildMidiInputList();
            refreshMidiPage();
        };

    addAndMakeVisible (titleLabel);
    addAndMakeVisible (statusLabel);
    addAndMakeVisible (detailLabel);
    addAndMakeVisible (placeholderLabel);
    addAndMakeVisible (closeButton);
    addAndMakeVisible (deviceSelector);

    addAndMakeVisible (midiInputTitleLabel);
    addAndMakeVisible (midiConnectionLabel);
    addAndMakeVisible (midiActivityLabel);
    addAndMakeVisible (midiHelpLabel);
    addAndMakeVisible (midiInputBox);
    addAndMakeVisible (autoReconnectButton);
    addAndMakeVisible (refreshMidiButton);

    rebuildMidiInputList();
    setPreferencesPage (0);
    refreshStatus();

    startTimerHz (4);
}

AudioSettingsPanel::~AudioSettingsPanel()
{
    stopTimer();
}

void AudioSettingsPanel::setPreferencesPage (
    int page)
{
    activePreferencesPage =
        juce::jlimit (
            0,
            4,
            page
        );

    audioTabButton.setToggleState (
        activePreferencesPage == 0,
        juce::dontSendNotification
    );

    midiTabButton.setToggleState (
        activePreferencesPage == 1,
        juce::dontSendNotification
    );

    recordingTabButton.setToggleState (
        activePreferencesPage == 2,
        juce::dontSendNotification
    );

    appearanceTabButton.setToggleState (
        activePreferencesPage == 3,
        juce::dontSendNotification
    );

    updatesTabButton.setToggleState (
        activePreferencesPage == 4,
        juce::dontSendNotification
    );

    const bool audioVisible =
        activePreferencesPage == 0;

    const bool midiVisible =
        activePreferencesPage == 1;

    deviceSelector.setVisible (
        audioVisible
    );

    for (auto* component : {
            static_cast<juce::Component*> (&midiInputTitleLabel),
            static_cast<juce::Component*> (&midiConnectionLabel),
            static_cast<juce::Component*> (&midiActivityLabel),
            static_cast<juce::Component*> (&midiHelpLabel),
            static_cast<juce::Component*> (&midiInputBox),
            static_cast<juce::Component*> (&autoReconnectButton),
            static_cast<juce::Component*> (&refreshMidiButton)
        })
    {
        component->setVisible (
            midiVisible
        );
    }

    placeholderLabel.setVisible (
        ! audioVisible
        && ! midiVisible
    );

    static const juce::String pageNames[] {
        "AUDIO",
        "MIDI",
        "RECORDING",
        "APPEARANCE",
        "UPDATES"
    };

    if (
        audioVisible
        || midiVisible
    )
    {
        placeholderLabel.setText (
            {},
            juce::dontSendNotification
        );
    }
    else
    {
        placeholderLabel.setText (
            pageNames[activePreferencesPage]
                + " PREFERENCES\n\n"
                + "CONTROL SURFACE RESERVED FOR THE NEXT ENGINE PASS",
            juce::dontSendNotification
        );
    }

    if (midiVisible)
    {
        midiSystem.refreshDevices();
        rebuildMidiInputList();
        refreshMidiPage();
    }

    refreshStatus();
    resized();
    repaint();
}

void AudioSettingsPanel::timerCallback()
{
    refreshStatus();

    ++midiRefreshCounter;

    if (
        midiRefreshCounter >= 4
        && activePreferencesPage == 1
    )
    {
        midiRefreshCounter = 0;

        midiSystem.refreshDevices();
        rebuildMidiInputList();
        refreshMidiPage();
    }
}

void AudioSettingsPanel::rebuildMidiInputList()
{
    const auto preferredIdentifier =
        midiSystem.getPreferredDeviceIdentifier();

    midiInputBox.clear (
        juce::dontSendNotification
    );

    midiInputBox.addItem (
        "PC KEYBOARD + ONSCREEN PIANO",
        1
    );

    int selectedId = 1;

    for (
        int index = 0;
        index < midiSystem.getDeviceCount();
        ++index
    )
    {
        midiInputBox.addItem (
            midiSystem.getDeviceName (index),
            index + 2
        );

        if (
            midiSystem.getDeviceIdentifier (index)
            == preferredIdentifier
        )
        {
            selectedId = index + 2;
        }
    }

    midiInputBox.setSelectedId (
        selectedId,
        juce::dontSendNotification
    );
}

void AudioSettingsPanel::refreshMidiPage()
{
    autoReconnectButton.setToggleState (
        midiSystem.isAutoReconnectEnabled(),
        juce::dontSendNotification
    );

    if (midiSystem.isDeviceOpen())
    {
        midiConnectionLabel.setText (
            "CONNECTED  |  "
                + midiSystem.getOpenDeviceName(),
            juce::dontSendNotification
        );

        midiConnectionLabel.setColour (
            juce::Label::textColourId,
            juce::Colour::fromRGB (
                36,
                231,
                180
            )
        );
    }
    else if (
        midiSystem
            .getPreferredDeviceIdentifier()
            .isNotEmpty()
    )
    {
        midiConnectionLabel.setText (
            "WAITING FOR SAVED USB MIDI DEVICE",
            juce::dontSendNotification
        );

        midiConnectionLabel.setColour (
            juce::Label::textColourId,
            juce::Colour::fromRGB (
                255,
                178,
                70
            )
        );
    }
    else
    {
        midiConnectionLabel.setText (
            "PC KEYBOARD ONLINE",
            juce::dontSendNotification
        );

        midiConnectionLabel.setColour (
            juce::Label::textColourId,
            juce::Colour::fromRGB (
                32,
                218,
                235
            )
        );
    }

    const auto note =
        midiSystem.getLastNote();

    if (
        midiSystem.isMidiActive()
        && note >= 0
    )
    {
        midiActivityLabel.setText (
            "LIVE NOTE: "
                + juce::String (note)
                + "    VELOCITY: "
                + juce::String (
                    midiSystem.getLastVelocity(),
                    2
                ),
            juce::dontSendNotification
        );
    }
    else
    {
        midiActivityLabel.setText (
            "Play a key to test live MIDI input.",
            juce::dontSendNotification
        );
    }
}

void AudioSettingsPanel::refreshStatus()
{
    if (activePreferencesPage == 1)
    {
        statusLabel.setText (
            midiSystem.isDeviceOpen()
                ? "USB MIDI ONLINE"
                : "MIDI ONLINE",
            juce::dontSendNotification
        );

        detailLabel.setText (
            midiSystem.isDeviceOpen()
                ? midiSystem.getOpenDeviceName()
                : "PC KEYBOARD",
            juce::dontSendNotification
        );

        refreshMidiPage();
        return;
    }

    auto* device =
        audioSystem.getDeviceManager()
            .getCurrentAudioDevice();

    if (device == nullptr)
    {
        statusLabel.setText (
            "AUDIO OFFLINE",
            juce::dontSendNotification
        );

        detailLabel.setText (
            "NO ACTIVE DEVICE",
            juce::dontSendNotification
        );

        return;
    }

    statusLabel.setText (
        "AUDIO ONLINE",
        juce::dontSendNotification
    );

    detailLabel.setText (
        juce::String (
            device->getCurrentSampleRate(),
            0
        )
            + " Hz  /  "
            + juce::String (
                device->getCurrentBufferSizeSamples()
            )
            + " samples",
        juce::dontSendNotification
    );
}

void AudioSettingsPanel::paint (
    juce::Graphics& g)
{
    const auto bounds =
        getLocalBounds().toFloat();

    g.setColour (
        juce::Colour::fromRGB (
            7,
            5,
            18
        )
        .withAlpha (0.98f)
    );

    g.fillRoundedRectangle (
        bounds,
        16.0f
    );

    g.setColour (
        jadLookAndFeel.colourA()
            .withAlpha (0.90f)
    );

    g.drawRoundedRectangle (
        bounds.reduced (1.0f),
        16.0f,
        2.0f
    );

    if (activePreferencesPage == 1)
    {
        auto card =
            getLocalBounds()
                .reduced (22)
                .withTrimmedTop (126)
                .toFloat();

        g.setColour (
            juce::Colour::fromRGB (
                17,
                27,
                55
            )
            .withAlpha (0.80f)
        );

        g.fillRoundedRectangle (
            card,
            12.0f
        );

        g.setColour (
            jadLookAndFeel.colourD()
                .withAlpha (0.55f)
        );

        g.drawRoundedRectangle (
            card,
            12.0f,
            1.5f
        );
    }
}

void AudioSettingsPanel::resized()
{
    auto area =
        getLocalBounds()
            .reduced (18);

    auto header =
        area.removeFromTop (38);

    titleLabel.setBounds (
        header.removeFromLeft (220)
    );

    closeButton.setBounds (
        header.removeFromRight (42)
            .reduced (3)
    );

    detailLabel.setBounds (
        header.removeFromRight (230)
    );

    statusLabel.setBounds (
        header
    );

    area.removeFromTop (8);

    auto tabs =
        area.removeFromTop (38);

    audioTabButton.setBounds (
        tabs.removeFromLeft (82)
            .reduced (2)
    );

    midiTabButton.setBounds (
        tabs.removeFromLeft (76)
            .reduced (2)
    );

    recordingTabButton.setBounds (
        tabs.removeFromLeft (112)
            .reduced (2)
    );

    appearanceTabButton.setBounds (
        tabs.removeFromLeft (118)
            .reduced (2)
    );

    updatesTabButton.setBounds (
        tabs.removeFromLeft (92)
            .reduced (2)
    );

    area.removeFromTop (10);

    deviceSelector.setBounds (
        area
    );

    placeholderLabel.setBounds (
        area.reduced (20)
    );

    auto midiArea =
        area.reduced (30);

    midiInputTitleLabel.setBounds (
        midiArea.removeFromTop (32)
    );

    midiArea.removeFromTop (8);

    auto selectorRow =
        midiArea.removeFromTop (42);

    midiInputBox.setBounds (
        selectorRow.removeFromLeft (
            juce::jmax (
                260,
                selectorRow.getWidth() - 170
            )
        )
        .reduced (2)
    );

    refreshMidiButton.setBounds (
        selectorRow.reduced (2)
    );

    midiArea.removeFromTop (18);

    midiConnectionLabel.setBounds (
        midiArea.removeFromTop (32)
    );

    midiActivityLabel.setBounds (
        midiArea.removeFromTop (30)
    );

    midiArea.removeFromTop (12);

    autoReconnectButton.setBounds (
        midiArea.removeFromTop (34)
    );

    midiArea.removeFromTop (20);

    midiHelpLabel.setBounds (
        midiArea.removeFromTop (70)
    );
}