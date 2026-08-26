#include "StartupOverlay.h"

#include <cmath>

namespace
{
    float smoothstep(float a, float b, float x)
    {
        x = juce::jlimit(
            0.0f,
            1.0f,
            (x - a) / juce::jmax(0.0001f, b - a)
        );

        return x * x * (3.0f - 2.0f * x);
    }

    juce::Colour pink()
    {
        return juce::Colour::fromRGB(255, 42, 155);
    }

    juce::Colour cyan()
    {
        return juce::Colour::fromRGB(42, 226, 255);
    }

    juce::Colour purple()
    {
        return juce::Colour::fromRGB(150, 76, 255);
    }

    juce::Colour gold()
    {
        return juce::Colour::fromRGB(255, 211, 95);
    }
}

StartupOverlay::StartupOverlay()
{
    setOpaque(true);

    createButton.onClick =
        [this]
        {
            if (stage != Stage::home)
                return;

            if (onCreateNewProject)
                onCreateNewProject();

            beginDissolve();
        };

    continueButton.onClick =
        [this]
        {
            if (stage != Stage::home)
                return;

            beginDissolve();
        };

    addAndMakeVisible(createButton);
    addAndMakeVisible(continueButton);

    createButton.setVisible(false);
    continueButton.setVisible(false);

    startTimerHz(60);
}

StartupOverlay::~StartupOverlay()
{
    stopTimer();
}

void StartupOverlay::beginDissolve()
{
    stage = Stage::dissolving;

    createButton.setEnabled(false);
    continueButton.setEnabled(false);
}

void StartupOverlay::timerCallback()
{
    elapsedSeconds += 1.0 / 60.0;

    phase += 0.025f;

    if (phase > juce::MathConstants<float>::twoPi)
        phase -= juce::MathConstants<float>::twoPi;

    pulse =
        0.5f
        + 0.5f * std::sin(phase * 0.7f);

    if (
        stage == Stage::boot
        && elapsedSeconds >= bootDuration
    )
    {
        stage = Stage::home;

        createButton.setVisible(true);
        continueButton.setVisible(true);

        createButton.setEnabled(true);
        continueButton.setEnabled(true);
    }

    if (stage == Stage::home)
    {
        homeAlpha =
            juce::jmin(
                1.0f,
                homeAlpha + 0.035f
            );
    }

    if (stage == Stage::dissolving)
    {
        dissolveAlpha =
            juce::jmax(
                0.0f,
                dissolveAlpha - 0.05f
            );

        if (dissolveAlpha <= 0.001f)
        {
            stopTimer();
            setVisible(false);
            return;
        }
    }

    repaint();
}

void StartupOverlay::drawBackdrop(
    juce::Graphics& g)
{
    auto bounds =
        getLocalBounds().toFloat();

    g.fillAll(
        juce::Colour::fromRGB(
            3,
            3,
            11
        )
    );

    juce::ColourGradient gradient(
        purple().withAlpha(
            0.20f * dissolveAlpha
        ),
        bounds.getTopLeft(),

        cyan().withAlpha(
            0.16f * dissolveAlpha
        ),
        bounds.getBottomRight(),

        false
    );

    gradient.addColour(
        0.45,
        pink().withAlpha(
            0.13f * dissolveAlpha
        )
    );

    g.setGradientFill(gradient);
    g.fillRect(bounds);
}

