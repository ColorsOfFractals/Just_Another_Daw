#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

#include "UpdateChecker.h"
#include "../Core/JADContext.h"

class LaunchSplash;

class JADApplication :
    public juce::JUCEApplication
{
public:
    JADApplication();
    ~JADApplication() override;

    const juce::String
    getApplicationName() override;

    const juce::String
    getApplicationVersion() override;

    void initialise (
        const juce::String&) override;

    void shutdown() override;

private:
    class MainWindow;

    void revealMainWindow();
    void completeLaunch();
    void beginUpdateCheck();

    void startSelfUpdate (
        const UpdateChecker::ReleaseInfo& release
    );

    JADContext context;

    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<LaunchSplash> launchSplash;
    std::unique_ptr<UpdateChecker> updateChecker;
};