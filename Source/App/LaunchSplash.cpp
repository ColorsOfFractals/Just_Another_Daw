#include "LaunchSplash.h"

#include <BinaryData.h>

#include <array>
#include <cmath>

namespace
{
constexpr double launchDurationMs = 2000.0;
constexpr double revealTimeMs = 1580.0;

juce::Colour cyan()
{
    return juce::Colour::fromRGB (
        32,
        218,
        235
    );
}

juce::Colour magenta()
{
    return juce::Colour::fromRGB (
        255,
        36,
        139
    );
}
}

LaunchSplash::LaunchSplash (
    std::function<void()> revealMainWindow,
    std::function<void()> launchFinished)
    : revealCallback (
        std::move (revealMainWindow)
    ),
      finishedCallback (
        std::move (launchFinished)
    )
{
    machineImage =
        juce::ImageCache::getFromMemory (
            BinaryData::JAD_png,
            BinaryData::JAD_pngSize
        );

    setOpaque (true);
    setAlwaysOnTop (true);
    setInterceptsMouseClicks (
        false,
        false
    );

    setSize (
        640,
        430
    );

    addToDesktop (
        juce::ComponentPeer::windowIsTemporary
        | juce::ComponentPeer::windowIgnoresKeyPresses
        | juce::ComponentPeer::windowHasDropShadow
    );

    centreWithSize (
        getWidth(),
        getHeight()
    );

    startTimeMs =
        juce::Time::getMillisecondCounterHiRes();

    setVisible (true);
    toFront (false);

    startTimerHz (60);
}

LaunchSplash::~LaunchSplash()
{
    stopTimer();
}