void StartupOverlay::drawWavetable(
    juce::Graphics& g,
    juce::Rectangle<float> area,
    float reveal,
    float staffGrowth)
{
    const auto centreY =
        area.getCentreY() + 55.0f;

    constexpr int points = 240;

    juce::Colour colours[4] = {
        pink(),
        purple(),
        cyan(),
        gold()
    };

    for (int wave = 0; wave < 4; ++wave)
    {
        juce::Path path;

        const auto amplitude =
            18.0f + wave * 9.0f;

        const auto frequency =
            2.0f + wave * 0.65f;

        for (int i = 0; i < points; ++i)
        {
            const auto t =
                static_cast<float>(i)
                / static_cast<float>(points - 1);

            if (t > reveal)
                break;

            const auto x =
                area.getX()
                + t * area.getWidth();

            const auto y =
                centreY
                + std::sin(
                    t
                    * juce::MathConstants<float>::twoPi
                    * frequency
                    + phase * (0.75f + wave * 0.20f)
                )
                * amplitude;

            if (i == 0)
                path.startNewSubPath(x, y);
            else
                path.lineTo(x, y);
        }

        g.setColour(
            colours[wave].withAlpha(
                0.10f * dissolveAlpha
            )
        );

        g.strokePath(
            path,
            juce::PathStrokeType(
                9.0f,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded
            )
        );

        g.setColour(
            colours[wave].withAlpha(
                (0.55f + pulse * 0.30f)
                * dissolveAlpha
            )
        );

        g.strokePath(
            path,
            juce::PathStrokeType(
                2.0f,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded
            )
        );
    }

    for (int i = 0; i < 16; ++i)
    {
        const auto t =
            static_cast<float>(i) / 15.0f;

        const auto x =
            area.getX()
            + t * area.getWidth();

        const auto sourceY =
            centreY
            + std::sin(
                t
                * juce::MathConstants<float>::twoPi
                * 3.0f
                + phase
            )
            * 22.0f;

        const auto targetY =
            area.getCentreY()
            - 80.0f
            + std::sin(
                t
                * juce::MathConstants<float>::pi
                * 5.0f
            )
            * 8.0f;

        const auto y =
            sourceY
            + (
                targetY - sourceY
            )
            * staffGrowth;

        juce::Path branch;

        branch.startNewSubPath(
            x,
            sourceY
        );

        branch.cubicTo(
            x + std::sin(phase + t * 9.0f)
                * 18.0f
                * staffGrowth,

            sourceY
                - 28.0f
                * staffGrowth,

            x - std::cos(phase + t * 11.0f)
                * 16.0f
                * staffGrowth,

            y + 18.0f,

            x,
            y
        );

        auto c =
            i % 2 == 0
                ? cyan()
                : pink();

        g.setColour(
            c.withAlpha(
                0.48f
                * staffGrowth
                * dissolveAlpha
            )
        );

        g.strokePath(
            branch,
            juce::PathStrokeType(
                1.2f,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded
            )
        );
    }
}

void StartupOverlay::drawGrandStaff(
    juce::Graphics& g,
    juce::Rectangle<float> area,
    float growth,
    float alpha)
{
    if (growth <= 0.0f)
        return;

    const auto width =
        area.getWidth()
        * 0.74f
        * growth;

    const auto left =
        area.getCentreX()
        - width * 0.5f;

    const auto topY =
        area.getCentreY()
        - 110.0f;

    const auto bottomY =
        area.getCentreY()
        - 10.0f;

    for (int staff = 0; staff < 2; ++staff)
    {
        const auto base =
            staff == 0
                ? topY
                : bottomY;

        for (int line = -2; line <= 2; ++line)
        {
            const auto y =
                base
                + line * 10.0f;

            auto c =
                staff == 0
                    ? cyan()
                    : purple();

            g.setColour(
                c.withAlpha(
                    0.65f
                    * alpha
                    * dissolveAlpha
                )
            );

            g.drawLine(
                left,
                y,
                left + width,
                y,
                1.15f
            );
        }
    }

    for (int note = 0; note < 12; ++note)
    {
        const auto t =
            static_cast<float>(note + 1)
            / 13.0f;

        const auto x =
            left + t * width;

        const auto upper =
            note % 2 == 0;

        const auto baseY =
            upper ? topY : bottomY;

        const auto y =
            baseY
            + (
                (
                    note * 3
                ) % 9
                - 4
            ) * 5.0f;

        auto c =
            note % 3 == 0
                ? gold()
                : (
                    upper
                        ? cyan()
                        : pink()
                );

        g.setColour(
            c.withAlpha(
                0.90f
                * growth
                * dissolveAlpha
            )
        );

        g.fillEllipse(
            x - 4.0f,
            y - 3.0f,
            8.0f,
            6.0f
        );

        g.drawLine(
            x + 4.0f,
            y,
            x + 4.0f,
            y - 26.0f,
            1.4f
        );
    }
}

