#include "SessionOverviewPanel.h"

SessionOverviewPanel::SessionOverviewPanel (
    SessionState& sessionState)
    : session (sessionState)
{
}

void SessionOverviewPanel::paint (
    juce::Graphics& g)
{
    const auto bounds =
        getLocalBounds().toFloat();

    g.setColour (
        juce::Colour::fromRGB (
            9,
            15,
            23
        )
    );

    g.fillRoundedRectangle (
        bounds,
        12.0f
    );

    g.setColour (
        juce::Colour::fromRGB (
            75,
            200,
            220
        )
    );

    g.drawRoundedRectangle (
        bounds.reduced (1.0f),
        12.0f,
        1.5f
    );

    auto area =
        getLocalBounds().reduced (18);

    auto& project =
        session.getProjectState();

    auto& transport =
        session.getTransportState();

    auto& master =
        session.getMasterBusState();

    g.setColour (
        juce::Colours::white
    );

    g.setFont (18.0f);

    g.drawText (
        juce::String (
            project.getTitle()
        ),
        area.removeFromTop (28),
        juce::Justification::centredLeft
    );

    g.setFont (12.0f);

    g.setColour (
        juce::Colour::fromRGB (
            110,
            230,
            245
        )
    );

    g.drawText (
        "SESSION BACKBONE",
        area.removeFromTop (22),
        juce::Justification::centredLeft
    );

    area.removeFromTop (8);

    g.setColour (
        juce::Colours::lightgrey
    );

    const auto transportText =
        "TRANSPORT   "
        + juce::String (
            transport.getTempo(),
            1
        )
        + " BPM   "
        + juce::String (
            transport.getTimeSignatureNumerator()
        )
        + "/"
        + juce::String (
            transport.getTimeSignatureDenominator()
        );

    g.drawText (
        transportText,
        area.removeFromTop (24),
        juce::Justification::centredLeft
    );

    const auto masterText =
        "MASTER      gain "
        + juce::String (
            master.getGain(),
            2
        )
        + (
            master.isMuted()
                ? "   MUTED"
                : "   LIVE"
        );

    g.drawText (
        masterText,
        area.removeFromTop (24),
        juce::Justification::centredLeft
    );

    area.removeFromTop (10);

    for (
        std::size_t i = 0;
        i < session.getTrackCount();
        ++i
    )
    {
        auto* track =
            session.getTrack (i);

        if (track == nullptr)
            continue;

        auto row =
            area.removeFromTop (50);

        g.setColour (
            juce::Colour::fromRGB (
                15,
                25,
                34
            )
        );

        g.fillRoundedRectangle (
            row.toFloat(),
            7.0f
        );

        g.setColour (
            juce::Colour::fromRGB (
                55,
                120,
                145
            )
        );

        g.drawRoundedRectangle (
            row.toFloat(),
            7.0f,
            1.0f
        );

        auto inner =
            row.reduced (10, 5);

        g.setColour (
            juce::Colours::white
        );

        g.drawText (
            juce::String (
                track->getName()
            ),
            inner.removeFromLeft (95),
            juce::Justification::centredLeft
        );

        g.setColour (
            juce::Colour::fromRGB (
                110,
                220,
                240
            )
        );

        g.drawText (
            "ID "
                + juce::String (
                    static_cast<int> (
                        track->getId()
                    )
                ),
            inner.removeFromLeft (45),
            juce::Justification::centredLeft
        );

        g.setColour (
            juce::Colours::lightgrey
        );

        g.drawText (
            juce::String (
                static_cast<int> (
                    track->getClipCount()
                )
            )
                + " clip",
            inner.removeFromLeft (55),
            juce::Justification::centredLeft
        );

        const auto flags =
            juce::String (
                track->isMuted()
                    ? "M "
                    : "- "
            )
            + (
                track->isSolo()
                    ? "S "
                    : "- "
            )
            + (
                track->isRecordArmed()
                    ? "R"
                    : "-"
            );

        g.drawText (
            flags,
            inner,
            juce::Justification::centredRight
        );

        area.removeFromTop (7);
    }

    area.removeFromTop (8);

    g.setColour (
        juce::Colour::fromRGB (
            130,
            130,
            155
        )
    );

    g.drawText (
        "CLIPS / MASTER / PROJECT / TRACK FLEET",
        area.removeFromTop (22),
        juce::Justification::centred
    );
}
