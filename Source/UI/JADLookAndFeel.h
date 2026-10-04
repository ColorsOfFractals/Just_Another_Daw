#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class JADLookAndFeel :
    public juce::LookAndFeel_V4
{
public:
    JADLookAndFeel();

    void setPaletteIndex (
        int newIndex
    ) noexcept;

    int getPaletteIndex() const noexcept;

    juce::Colour colourA() const;
    juce::Colour colourB() const;
    juce::Colour colourC() const;
    juce::Colour colourD() const;

    juce::Colour trackColour (
        int trackIndex
    ) const;

    void drawButtonBackground (
        juce::Graphics&,
        juce::Button&,
        const juce::Colour&,
        bool highlighted,
        bool down
    ) override;

    void drawRotarySlider (
        juce::Graphics&,
        int x,
        int y,
        int width,
        int height,
        float sliderPos,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider&
    ) override;

private:
    int paletteIndex = 0;
};
