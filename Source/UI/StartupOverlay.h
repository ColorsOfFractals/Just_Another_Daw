#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <functional>

class StartupOverlay :
    public juce::Component,
    private juce::Timer
{
public:
    StartupOverlay();
    ~StartupOverlay() override;

    void paint(juce::Graphics&) override;
    void resized() override;

    std::function<void()> onCreateNewProject;

private:
    enum class Stage
    {
        boot,
        home,
        dissolving
    };

    void timerCallback() override;
    void beginDissolve();

    void drawBackdrop(juce::Graphics&);
    void drawWavetable(
        juce::Graphics&,
        juce::Rectangle<float>,
        float reveal,
        float staffGrowth
    );

    void drawGrandStaff(
        juce::Graphics&,
        juce::Rectangle<float>,
        float growth,
        float alpha
    );

    void drawHomeCard(
        juce::Graphics&,
        juce::Rectangle<float>,
        float alpha
    );

    juce::TextButton createButton {
        "CREATE A NEW PROJECT"
    };

    juce::TextButton continueButton {
        "OPEN JAD"
    };

    Stage stage = Stage::boot;

    double elapsedSeconds = 0.0;

    float phase = 0.0f;
    float pulse = 0.0f;

    float homeAlpha = 0.0f;
    float dissolveAlpha = 1.0f;

    static constexpr double bootDuration = 4.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StartupOverlay)
};
