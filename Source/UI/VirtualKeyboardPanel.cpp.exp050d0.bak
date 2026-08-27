#include "VirtualKeyboardPanel.h"

#include "JADLookAndFeel.h"

VirtualKeyboardPanel::VirtualKeyboardPanel (
    AudioSystem& audioSystemToUse,
    JADLookAndFeel& lookAndFeelToUse)
    : audioSystem (
        audioSystemToUse
    ),
      jadLookAndFeel (
        lookAndFeelToUse
    ),
      midiSystem (
        audioSystem
    ),
      keyboard (
        midiSystem.getKeyboardState(),
        juce::MidiKeyboardComponent::horizontalKeyboard
    )
{
    titleLabel.setText (
        "CHROMATIC KEYBOARD",
        juce::dontSendNotification
    );

    titleLabel.setFont (
        juce::FontOptions (18.0f)
            .withStyle ("Bold")
    );

    activityLabel.setText (
        "MIDI • waiting",
        juce::dontSendNotification
    );

    octaveLabel.setText (
        "OCT 3",
        juce::dontSendNotification
    );

    velocityLabel.setText (
        "VELOCITY",
        juce::dontSendNotification
    );

    keyboardThemeBox.addItem (
        "NEON GALAXY",
        1
    );

    keyboardThemeBox.addItem (
        "SUNSET FIRE",
        2
    );

    keyboardThemeBox.addItem (
        "CYBER OCEAN",
        3
    );

    keyboardThemeBox.addItem (
        "ACID FOREST",
        4
    );

    keyboardThemeBox.setSelectedId (
        1,
        juce::dontSendNotification
    );

    keyboardThemeBox.onChange =
        [this]
        {
            jadLookAndFeel.setPaletteIndex (
                keyboardThemeBox.getSelectedItemIndex()
            );

            applyKeyboardTheme();

            repaint();

            if (
                auto* parent =
                    getParentComponent()
            )
            {
                parent->repaint();
            }
        };

    velocitySlider.setRange (
        0.1,
        1.0,
        0.01
    );

    velocitySlider.setValue (
        0.82
    );

    velocitySlider.setSliderStyle (
        juce::Slider::LinearHorizontal
    );

    velocitySlider.setTextBoxStyle (
        juce::Slider::TextBoxRight,
        false,
        62,
        22
    );

    keyboard.setAvailableRange (
        24,
        96
    );

    keyboard.setLowestVisibleKey (
        36
    );

    keyboard.setKeyWidth (
        24.0f
    );

    keyboard.setMidiChannel (
        1
    );

    octaveDownButton.onClick =
        [this]
        {
            baseOctave =
                juce::jmax (
                    0,
                    baseOctave - 1
                );

            keyboard.setLowestVisibleKey (
                12 * baseOctave
            );

            octaveLabel.setText (
                "OCT "
                    + juce::String (
                        baseOctave
                    ),
                juce::dontSendNotification
            );
        };

    octaveUpButton.onClick =
        [this]
        {
            baseOctave =
                juce::jmin (
                    7,
                    baseOctave + 1
                );

            keyboard.setLowestVisibleKey (
                12 * baseOctave
            );

            octaveLabel.setText (
                "OCT "
                    + juce::String (
                        baseOctave
                    ),
                juce::dontSendNotification
            );
        };

    refreshMidiButton.onClick =
        [this]
        {
            midiSystem.refreshDevices();
            rebuildMidiDeviceList();
        };

    midiDeviceBox.onChange =
        [this]
        {
            const auto index =
                midiDeviceBox
                    .getSelectedItemIndex()
                - 1;

            if (index < 0)
            {
                midiSystem.closeDevice();
                return;
            }

            midiSystem.openDevice (
                index
            );
        };

    addAndMakeVisible (titleLabel);
    addAndMakeVisible (activityLabel);
    addAndMakeVisible (octaveLabel);
    addAndMakeVisible (velocityLabel);

    addAndMakeVisible (midiDeviceBox);
    addAndMakeVisible (keyboardThemeBox);
    addAndMakeVisible (octaveDownButton);
    addAndMakeVisible (octaveUpButton);
    addAndMakeVisible (refreshMidiButton);
    addAndMakeVisible (velocitySlider);
    addAndMakeVisible (keyboard);

    rebuildMidiDeviceList();
    applyKeyboardTheme();

    startTimerHz (24);
}