void StartupOverlay::drawHomeCard(
    juce::Graphics& g,
    juce::Rectangle<float> area,
    float alpha)
{
    auto card =
        juce::Rectangle<float>(
            540.0f,
            300.0f
        );

    card.setCentre(
        area.getCentreX(),
        area.getCentreY() + 185.0f
    );

    juce::ColourGradient gradient(
        pink().withAlpha(
            0.20f * alpha
        ),
        card.getTopLeft(),

        cyan().withAlpha(
            0.13f * alpha
        ),
        card.getBottomRight(),

        false
    );

    g.setGradientFill(gradient);

    g.fillRoundedRectangle(
        card,
        24.0f
    );

    g.setColour(
        cyan().withAlpha(
            0.55f * alpha
        )
    );

    g.drawRoundedRectangle(
        card,
        24.0f,
        1.2f
    );

    auto text =
        card.reduced(
            30.0f,
            24.0f
        );

    g.setColour(
        juce::Colours::white.withAlpha(
            alpha
        )
    );

    g.setFont(
        juce::FontOptions(34.0f)
            .withStyle("Bold")
    );

    g.drawText(
        "JUST ANOTHER DAW",
        text.removeFromTop(48.0f),
        juce::Justification::centred
    );

    g.setColour(
        cyan().withAlpha(
            0.80f * alpha
        )
    );

    g.setFont(
        juce::FontOptions(12.0f)
    );

    g.drawText(
        "A SMALL STUDIO WITH A VERY LARGE ALPHABET",
        text.removeFromTop(30.0f),
        juce::Justification::centred
    );
}

void StartupOverlay::paint(
    juce::Graphics& g)
{
    drawBackdrop(g);

    auto area =
        getLocalBounds()
            .toFloat()
            .reduced(36.0f);

    const auto boot =
        juce::jlimit(
            0.0f,
            1.0f,
            static_cast<float>(
                elapsedSeconds
                / bootDuration
            )
        );

    const auto waveReveal =
        smoothstep(
            0.0f,
            0.35f,
            boot
        );

    const auto staffGrowth =
        smoothstep(
            0.22f,
            0.78f,
            boot
        );

    const auto staffAlpha =
        smoothstep(
            0.20f,
            0.65f,
            boot
        );

    drawWavetable(
        g,
        area,
        waveReveal,
        staffGrowth
    );

    drawGrandStaff(
        g,
        area,
        staffGrowth,
        staffAlpha
    );

    if (
        stage == Stage::home
        || stage == Stage::dissolving
    )
    {
        drawHomeCard(
            g,
            area,
            homeAlpha
        );
    }

    if (stage == Stage::boot)
    {
        juce::String status =
            "TUNING AUDIO SPACE";

        if (boot > 0.25f)
            status = "PHASING WAVETABLE";

        if (boot > 0.48f)
            status = "FRACTALIZING HARMONICS";

        if (boot > 0.70f)
            status = "ASSEMBLING GRAND STAFF";

        if (boot > 0.90f)
            status = "OPENING THE STUDIO";

        g.setColour(
            juce::Colours::white.withAlpha(
                0.55f * dissolveAlpha
            )
        );

        g.setFont(
            juce::FontOptions(11.0f)
        );

        g.drawText(
            status,
            getLocalBounds()
                .removeFromBottom(44),
            juce::Justification::centred
        );
    }
}

void StartupOverlay::resized()
{
    auto card =
        juce::Rectangle<int>(
            540,
            300
        );

    card.setCentre(
        getLocalBounds().getCentreX(),
        getLocalBounds().getCentreY() + 185
    );

    auto buttons =
        card.reduced(
            40,
            24
        );

    buttons.removeFromTop(150);

    createButton.setBounds(
        buttons.removeFromTop(48)
    );

    buttons.removeFromTop(10);

    continueButton.setBounds(
        buttons.removeFromTop(36)
    );

    createButton.setAlpha(
        homeAlpha * dissolveAlpha
    );

    continueButton.setAlpha(
        homeAlpha
        * 0.72f
        * dissolveAlpha
    );
}
