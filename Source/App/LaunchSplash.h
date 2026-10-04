#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include <functional>

class LaunchSplash final :
    public juce::Component,
    private juce::Timer
{
public:
    LaunchSplash (
        std::function<void()> revealMainWindow,
        std::function<void()> launchFinished);

    ~LaunchSplash() override;

    void paint (juce::Graphics& g) override;

private:
    void timerCallback() override;
    void finishLaunch();

    std::function<void()> revealCallback;
    std::function<void()> finishedCallback;

    juce::Image machineImage;

    double startTimeMs = 0.0;
    float animationProgress = 0.0f;
    bool mainWindowRevealed = false;
    bool completionQueued = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (
        LaunchSplash
    )
};