void VirtualKeyboardPanel::rebuildMidiDeviceList()
{
    midiDeviceBox.clear (
        juce::dontSendNotification
    );

    midiDeviceBox.addItem (
        "VIRTUAL / NONE",
        1
    );

    for (
        int i = 0;
        i < midiSystem.getDeviceCount();
        ++i
    )
    {
        midiDeviceBox.addItem (
            midiSystem.getDeviceName (i),
            i + 2
        );
    }

    midiDeviceBox.setSelectedId (
        1,
        juce::dontSendNotification
    );
}

void VirtualKeyboardPanel::applyKeyboardTheme()
{
    const auto primary =
        jadLookAndFeel.colourA();

    const auto secondary =
        jadLookAndFeel.colourD();

    keyboard.setColour (
        juce::MidiKeyboardComponent::whiteNoteColourId,
        primary
            .interpolatedWith (
                juce::Colours::white,
                0.72f
            )
    );

    keyboard.setColour (
        juce::MidiKeyboardComponent::blackNoteColourId,
        juce::Colour::fromRGB (
            20,
            10,
            35
        )
    );

    keyboard.setColour (
        juce::MidiKeyboardComponent::keySeparatorLineColourId,
        secondary.withAlpha (0.65f)
    );

    keyboard.setColour (
        juce::MidiKeyboardComponent::mouseOverKeyOverlayColourId,
        secondary.withAlpha (0.50f)
    );

    keyboard.setColour (
        juce::MidiKeyboardComponent::keyDownOverlayColourId,
        primary.withAlpha (0.88f)
    );

    keyboard.setColour (
        juce::MidiKeyboardComponent::textLabelColourId,
        juce::Colour::fromRGB (
            35,
            15,
            55
        )
    );
}

void VirtualKeyboardPanel::timerCallback()
{
    const auto note =
        midiSystem.getLastNote();

    if (note < 0)
    {
        activityLabel.setText (
            "MIDI • waiting",
            juce::dontSendNotification
        );

        return;
    }

    activityLabel.setText (
        "MIDI • NOTE "
            + juce::String (note)
            + " • VEL "
            + juce::String (
                midiSystem.getLastVelocity(),
                2
            ),
        juce::dontSendNotification
    );
}

void VirtualKeyboardPanel::paint (
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds()
            .toFloat();

    juce::ColourGradient glass (
        jadLookAndFeel.colourB()
            .withAlpha (0.30f),
        bounds.getTopLeft(),
        jadLookAndFeel.colourD()
            .withAlpha (0.14f),
        bounds.getBottomRight(),
        false
    );

    g.setGradientFill (glass);

    g.fillRoundedRectangle (
        bounds,
        18.0f
    );

    g.setColour (
        jadLookAndFeel.colourD()
            .withAlpha (0.58f)
    );

    g.drawRoundedRectangle (
        bounds.reduced (1.0f),
        18.0f,
        1.5f
    );
}

void VirtualKeyboardPanel::resized()
{
    auto area =
        getLocalBounds()
            .reduced (16);

    auto header =
        area.removeFromTop (34);

    titleLabel.setBounds (
        header.removeFromLeft (230)
    );

    activityLabel.setBounds (
        header.removeFromRight (180)
    );

    auto controls =
        area.removeFromTop (42);

    midiDeviceBox.setBounds (
        controls.removeFromLeft (230)
            .reduced (3)
    );

    refreshMidiButton.setBounds (
        controls.removeFromLeft (125)
            .reduced (3)
    );

    keyboardThemeBox.setBounds (
        controls.removeFromLeft (165)
            .reduced (3)
    );

    octaveDownButton.setBounds (
        controls.removeFromLeft (72)
            .reduced (3)
    );

    octaveLabel.setBounds (
        controls.removeFromLeft (58)
            .reduced (3)
    );

    octaveUpButton.setBounds (
        controls.removeFromLeft (72)
            .reduced (3)
    );

    velocityLabel.setBounds (
        controls.removeFromLeft (68)
            .reduced (3)
    );

    velocitySlider.setBounds (
        controls.reduced (3)
    );

    area.removeFromTop (8);

    keyboard.setBounds (
        area
    );
}
