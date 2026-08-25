#include "TransportPanel.h"

#include <cmath>

TransportPanel::TransportPanel (
    TransportState& transportState)
    : transport (transportState)
{
    addAndMakeVisible (stopButton);
    addAndMakeVisible (playButton);
    addAndMakeVisible (recordButton);
    addAndMakeVisible (loopButton);

    stopButton.onClick = [this]
    {
        transport.stop();
        refreshState();
    };

    playButton.onClick = [this]
    {
        transport.play();
        refreshState();
    };

    recordButton.setClickingTogglesState (
        true
    );

    recordButton.onClick = [this]
    {
        if (recordButton.getToggleState())
            transport.beginRecording();

        if (! recordButton.getToggleState())
            transport.endRecording();

        refreshState();
    };

    loopButton.setClickingTogglesState (
        true
    );

    loopButton.setToggleState (
        transport.isLoopEnabled(),
        juce::dontSendNotification
    );

    loopButton.onClick = [this]
    {
        transport.setLoopEnabled (
            loopButton.getToggleState()
        );

        repaint();
    };

    tempoSlider.setRange (
        20.0,
        400.0,
        0.1
    );

    tempoSlider.setValue (
        transport.getTempo(),
        juce::dontSendNotification
    );

    tempoSlider.setSliderStyle (
        juce::Slider::LinearHorizontal
    );

    tempoSlider.setTextBoxStyle (
        juce::Slider::TextBoxRight,
        false,
        72,
        24
    );

    tempoSlider.onValueChange = [this]
    {
        transport.setTempo (
            tempoSlider.getValue()
        );
    };

    addAndMakeVisible (
        tempoSlider
    );

    tempoLabel.setText (
        "BPM",
        juce::dontSendNotification
    );

    tempoLabel.setJustificationType (
        juce::Justification::centred
    );

    addAndMakeVisible (
        tempoLabel
    );

    positionLabel.setJustificationType (
        juce::Justification::centred
    );

    meterLabel.setJustificationType (
        juce::Justification::centred
    );

    addAndMakeVisible (
        positionLabel
    );

    addAndMakeVisible (
        meterLabel
    );

    startTimerHz (30);

    refreshState();
}

TransportPanel::~TransportPanel()
{
    stopTimer();
}

void TransportPanel::refreshState()
{
    playButton.setButtonText (
        transport.isPlaying()
            ? "PLAYING"
            : "PLAY"
    );

    recordButton.setToggleState (
        transport.isRecording(),
        juce::dontSendNotification
    );

    const auto beats =
        transport.getPositionInBeats();

    const auto numerator =
        juce::jmax (
            1,
            transport.getTimeSignatureNumerator()
        );

    const auto wholeBeat =
        static_cast<int> (
            std::floor (beats)
        );

    const auto bar =
        wholeBeat / numerator + 1;

    const auto beat =
        wholeBeat % numerator + 1;

    const auto fraction =
        static_cast<int> (
            std::floor (
                (
                    beats
                    - std::floor (beats)
                )
                * 1000.0
            )
        );

    positionLabel.setText (
        juce::String (bar)
            + " | "
            + juce::String (beat)
            + " | "
            + juce::String (fraction)
                .paddedLeft ('0', 3),
        juce::dontSendNotification
    );

    meterLabel.setText (
        juce::String (
            transport.getTimeSignatureNumerator()
        )
        + " / "
        + juce::String (
            transport.getTimeSignatureDenominator()
        ),
        juce::dontSendNotification
    );

    repaint();
}

void TransportPanel::timerCallback()
{
    refreshState();
}

void TransportPanel::paint (
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds().toFloat();

    juce::ColourGradient gradient (
        juce::Colour::fromRGB (
            8,
            8,
            10
        ),
        bounds.getX(),
        bounds.getCentreY(),

        juce::Colour::fromRGB (
            90,
            0,
            8
        ),
        bounds.getCentreX(),
        bounds.getCentreY(),

        false
    );

    gradient.addColour (
        0.72,
        juce::Colour::fromRGB (
            28,
            0,
            4
        )
    );

    gradient.addColour (
        1.0,
        juce::Colour::fromRGB (
            5,
            5,
            7
        )
    );

    g.setGradientFill (
        gradient
    );

    g.fillRoundedRectangle (
        bounds,
        10.0f
    );

    g.setColour (
        juce::Colour::fromRGB (
            180,
            25,
            35
        )
    );

    g.drawRoundedRectangle (
        bounds.reduced (1.0f),
        10.0f,
        1.4f
    );

    if (transport.isPlaying())
    {
        g.setColour (
            juce::Colour::fromRGB (
                255,
                55,
                65
            )
            .withAlpha (0.12f)
        );

        g.fillRoundedRectangle (
            bounds.reduced (5.0f),
            7.0f
        );
    }
}

void TransportPanel::resized()
{
    auto area =
        getLocalBounds().reduced (14);

    const int buttonWidth =
        86;

    stopButton.setBounds (
        area.removeFromLeft (
            buttonWidth
        )
    );

    area.removeFromLeft (8);

    playButton.setBounds (
        area.removeFromLeft (
            buttonWidth
        )
    );

    area.removeFromLeft (8);

    recordButton.setBounds (
        area.removeFromLeft (
            buttonWidth
        )
    );

    area.removeFromLeft (8);

    loopButton.setBounds (
        area.removeFromLeft (
            buttonWidth
        )
    );

    area.removeFromLeft (24);

    tempoLabel.setBounds (
        area.removeFromLeft (42)
    );

    tempoSlider.setBounds (
        area.removeFromLeft (190)
    );

    area.removeFromLeft (18);

    meterLabel.setBounds (
        area.removeFromLeft (75)
    );

    area.removeFromLeft (18);

    positionLabel.setBounds (
        area
    );
}
