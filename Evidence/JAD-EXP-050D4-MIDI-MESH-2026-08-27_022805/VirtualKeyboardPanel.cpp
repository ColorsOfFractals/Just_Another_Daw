#include "VirtualKeyboardPanel.h"

#include <array>

namespace
{
    constexpr int firstVisibleMidiNote = 12;  // C0 in JAD display language
    constexpr int lastVisibleMidiNote  = 84;  // C6

    struct ComputerKeyMap
    {
        juce::uint32 key;
        int semitone;
    };

    constexpr std::array<ComputerKeyMap, 13> computerKeyMap {{
        { 'a',  0 },
        { 'w',  1 },
        { 's',  2 },
        { 'e',  3 },
        { 'd',  4 },
        { 'f',  5 },
        { 't',  6 },
        { 'g',  7 },
        { 'y',  8 },
        { 'h',  9 },
        { 'u', 10 },
        { 'j', 11 },
        { 'k', 12 }
    }};
}

VirtualKeyboardPanel::VirtualKeyboardPanel (
    AudioSystem& audioSystemToUse,
    JADLookAndFeel& lookAndFeelToUse)
    : audioSystem (audioSystemToUse),
      jadLookAndFeel (lookAndFeelToUse),
      midiSystem (audioSystem),
      keyboard (
          midiSystem.getKeyboardState(),
          juce::MidiKeyboardComponent::horizontalKeyboard
      )
{
    setWantsKeyboardFocus (true);

    titleLabel.setText (
        "CHROMATIC KEYBOARD",
        juce::dontSendNotification
    );

    titleLabel.setFont (
        juce::FontOptions (18.0f)
            .withStyle ("Bold")
    );

    activityLabel.setText (
        "MIDI - waiting",
        juce::dontSendNotification
    );

    activityLabel.setJustificationType (
        juce::Justification::centredRight
    );

    octaveLabel.setText (
        "OCT 3",
        juce::dontSendNotification
    );

    octaveLabel.setJustificationType (
        juce::Justification::centred
    );

    velocityLabel.setText (
        "VELOCITY",
        juce::dontSendNotification
    );

    keyboardThemeBox.addItem ("NEON GALAXY", 1);
    keyboardThemeBox.addItem ("SUNSET FIRE", 2);
    keyboardThemeBox.addItem ("CYBER OCEAN", 3);
    keyboardThemeBox.addItem ("ACID FOREST", 4);

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

            if (auto* parent = getParentComponent())
                parent->repaint();
        };

    velocitySlider.setRange (
        0.1,
        1.0,
        0.01
    );

    velocitySlider.setValue (0.82);

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
        firstVisibleMidiNote,
        lastVisibleMidiNote
    );

    keyboard.setLowestVisibleKey (
        firstVisibleMidiNote
    );

    keyboard.setKeyWidth (24.0f);

    keyboard.setMidiChannel (1);

    keyboard.setScrollButtonsVisible (false);

    octaveDownButton.onClick =
        [this]
        {
            changeComputerOctave (-1);
        };

    octaveUpButton.onClick =
        [this]
        {
            changeComputerOctave (1);
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
                midiDeviceBox.getSelectedItemIndex() - 1;

            if (index < 0)
            {
                midiSystem.closeDevice();
                return;
            }

            midiSystem.openDevice (index);
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

    startTimerHz (30);
}

