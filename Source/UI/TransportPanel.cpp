#include "TransportPanel.h"

#include <cmath>
#include <memory>


namespace
{
class MetronomeSettingsComponent final :
    public juce::Component
{
public:
    explicit MetronomeSettingsComponent (
        TransportState& transportState)
        : transport (transportState)
    {
        titleLabel.setText (
            "METRONOME",
            juce::dontSendNotification
        );

        titleLabel.setFont (
            juce::FontOptions (17.0f)
                .withStyle ("Bold")
        );

        subtitleLabel.setText (
            "Right-click control deck",
            juce::dontSendNotification
        );

        subtitleLabel.setColour (
            juce::Label::textColourId,
            juce::Colours::white
                .withAlpha (0.52f)
        );

        countInLabel.setText (
            "COUNT-IN",
            juce::dontSendNotification
        );

        countInLabel.setFont (
            juce::FontOptions (11.0f)
                .withStyle ("Bold")
        );

        countInBox.addItem (
            "OFF",
            1
        );

        countInBox.addItem (
            "1 BAR",
            2
        );

        countInBox.addItem (
            "2 BARS",
            3
        );

        countInBox.addItem (
            "4 BARS",
            4
        );

        const auto bars =
            transport.getCountInBars();

        countInBox.setSelectedId (
            bars <= 0
                ? 1
                : bars == 1
                    ? 2
                    : bars == 2
                        ? 3
                        : 4,
            juce::dontSendNotification
        );

        countInBox.onChange =
            [this]
            {
                const auto id =
                    countInBox.getSelectedId();

                transport.setCountInBars (
                    id == 1
                        ? 0
                        : id == 2
                            ? 1
                            : id == 3
                                ? 2
                                : 4
                );
            };

        soundLabel.setText (
            "CLICK SOUND",
            juce::dontSendNotification
        );

        soundLabel.setFont (
            juce::FontOptions (11.0f)
                .withStyle ("Bold")
        );

        soundBox.addItem (
            "DIGITAL TICK",
            1
        );

        soundBox.addItem (
            "WOOD BLOCK",
            2
        );

        soundBox.addItem (
            "STUDIO BEEP",
            3
        );

        soundBox.addItem (
            "NEON PULSE",
            4
        );

        soundBox.setSelectedId (
            transport.getMetronomeSound() + 1,
            juce::dontSendNotification
        );

        soundBox.onChange =
            [this]
            {
                transport.setMetronomeSound (
                    soundBox.getSelectedId() - 1
                );
            };

        playbackButton.setToggleState (
            transport
                .isClickDuringPlaybackEnabled(),
            juce::dontSendNotification
        );

        recordingButton.setToggleState (
            transport
                .isClickDuringRecordingEnabled(),
            juce::dontSendNotification
        );

        accentButton.setToggleState (
            transport
                .isAccentFirstBeatEnabled(),
            juce::dontSendNotification
        );

        playbackButton.onClick =
            [this]
            {
                transport.setClickDuringPlayback (
                    playbackButton.getToggleState()
                );
            };

        recordingButton.onClick =
            [this]
            {
                transport.setClickDuringRecording (
                    recordingButton.getToggleState()
                );
            };

        accentButton.onClick =
            [this]
            {
                transport.setAccentFirstBeat (
                    accentButton.getToggleState()
                );
            };

        volumeLabel.setText (
            "CLICK VOLUME",
            juce::dontSendNotification
        );

        volumeLabel.setFont (
            juce::FontOptions (11.0f)
                .withStyle ("Bold")
        );

        volumeSlider.setRange (
            0.0,
            1.0,
            0.01
        );

        volumeSlider.setValue (
            transport.getMetronomeVolume(),
            juce::dontSendNotification
        );

        volumeSlider.setSliderStyle (
            juce::Slider::LinearHorizontal
        );

        volumeSlider.setTextBoxStyle (
            juce::Slider::TextBoxRight,
            false,
            58,
            22
        );

        volumeSlider.onValueChange =
            [this]
            {
                transport.setMetronomeVolume (
                    static_cast<float> (
                        volumeSlider.getValue()
                    )
                );
            };

        closeButton.onClick =
            [this]
            {
                if (
                    auto* callout =
                        findParentComponentOfClass<
                            juce::CallOutBox
                        >()
                )
                {
                    callout->dismiss();
                }
            };

        for (
            auto* component :
            std::initializer_list<
                juce::Component*
            > {
                &titleLabel,
                &subtitleLabel,
                &countInLabel,
                &countInBox,
                &soundLabel,
                &soundBox,
                &playbackButton,
                &recordingButton,
                &accentButton,
                &volumeLabel,
                &volumeSlider,
                &closeButton
            }
        )
        {
            addAndMakeVisible (
                *component
            );
        }

        setSize (
            326,
            334
        );
    }

    void paint (
        juce::Graphics& g) override
    {
        const auto bounds =
            getLocalBounds()
                .toFloat();

        juce::ColourGradient background (
            juce::Colour::fromRGB (
                42,
                18,
                73
            ),
            bounds.getTopLeft(),
            juce::Colour::fromRGB (
                7,
                48,
                68
            ),
            bounds.getBottomRight(),
            false
        );

        g.setGradientFill (
            background
        );

        g.fillRoundedRectangle (
            bounds,
            16.0f
        );

        g.setColour (
            juce::Colour::fromRGB (
                255,
                38,
                137
            )
            .withAlpha (0.88f)
        );

        g.drawRoundedRectangle (
            bounds.reduced (1.0f),
            16.0f,
            2.0f
        );

        g.setColour (
            juce::Colours::white
                .withAlpha (0.08f)
        );

        g.fillRoundedRectangle (
            getLocalBounds()
                .reduced (14)
                .withTrimmedTop (62)
                .toFloat(),
            10.0f
        );
    }

    void resized() override
    {
        auto area =
            getLocalBounds()
                .reduced (16);

        auto header =
            area.removeFromTop (46);

        closeButton.setBounds (
            header.removeFromRight (34)
        );

        titleLabel.setBounds (
            header.removeFromTop (24)
        );

        subtitleLabel.setBounds (
            header
        );

        area.removeFromTop (10);

        auto countRow =
            area.removeFromTop (34);

        countInLabel.setBounds (
            countRow.removeFromLeft (104)
        );

        countInBox.setBounds (
            countRow
        );

        area.removeFromTop (8);

        auto soundRow =
            area.removeFromTop (34);

        soundLabel.setBounds (
            soundRow.removeFromLeft (104)
        );

        soundBox.setBounds (
            soundRow
        );

        area.removeFromTop (8);

        playbackButton.setBounds (
            area.removeFromTop (29)
        );

        recordingButton.setBounds (
            area.removeFromTop (29)
        );

        accentButton.setBounds (
            area.removeFromTop (29)
        );

        area.removeFromTop (8);

        volumeLabel.setBounds (
            area.removeFromTop (20)
        );

        volumeSlider.setBounds (
            area.removeFromTop (32)
        );
    }

private:
    TransportState& transport;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label countInLabel;
    juce::Label soundLabel;
    juce::Label volumeLabel;

    juce::ComboBox countInBox;
    juce::ComboBox soundBox;

    juce::ToggleButton playbackButton {
        "Click during playback"
    };

    juce::ToggleButton recordingButton {
        "Click during recording"
    };

    juce::ToggleButton accentButton {
        "Accent first beat"
    };

    juce::Slider volumeSlider;

    juce::TextButton closeButton {
        "X"
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        MetronomeSettingsComponent
    )
};
}


