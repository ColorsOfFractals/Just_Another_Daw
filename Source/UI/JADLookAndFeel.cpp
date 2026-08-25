#include "JADLookAndFeel.h"

#include <cmath>

JADLookAndFeel::JADLookAndFeel()
{
    setColour (
        juce::Label::textColourId,
        juce::Colours::white
    );

    setColour (
        juce::ComboBox::textColourId,
        juce::Colours::white
    );

    setColour (
        juce::ComboBox::backgroundColourId,
        juce::Colour::fromRGB (22, 18, 40)
    );

    setColour (
        juce::ComboBox::outlineColourId,
        juce::Colour::fromRGB (130, 80, 255)
    );

    setColour (
        juce::Slider::textBoxTextColourId,
        juce::Colours::white
    );

    setColour (
        juce::Slider::textBoxBackgroundColourId,
        juce::Colour::fromRGB (20, 15, 34)
    );

    setColour (
        juce::Slider::textBoxOutlineColourId,
        juce::Colours::transparentBlack
    );
}

void JADLookAndFeel::setPaletteIndex (
    int newIndex) noexcept
{
    paletteIndex =
        juce::jlimit (
            0,
            3,
            newIndex
        );
}

int JADLookAndFeel::getPaletteIndex() const noexcept
{
    return paletteIndex;
}

juce::Colour JADLookAndFeel::colourA() const
{
    if (paletteIndex == 1)
        return juce::Colour::fromRGB (255, 92, 40);

    if (paletteIndex == 2)
        return juce::Colour::fromRGB (0, 220, 255);

    if (paletteIndex == 3)
        return juce::Colour::fromRGB (85, 255, 155);

    return juce::Colour::fromRGB (255, 35, 125);
}

juce::Colour JADLookAndFeel::colourB() const
{
    if (paletteIndex == 1)
        return juce::Colour::fromRGB (255, 205, 55);

    if (paletteIndex == 2)
        return juce::Colour::fromRGB (80, 110, 255);

    if (paletteIndex == 3)
        return juce::Colour::fromRGB (35, 210, 190);

    return juce::Colour::fromRGB (155, 55, 255);
}

juce::Colour JADLookAndFeel::colourC() const
{
    if (paletteIndex == 1)
        return juce::Colour::fromRGB (255, 55, 90);

    if (paletteIndex == 2)
        return juce::Colour::fromRGB (200, 75, 255);

    if (paletteIndex == 3)
        return juce::Colour::fromRGB (120, 255, 80);

    return juce::Colour::fromRGB (40, 125, 255);
}

juce::Colour JADLookAndFeel::colourD() const
{
    if (paletteIndex == 1)
        return juce::Colour::fromRGB (255, 120, 30);

    if (paletteIndex == 2)
        return juce::Colour::fromRGB (0, 245, 210);

    if (paletteIndex == 3)
        return juce::Colour::fromRGB (0, 180, 255);

    return juce::Colour::fromRGB (0, 225, 245);
}

juce::Colour JADLookAndFeel::trackColour (
    int trackIndex) const
{
    switch (trackIndex % 4)
    {
        case 0: return colourA();
        case 1: return juce::Colour::fromRGB (255, 155, 45);
        case 2: return colourD();
        default: return colourB();
    }
}

void JADLookAndFeel::drawButtonBackground (
    juce::Graphics& g,
    juce::Button& button,
    const juce::Colour&,
    bool highlighted,
    bool down)
{
    auto bounds =
        button.getLocalBounds()
            .toFloat()
            .reduced (1.0f);

    auto top =
        colourA()
            .withAlpha (
                down ? 0.95f : 0.72f
            );

    auto bottom =
        colourB()
            .withAlpha (
                highlighted ? 0.90f : 0.58f
            );

    juce::ColourGradient gradient (
        top,
        bounds.getTopLeft(),
        bottom,
        bounds.getBottomRight(),
        false
    );

    g.setGradientFill (gradient);

    g.fillRoundedRectangle (
        bounds,
        10.0f
    );

    g.setColour (
        colourD()
            .withAlpha (
                highlighted ? 0.90f : 0.35f
            )
    );

    g.drawRoundedRectangle (
        bounds,
        10.0f,
        highlighted ? 2.0f : 1.0f
    );
}

void JADLookAndFeel::drawRotarySlider (
    juce::Graphics& g,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float rotaryStartAngle,
    float rotaryEndAngle,
    juce::Slider&)
{
    auto bounds =
        juce::Rectangle<float> (
            static_cast<float> (x),
            static_cast<float> (y),
            static_cast<float> (width),
            static_cast<float> (height)
        )
        .reduced (7.0f);

    const auto radius =
        juce::jmin (
            bounds.getWidth(),
            bounds.getHeight()
        ) * 0.5f;

    const auto centre =
        bounds.getCentre();

    const auto angle =
        rotaryStartAngle
        + sliderPos
        * (rotaryEndAngle - rotaryStartAngle);

    juce::ColourGradient glow (
        colourA(),
        bounds.getTopLeft(),
        colourD(),
        bounds.getBottomRight(),
        false
    );

    g.setGradientFill (glow);

    g.fillEllipse (
        bounds.reduced (3.0f)
    );

    g.setColour (
        juce::Colour::fromRGB (
            12,
            10,
            25
        )
    );

    g.fillEllipse (
        bounds.reduced (8.0f)
    );

    juce::Path pointer;

    pointer.addRoundedRectangle (
        -2.0f,
        -radius + 13.0f,
        4.0f,
        radius * 0.48f,
        2.0f
    );

    g.setColour (
        juce::Colours::white
    );

    g.fillPath (
        pointer,
        juce::AffineTransform::rotation (
            angle
        )
        .translated (
            centre.x,
            centre.y
        )
    );
}