VirtualKeyboardPanel::~VirtualKeyboardPanel()
{
    for (const auto note : heldComputerNotes)
    {
        midiSystem.getKeyboardState().noteOff (
            1,
            note,
            0.0f
        );
    }
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
        primary.interpolatedWith (
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

void VirtualKeyboardPanel::changeComputerOctave (
    int delta)
{
    const int newOctave =
        juce::jlimit (
            0,
            5,
            baseOctave + delta
        );

    if (newOctave == baseOctave)
        return;

    for (const auto note : heldComputerNotes)
    {
        midiSystem.getKeyboardState().noteOff (
            1,
            note,
            0.0f
        );
    }

    heldComputerNotes.clear();

    baseOctave = newOctave;

    octaveLabel.setText (
        "OCT " + juce::String (baseOctave),
        juce::dontSendNotification
    );

    repaint();
}

int VirtualKeyboardPanel::getMidiNoteForComputerKey (
    juce::uint32 key) const
{
    key = juce::CharacterFunctions::toLowerCase (key);

    for (const auto& mapping : computerKeyMap)
    {
        if (mapping.key == key)
        {
            const int note =
                firstVisibleMidiNote
                + (baseOctave * 12)
                + mapping.semitone;

            if (
                note >= firstVisibleMidiNote
                && note <= lastVisibleMidiNote
            )
            {
                return note;
            }

            return -1;
        }
    }

    return -1;
}

bool VirtualKeyboardPanel::keyPressed (
    const juce::KeyPress& key)
{
    const auto character =
        juce::CharacterFunctions::toLowerCase (
            key.getTextCharacter()
        );

    if (character == 'z')
    {
        changeComputerOctave (-1);
        return true;
    }

    if (character == 'x')
    {
        changeComputerOctave (1);
        return true;
    }

    const int note =
        getMidiNoteForComputerKey (
            character
        );

    if (note < 0)
        return false;

    if (! heldComputerNotes.contains (note))
    {
        heldComputerNotes.add (note);

        midiSystem.getKeyboardState().noteOn (
            1,
            note,
            static_cast<float> (
                velocitySlider.getValue()
            )
        );
    }

    return true;
}

bool VirtualKeyboardPanel::keyStateChanged (
    bool)
{
    refreshComputerNotes();
    return false;
}

void VirtualKeyboardPanel::refreshComputerNotes()
{
    for (
        int i = heldComputerNotes.size() - 1;
        i >= 0;
        --i
    )
    {
        const int note =
            heldComputerNotes.getUnchecked (i);

        bool stillHeld = false;

        for (const auto& mapping : computerKeyMap)
        {
            const int mappedNote =
                firstVisibleMidiNote
                + (baseOctave * 12)
                + mapping.semitone;

            if (mappedNote != note)
                continue;

            if (
                juce::KeyPress::isKeyCurrentlyDown (
                    mapping.key
                )
            )
            {
                stillHeld = true;
            }

            break;
        }

        if (! stillHeld)
        {
            midiSystem.getKeyboardState().noteOff (
                1,
                note,
                0.0f
            );

            heldComputerNotes.remove (i);
        }
    }
}

void VirtualKeyboardPanel::timerCallback()
{
    refreshComputerNotes();

    const auto note =
        midiSystem.getLastNote();

    if (note < 0)
    {
        activityLabel.setText (
            "MIDI - waiting",
            juce::dontSendNotification
        );

        return;
    }

    activityLabel.setText (
        "MIDI - NOTE "
            + juce::String (note)
            + " - VEL "
            + juce::String (
                midiSystem.getLastVelocity(),
                2
            ),
        juce::dontSendNotification
    );
}

void VirtualKeyboardPanel::drawOctaveMarkers (
    juce::Graphics& g,
    juce::Rectangle<int> bounds)
{
    if (bounds.isEmpty())
        return;

    const int startSemitone =
        baseOctave * 12;

    const float startRatio =
        static_cast<float> (startSemitone)
        / 72.0f;

    const float endRatio =
        static_cast<float> (startSemitone + 12)
        / 72.0f;

    const float x1 =
        bounds.getX()
        + bounds.getWidth() * startRatio;

    const float x2 =
        bounds.getX()
        + bounds.getWidth() * endRatio;

    const float y =
        static_cast<float> (
            bounds.getY() - 6
        );

    g.setColour (
        jadLookAndFeel.colourA()
            .withAlpha (0.95f)
    );

    g.fillEllipse (
        x1 - 4.0f,
        y - 4.0f,
        8.0f,
        8.0f
    );

    g.fillEllipse (
        x2 - 4.0f,
        y - 4.0f,
        8.0f,
        8.0f
    );

    g.setColour (
        jadLookAndFeel.colourD()
            .withAlpha (0.55f)
    );

    g.drawLine (
        x1,
        y,
        x2,
        y,
        1.5f
    );
}

void VirtualKeyboardPanel::drawComputerKeyboardLegend (
    juce::Graphics& g,
    juce::Rectangle<int> bounds)
{
    if (bounds.isEmpty())
        return;

    auto box =
        bounds.toFloat()
            .reduced (3.0f);

    g.setColour (
        juce::Colour::fromRGB (
            10,
            14,
            32
        ).withAlpha (0.62f)
    );

    g.fillRoundedRectangle (
        box,
        10.0f
    );

    g.setColour (
        jadLookAndFeel.colourD()
            .withAlpha (0.62f)
    );

    g.drawRoundedRectangle (
        box,
        10.0f,
        1.0f
    );

    auto text =
        bounds.reduced (14, 8);

    g.setFont (
        juce::FontOptions (13.0f)
    );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.96f)
    );

    g.drawText (
        "COMPUTER KEYS",
        text.removeFromTop (20),
        juce::Justification::centredLeft
    );

    text.removeFromTop (5);

    g.setColour (
        jadLookAndFeel.colourD()
            .interpolatedWith (
                juce::Colours::white,
                0.22f
            )
    );

    g.drawText (
        "   W E   T Y U        BLACK",
        text.removeFromTop (18),
        juce::Justification::centredLeft
    );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.94f)
    );

    g.drawText (
        "A S D F G H J K      WHITE",
        text.removeFromTop (18),
        juce::Justification::centredLeft
    );

    text.removeFromTop (8);

    g.setColour (
        juce::Colours::white
            .withAlpha (0.74f)
    );

    g.drawText (
        "Z  OCTAVE DOWN",
        text.removeFromTop (18),
        juce::Justification::centredLeft
    );

    g.drawText (
        "X  OCTAVE UP",
        text.removeFromTop (18),
        juce::Justification::centredLeft
    );

    text.removeFromTop (10);

    auto octaveRow =
        text.removeFromTop (20);

    const float centreY =
        static_cast<float> (
            octaveRow.getCentreY()
        );

    const float leftX =
        static_cast<float> (
            octaveRow.getX() + 5
        );

    const float rightX =
        static_cast<float> (
            octaveRow.getRight() - 5
        );

    g.setColour (
        jadLookAndFeel.colourA()
            .withAlpha (0.92f)
    );

    g.fillEllipse (
        leftX - 3.5f,
        centreY - 3.5f,
        7.0f,
        7.0f
    );

    g.fillEllipse (
        rightX - 3.5f,
        centreY - 3.5f,
        7.0f,
        7.0f
    );

    g.drawLine (
        leftX + 7.0f,
        centreY,
        rightX - 7.0f,
        centreY,
        1.0f
    );

    g.setColour (
        juce::Colour::fromRGB (
            10,
            14,
            32
        )
    );

    auto labelBounds =
        octaveRow.withSizeKeepingCentre (
            132,
            octaveRow.getHeight()
        );

    g.fillRect (
        labelBounds.reduced (2, 4)
    );

    g.setColour (
        juce::Colours::white
            .withAlpha (0.86f)
    );

    g.setFont (
        juce::FontOptions (10.5f)
    );

    g.drawText (
        "CURRENT OCTAVE",
        labelBounds,
        juce::Justification::centred
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

    drawOctaveMarkers (
        g,
        keyboardBounds
    );

    drawComputerKeyboardLegend (
        g,
        legendBounds
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
        header.removeFromRight (220)
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

    area.removeFromTop (12);

    auto performanceArea =
        area;

    constexpr int whiteKeyCount = 43;
    constexpr int gapWidth = 14;

    const int legendWidth =
        juce::jlimit (
            315,
            380,
            performanceArea.getWidth() / 4
        );

    legendBounds =
        performanceArea.removeFromRight (
            legendWidth
        );

    performanceArea.removeFromRight (
        gapWidth
    );

    const float fittedKeyWidth =
        static_cast<float> (
            performanceArea.getWidth()
        )
        / static_cast<float> (
            whiteKeyCount
        );

    keyboard.setKeyWidth (
        fittedKeyWidth
    );

    const int exactKeyboardWidth =
        juce::roundToInt (
            fittedKeyWidth
            * static_cast<float> (
                whiteKeyCount
            )
        );

    keyboardBounds =
        performanceArea.removeFromLeft (
            exactKeyboardWidth
        );

    keyboard.setBounds (
        keyboardBounds
    );

    repaint();
}