TransportPanel::TransportPanel (
    TransportState& transportState)
    : transport (transportState)
{
    addAndMakeVisible (stopButton);
    addAndMakeVisible (playButton);
    addAndMakeVisible (recordButton);
    addAndMakeVisible (loopButton);
    addAndMakeVisible (metronomeButton);

    stopButton.onClick =
        [this]
        {
            transport.stop();
            refreshState();
        };
    // PLAY / PAUSE TRANSPORT TOGGLE
    playButton.onClick = [this]
    {
        if (transport.isPlaying())
            transport.stop();
        else
            transport.play();

        refreshState();
    };

    recordButton.setClickingTogglesState (
        true
    );

    recordButton.onClick =
        [this]
        {
            if (
                recordButton.getToggleState()
            )
            {
                transport.beginRecording();
            }

            if (
                ! recordButton.getToggleState()
            )
            {
                transport.endRecording();
            }

            refreshState();
        };

    loopButton.setClickingTogglesState (
        true
    );

    loopButton.setToggleState (
        transport.isLoopEnabled(),
        juce::dontSendNotification
    );

    loopButton.onClick =
        [this]
        {
            transport.setLoopEnabled (
                loopButton.getToggleState()
            );

            repaint();
        };

    metronomeButton.setClickingTogglesState (
        true
    );

    metronomeButton.setToggleState (
        transport.isMetronomeEnabled(),
        juce::dontSendNotification
    );

    metronomeButton.onClick =
        [this]
        {
            transport.setMetronomeEnabled (
                metronomeButton.getToggleState()
            );

            refreshState();
        };

    metronomeButton.onRightClick =
        [this]
        {
            showMetronomeSettings();
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

    tempoSlider.onValueChange =
        [this]
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


void TransportPanel::showMetronomeSettings()
{
    auto settings =
        std::make_unique<
            MetronomeSettingsComponent
        > (
            transport
        );

    juce::CallOutBox::launchAsynchronously (
        std::move (settings),
        metronomeButton.getScreenBounds(),
        nullptr
    );
}


void TransportPanel::refreshState()
{
    playButton.setButtonText (
        transport.isPlaying()
            ? "PAUSE"
            : "PLAY"
    );

    recordButton.setToggleState (
        transport.isRecording(),
        juce::dontSendNotification
    );

    metronomeButton.setToggleState (
        transport.isMetronomeEnabled(),
        juce::dontSendNotification
    );

    metronomeButton.setButtonText (
        transport.isMetronomeEnabled()
            ? "MET ON"
            : "MET"
    );

    const auto beats =
        transport.getPositionInBeats();

    const auto numerator =
        juce::jmax (
            1,
            transport
                .getTimeSignatureNumerator()
        );

    const auto wholeBeat =
        static_cast<int> (
            std::floor (
                beats
            )
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
                .paddedLeft (
                    '0',
                    3
                ),
        juce::dontSendNotification
    );

    meterLabel.setText (
        juce::String (
            transport
                .getTimeSignatureNumerator()
        )
        + " / "
        + juce::String (
            transport
                .getTimeSignatureDenominator()
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
        getLocalBounds()
            .toFloat();

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
        getLocalBounds()
            .reduced (14);

    constexpr int buttonWidth = 82;

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

    area.removeFromLeft (8);

    metronomeButton.setBounds (
        area.removeFromLeft (
            82
        )
    );

    area.removeFromLeft (20);

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