void LaunchSplash::paint (
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds()
            .toFloat();

    juce::ColourGradient background (
        juce::Colour::fromRGB (43, 5, 45),
        bounds.getTopLeft(),
        juce::Colour::fromRGB (4, 38, 58),
        bounds.getBottomRight(),
        false
    );

    background.addColour (
        0.48,
        juce::Colour::fromRGB (34, 28, 82)
    );

    g.setGradientFill (background);
    g.fillAll();

    auto frame =
        bounds.reduced (9.0f);

    g.setColour (
        cyan().withAlpha (0.58f)
    );

    g.drawRoundedRectangle (
        frame,
        25.0f,
        2.0f
    );

    auto glow =
        frame.reduced (4.0f);

    g.setColour (
        magenta().withAlpha (0.20f)
    );

    g.drawRoundedRectangle (
        glow,
        22.0f,
        7.0f
    );

    const float breathe =
        0.5f
        + 0.5f
        * std::sin (
            animationProgress
            * juce::MathConstants<float>::twoPi
            * 2.0f
        );

    auto imageArea =
        juce::Rectangle<int> (
            177,
            34,
            286,
            286
        );

    g.setColour (
        cyan().withAlpha (
            0.11f + breathe * 0.10f
        )
    );

    g.fillEllipse (
        imageArea.toFloat()
            .expanded (
                14.0f + breathe * 6.0f
            )
    );

    if (machineImage.isValid())
    {
        g.drawImageWithin (
            machineImage,
            imageArea.getX(),
            imageArea.getY(),
            imageArea.getWidth(),
            imageArea.getHeight(),
            juce::RectanglePlacement::centred,
            false
        );
    }

    // Stylized external crank overlay.
    const auto crankCentre =
        juce::Point<float> (
            435.0f,
            247.0f
        );

    const float crankAngle =
        animationProgress
        * juce::MathConstants<float>::twoPi
        * 4.0f;

    const auto crankJoint =
        crankCentre.getPointOnCircumference (
            31.0f,
            crankAngle
        );

    const auto crankHandle =
        crankJoint.getPointOnCircumference (
            20.0f,
            crankAngle
            + juce::MathConstants<float>::halfPi
        );

    juce::Path crankPath;
    crankPath.startNewSubPath (
        crankCentre
    );

    crankPath.lineTo (
        crankJoint
    );

    crankPath.lineTo (
        crankHandle
    );

    g.setColour (
        juce::Colours::black.withAlpha (0.50f)
    );

    g.strokePath (
        crankPath,
        juce::PathStrokeType (
            8.0f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    g.setColour (
        juce::Colour::fromRGB (
            236,
            175,
            55
        )
    );

    g.strokePath (
        crankPath,
        juce::PathStrokeType (
            4.0f,
            juce::PathStrokeType::curved,
            juce::PathStrokeType::rounded
        )
    );

    g.fillEllipse (
        juce::Rectangle<float> (
            13.0f,
            13.0f
        )
        .withCentre (
            crankCentre
        )
    );

    g.setColour (
        magenta()
    );

    g.fillEllipse (
        juce::Rectangle<float> (
            16.0f,
            16.0f
        )
        .withCentre (
            crankHandle
        )
    );

    // Notes rise from the horn at staggered speeds.
    const std::array<float, 5> offsets {
        0.00f,
        0.18f,
        0.36f,
        0.54f,
        0.72f
    };

    for (
        std::size_t index = 0;
        index < offsets.size();
        ++index)
    {
        const float noteProgress =
            std::fmod (
                animationProgress
                + offsets[index],
                1.0f
            );

        const float x =
            382.0f
            + static_cast<float> (index) * 20.0f
            + std::sin (
                noteProgress
                * juce::MathConstants<float>::twoPi
            ) * 17.0f;

        const float y =
            166.0f
            - noteProgress * 128.0f;

        const float alpha =
            std::sin (
                noteProgress
                * juce::MathConstants<float>::pi
            );

        g.setColour (
            (
                index % 2 == 0
                    ? cyan()
                    : magenta()
            )
            .withAlpha (
                juce::jlimit (
                    0.0f,
                    0.92f,
                    alpha
                )
            )
        );

        g.setFont (
            juce::FontOptions (
                27.0f
                + static_cast<float> (
                    index % 3
                ) * 4.0f
            )
            .withStyle ("Bold")
        );

        const auto noteCharacter =
            juce::String::charToString (
                index % 2 == 0
                    ? 0x266A
                    : 0x266B
            );

        g.drawText (
            noteCharacter,
            juce::Rectangle<float> (
                x,
                y,
                42.0f,
                42.0f
            ),
            juce::Justification::centred
        );
    }

    g.setColour (
        juce::Colours::white.withAlpha (
            0.82f + breathe * 0.18f
        )
    );

    g.setFont (
        juce::FontOptions (24.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        "LAUNCHING JAD...",
        40,
        334,
        getWidth() - 80,
        38,
        juce::Justification::centred
    );

    g.setColour (
        juce::Colours::white.withAlpha (0.50f)
    );

    g.setFont (
        juce::FontOptions (13.0f)
            .withStyle ("Bold")
    );

    g.drawText (
        "WINDING THE LITTLE MUSIC MACHINE",
        40,
        371,
        getWidth() - 80,
        24,
        juce::Justification::centred
    );

    auto progressTrack =
        juce::Rectangle<float> (
            120.0f,
            403.0f,
            400.0f,
            5.0f
        );

    g.setColour (
        juce::Colours::black.withAlpha (0.42f)
    );

    g.fillRoundedRectangle (
        progressTrack,
        2.5f
    );

    auto progressFill =
        progressTrack.withWidth (
            progressTrack.getWidth()
            * animationProgress
        );

    juce::ColourGradient progressGradient (
        magenta(),
        progressTrack.getTopLeft(),
        cyan(),
        progressTrack.getTopRight(),
        false
    );

    g.setGradientFill (
        progressGradient
    );

    g.fillRoundedRectangle (
        progressFill,
        2.5f
    );
}

void LaunchSplash::timerCallback()
{
    const double elapsed =
        juce::Time::getMillisecondCounterHiRes()
        - startTimeMs;

    animationProgress =
        juce::jlimit (
            0.0f,
            1.0f,
            static_cast<float> (
                elapsed / launchDurationMs
            )
        );

    if (
        ! mainWindowRevealed
        && elapsed >= revealTimeMs
    )
    {
        mainWindowRevealed = true;

        if (revealCallback)
            revealCallback();
    }

    if (elapsed >= revealTimeMs)
    {
        const auto fadeProgress =
            juce::jlimit (
                0.0f,
                1.0f,
                static_cast<float> (
                    (elapsed - revealTimeMs)
                    / (
                        launchDurationMs
                        - revealTimeMs
                    )
                )
            );

        setAlpha (
            1.0f - fadeProgress
        );
    }

    repaint();

    if (
        elapsed >= launchDurationMs
        && ! completionQueued
    )
    {
        completionQueued = true;
        stopTimer();

        juce::MessageManager::callAsync (
            [safeThis =
                juce::Component::SafePointer<LaunchSplash> (
                    this
                )]
            {
                if (safeThis != nullptr)
                    safeThis->finishLaunch();
            }
        );
    }
}

void LaunchSplash::finishLaunch()
{
    setVisible (false);

    if (finishedCallback)
        finishedCallback();
}